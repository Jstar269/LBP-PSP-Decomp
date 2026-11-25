#!/usr/bin/env python3
"""A small Allegrex (PSP MIPS) interpreter for the behavioural check of near misses (tools/lbpcheck.py).

    python tools/lbpinterp.py --self-test

Project-authored, written from the public MIPS32 instruction set reference and the published Allegrex extensions
(ext/ins, seb/seh, wsbh/wsbw, bitrev, clz/clo, min/max, madd/msub, rotr). It is not derived from any other
interpreter. It runs one function from a given state until control leaves the function's bytes, and refuses by name
everything it does not model, so a result is never made up:

  * the VFPU, COP0 and the Allegrex system instructions (halt, mfic, mtic) are refused;
  * syscall ends the run as an import boundary; break, a taken trap and add/addi/sub overflow end it as a stop;
  * division by zero and the signed overflow INT_MIN / -1 are refused (their hardware results are not modelled);
  * the FPU is IEEE-754 single precision with round to nearest even. A rounding mode other than nearest, or an
    enabled FPU exception, is refused; denormals are kept (not flushed), exception flags are not raised, every NaN
    result is 0x7fc00000, and an invalid float-to-integer conversion gives 0x7fffffff;
  * memory is only the regions the caller maps; any other access, and any unaligned word or halfword access, is a
    memory fault; a branch or jump in a delay slot is refused.

The run ends with one of these kinds: `return` (the next instruction is the return sentinel), `call` or `jump`
(control left the function through a linking or a plain transfer; `target` is where it went), `syscall` (the
function reached a syscall), or a model stop: `fault`, `limit` (the step budget ran out), `unsupported`, `trap`,
`break`. Every byte the run writes is recorded with its value before the run and after it.
"""
from __future__ import annotations

import math
import struct
import sys
from dataclasses import dataclass, field

MASK = 0xFFFFFFFF
RETURN_SENTINEL = 0xFFFFFFF0       # the return address a run starts with; fetching it ends the run as a return
REG_NAMES = ("zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
             "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra")
REG_INDEX = {name: i for i, name in enumerate(REG_NAMES)} | {f"r{i}": i for i in range(32)} | {"s8": 30}
COMPLETED = ("return", "call", "jump", "syscall")
CANONICAL_NAN = 0x7FC00000
FCC0 = 1 << 23
FCR31_BITS = 0x0183FFFF            # RM, flags, enables, cause, FCC0, FS: the bits an FCSR write keeps
INVALID_WORD = 0x7FFFFFFF


def sx16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def s32(v: int) -> int:
    v &= MASK
    return v - (1 << 32) if v & 0x80000000 else v


class Fault(Exception):
    def __init__(self, address: int, why: str):
        super().__init__(why)
        self.address = address & MASK
        self.why = why


class Refused(Exception):
    """An instruction or operand the interpreter does not model; kind is the stop kind."""

    def __init__(self, kind: str, detail: str):
        super().__init__(detail)
        self.kind = kind
        self.detail = detail


class Memory:
    """Mapped regions (base, bytearray), non-overlapping. Records every byte written: its value before the run."""

    def __init__(self) -> None:
        self.regions: list[tuple[int, bytearray]] = []
        self.before: dict[int, int] = {}

    def map(self, base: int, data: bytes | bytearray) -> None:
        end = base + len(data)
        for b, buf in self.regions:
            if base < b + len(buf) and b < end:
                if b <= base and end <= b + len(buf):     # wholly inside: the bytes replace what is there
                    buf[base - b:end - b] = data
                    return
                raise ValueError(f"region 0x{base:08x}-0x{end:08x} overlaps 0x{b:08x}-0x{b + len(buf):08x}")
        self.regions.append((base, bytearray(data)))
        self.regions.sort(key=lambda r: r[0])

    def _locate(self, address: int, size: int) -> tuple[bytearray, int]:
        for b, buf in self.regions:
            if b <= address and address + size <= b + len(buf):
                return buf, address - b
        raise Fault(address, f"{size}-byte access outside the modelled memory at 0x{address:08x}")

    def read(self, address: int, size: int) -> int:
        address &= MASK
        if size > 1 and address % size:
            raise Fault(address, f"unaligned {size}-byte read at 0x{address:08x}")
        buf, off = self._locate(address, size)
        return int.from_bytes(buf[off:off + size], "little")

    def write(self, address: int, size: int, value: int) -> None:
        address &= MASK
        if size > 1 and address % size:
            raise Fault(address, f"unaligned {size}-byte write at 0x{address:08x}")
        buf, off = self._locate(address, size)
        for i in range(size):
            self.before.setdefault(address + i, buf[off + i])
        buf[off:off + size] = (value & ((1 << (8 * size)) - 1)).to_bytes(size, "little")

    def byte(self, address: int) -> int | None:
        for b, buf in self.regions:
            if b <= address < b + len(buf):
                return buf[address - b]
        return None

    def written(self) -> dict[int, int]:
        return {a: self.byte(a) for a in sorted(self.before)}


@dataclass
class CpuState:
    gpr: list[int] = field(default_factory=lambda: [0] * 32)
    hi: int = 0
    lo: int = 0
    fpr: list[int] = field(default_factory=lambda: [0] * 32)
    fcr31: int = 0

    def copy(self) -> "CpuState":
        return CpuState(list(self.gpr), self.hi, self.lo, list(self.fpr), self.fcr31)


@dataclass
class Outcome:
    kind: str                     # see the module docstring
    state: CpuState               # registers when the run ended (for a model stop: before the stopping instruction)
    pc: int                       # the stopping instruction, or for return/call/jump the address control went to
    target: int | None = None     # call/jump: where control went; syscall: the syscall code
    detail: str = ""
    steps: int = 0
    before: dict[int, int] = field(default_factory=dict)   # every written byte: its value before the run
    after: dict[int, int] = field(default_factory=dict)    # and after it

    @property
    def completed(self) -> bool:
        return self.kind in COMPLETED


# ------------------------------------------------------------------------------------------------------------------
# single precision

def f32(bits: int) -> float:
    return struct.unpack("<f", struct.pack("<I", bits & MASK))[0]


def is_nan(bits: int) -> bool:
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x007FFFFF) != 0


def bits_of(value: float) -> int:
    if math.isnan(value):
        return CANONICAL_NAN
    try:
        return struct.unpack("<I", struct.pack("<f", value))[0]
    except OverflowError:                       # rounds past the largest single: an infinity of that sign
        return 0x7F800000 if value > 0 else 0xFF800000


def to_word(value: float, how: str) -> int:
    if math.isnan(value) or math.isinf(value):
        return INVALID_WORD
    if how == "trunc":
        n = math.trunc(value)
    elif how == "ceil":
        n = math.ceil(value)
    elif how == "floor":
        n = math.floor(value)
    else:                                       # nearest, ties to even (Python's round)
        n = round(value)
    if not -(1 << 31) <= n <= (1 << 31) - 1:
        return INVALID_WORD
    return n & MASK


def fdiv(a: float, b: float) -> float:
    if b == 0.0:
        if a == 0.0 or math.isnan(a):
            return math.nan
        return math.copysign(math.inf, a) * math.copysign(1.0, b)
    return a / b


# ------------------------------------------------------------------------------------------------------------------
# decoding helpers

def _rs(w: int) -> int: return (w >> 21) & 31
def _rt(w: int) -> int: return (w >> 16) & 31
def _rd(w: int) -> int: return (w >> 11) & 31
def _sa(w: int) -> int: return (w >> 6) & 31


VFPU_OPCODES = {0x12, 0x18, 0x19, 0x1B, 0x32, 0x34, 0x35, 0x36, 0x37, 0x3A, 0x3C, 0x3D, 0x3E, 0x3F}


def is_link_transfer(w: int) -> bool:
    """jal, jalr and the REGIMM and-link branches."""
    op = w >> 26
    if op == 3:
        return True
    if op == 0 and (w & 0x3F) == 9:
        return True
    return op == 1 and _rt(w) in (0x10, 0x11, 0x12, 0x13)


def is_transfer(w: int) -> bool:
    op = w >> 26
    if op == 0:
        return (w & 0x3F) in (8, 9)
    if op == 1:
        return _rt(w) in (0, 1, 2, 3, 0x10, 0x11, 0x12, 0x13)
    if op == 0x11:
        return _rs(w) == 8
    return op in (2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17)


class Interpreter:
    def __init__(self, memory: Memory, state: CpuState, start: int, end: int, max_steps: int = 100000):
        self.mem = memory
        self.s = state
        self.start, self.end = start, end
        self.max_steps = max_steps
        self.link_bit = False

    # registers -------------------------------------------------------------------------------------------------
    def r(self, i: int) -> int:
        return self.s.gpr[i]

    def setr(self, i: int, v: int) -> None:
        if i:
            self.s.gpr[i] = v & MASK

    def fpu_check(self) -> None:
        rm = self.s.fcr31 & 3
        if rm:
            raise Refused("unsupported", f"FPU rounding mode {rm} (only round to nearest is modelled)")
        if self.s.fcr31 & 0xF80:
            raise Refused("unsupported", "FPU exceptions enabled in FCR31 (FPU traps are not modelled)")

    # the run ---------------------------------------------------------------------------------------------------
    def run(self) -> Outcome:
        pc = self.start
        pending: int | None = None          # the delay slot's successor, while a delay slot runs
        linked = False                      # the transfer that set `pending` was a linking one
        steps = 0
        while True:
            if pending is None and not self.start <= pc < self.end:
                if pc == RETURN_SENTINEL:
                    return self._out("return", pc, steps)
                return self._out("call" if linked else "jump", pc, steps, target=pc)
            if steps >= self.max_steps:
                return self._out("limit", pc, steps, detail=f"step budget of {self.max_steps} instructions ran out")
            before = self.s.copy()
            try:
                w = self.mem.read(pc, 4)
                if pending is not None and is_transfer(w):
                    raise Refused("unsupported", f"branch or jump in a delay slot at 0x{pc:08x}")
                if (w >> 26) == 0 and (w & 0x3F) == 0x0C:
                    return self._out("syscall", pc, steps, target=(w >> 6) & 0xFFFFF)
                result = self.execute(w, pc)
            except Fault as e:
                self.s = before
                return self._out("fault", pc, steps, detail=e.why)
            except Refused as e:
                self.s = before
                return self._out(e.kind, pc, steps, detail=f"{e.detail} (instruction 0x{w:08x} at 0x{pc:08x})")
            steps += 1
            if pending is not None:
                pc, pending = pending, None
                continue
            if result is None:
                pc += 4
            elif result == "skip":              # a likely branch not taken annuls its delay slot
                pc += 8
            else:
                pending, linked = result
                pc += 4

    def _out(self, kind: str, pc: int, steps: int, target: int | None = None, detail: str = "") -> Outcome:
        before = dict(sorted(self.mem.before.items()))
        return Outcome(kind, self.s.copy(), pc, target, detail, steps, before, self.mem.written())

    # one instruction: None (fall through), "skip", or (successor after the delay slot, linking) --------------
    def execute(self, w: int, pc: int):
        op = w >> 26
        rs, rt, rd, sa = _rs(w), _rt(w), _rd(w), _sa(w)
        imm = w & 0xFFFF
        simm = sx16(imm)
        r, setr = self.r, self.setr
        if op == 0x00:
            return self.special(w, pc, rs, rt, rd, sa)
        if op == 0x01:
            v = s32(r(rs))
            target = (pc + 4 + (simm << 2)) & MASK
            conds = {0x00: v < 0, 0x01: v >= 0, 0x02: v < 0, 0x03: v >= 0,
                     0x10: v < 0, 0x11: v >= 0, 0x12: v < 0, 0x13: v >= 0}
            if rt not in conds:
                raise Refused("unsupported", f"REGIMM function 0x{rt:02x} (trap-immediate or reserved)")
            link = rt >= 0x10
            if link:
                setr(31, pc + 8)
            return self.branch(conds[rt], target, likely=rt in (0x02, 0x03, 0x12, 0x13), link=link, pc=pc)
        if op in (0x02, 0x03):
            target = ((pc + 4) & 0xF0000000) | ((w & 0x03FFFFFF) << 2)
            if op == 0x03:
                setr(31, pc + 8)
            return (target, op == 0x03)
        if op in (0x04, 0x05, 0x06, 0x07, 0x14, 0x15, 0x16, 0x17):
            a, b = r(rs), r(rt)
            if op in (0x06, 0x07, 0x16, 0x17) and rt:
                raise Refused("unsupported", "blez/bgtz with a nonzero rt field (reserved)")
            cond = {0x04: a == b, 0x05: a != b, 0x06: s32(a) <= 0, 0x07: s32(a) > 0}[op & 0x07]
            return self.branch(cond, (pc + 4 + (simm << 2)) & MASK, likely=op >= 0x14, link=False, pc=pc)
        if op == 0x08:                                          # addi
            v = s32(r(rs)) + simm
            if not -(1 << 31) <= v < (1 << 31):
                raise Refused("trap", "addi integer overflow trap")
            setr(rt, v)
            return None
        if op == 0x09:
            setr(rt, r(rs) + simm)
        elif op == 0x0A:
            setr(rt, 1 if s32(r(rs)) < simm else 0)
        elif op == 0x0B:
            setr(rt, 1 if r(rs) < (simm & MASK) else 0)
        elif op == 0x0C:
            setr(rt, r(rs) & imm)
        elif op == 0x0D:
            setr(rt, r(rs) | imm)
        elif op == 0x0E:
            setr(rt, r(rs) ^ imm)
        elif op == 0x0F:
            setr(rt, imm << 16)
        elif op == 0x10:
            raise Refused("unsupported", "COP0 (system control) instruction")
        elif op == 0x11:
            return self.cop1(w, pc, rs, rt, rd, sa)
        elif op == 0x1C:
            raise Refused("unsupported", "Allegrex system instruction (halt, mfic or mtic)")
        elif op == 0x1F:
            self.special3(w, rs, rt, rd, sa)
        elif op in (0x20, 0x21, 0x23, 0x24, 0x25, 0x30):
            addr = (r(rs) + simm) & MASK
            size = {0x20: 1, 0x24: 1, 0x21: 2, 0x25: 2, 0x23: 4, 0x30: 4}[op]
            v = self.mem.read(addr, size)
            if op == 0x20:
                v = v - 0x100 if v & 0x80 else v
            elif op == 0x21:
                v = sx16(v)
            setr(rt, v)
            if op == 0x30:
                self.link_bit = True
        elif op in (0x22, 0x26):                                 # lwl, lwr (little-endian)
            addr = (r(rs) + simm) & MASK
            k = addr & 3
            word = self.mem.read(addr & ~3, 4)
            old = r(rt)
            if op == 0x22:
                v = ((word << (8 * (3 - k))) & MASK) | (old & (MASK >> (8 * (k + 1))))
            else:
                v = (word >> (8 * k)) | (old & ~(MASK >> (8 * k)) & MASK)
            setr(rt, v)
        elif op in (0x28, 0x29, 0x2B):
            size = {0x28: 1, 0x29: 2, 0x2B: 4}[op]
            self.mem.write(r(rs) + simm, size, r(rt))
        elif op in (0x2A, 0x2E):                                 # swl, swr (little-endian)
            addr = (r(rs) + simm) & MASK
            k = addr & 3
            base = addr & ~3
            v = r(rt)
            if op == 0x2A:                                       # bytes base..addr get the top k+1 bytes of rt
                for i in range(k + 1):
                    self.mem.write(base + i, 1, v >> (8 * (3 - k + i)))
            else:                                                # bytes addr..base+3 get the low 4-k bytes of rt
                for i in range(4 - k):
                    self.mem.write(addr + i, 1, v >> (8 * i))
        elif op == 0x2F:                                         # cache: no architectural effect in this model
            pass
        elif op == 0x31:
            self.s.fpr[rt] = self.mem.read(r(rs) + simm, 4)
        elif op == 0x38:                                         # sc
            if self.link_bit:
                self.mem.write(r(rs) + simm, 4, r(rt))
                setr(rt, 1)
            else:
                setr(rt, 0)
            self.link_bit = False
        elif op == 0x39:
            self.mem.write(r(rs) + simm, 4, self.s.fpr[rt])
        elif op in VFPU_OPCODES:
            raise Refused("unsupported", f"VFPU instruction (opcode 0x{op:02x})")
        else:
            raise Refused("unsupported", f"reserved opcode 0x{op:02x}")
        return None

    def branch(self, cond: bool, target: int, likely: bool, link: bool, pc: int):
        if cond:
            return (target, link)
        if likely:
            return "skip"
        return ((pc + 8) & MASK, False)

    def special(self, w: int, pc: int, rs: int, rt: int, rd: int, sa: int):
        f = w & 0x3F
        r, setr, s = self.r, self.setr, self.s
        a, b = r(rs), r(rt)
        if f == 0x00:
            setr(rd, b << sa)
        elif f == 0x02:
            if rs > 1:
                raise Refused("unsupported", "srl with a reserved rs field")
            setr(rd, ((b >> sa) | (b << (32 - sa))) if rs == 1 and sa else (b >> sa))
        elif f == 0x03:
            setr(rd, s32(b) >> sa)
        elif f == 0x04:
            setr(rd, b << (a & 31))
        elif f == 0x06:
            if sa > 1:
                raise Refused("unsupported", "srlv with a reserved sa field")
            n = a & 31
            setr(rd, ((b >> n) | (b << (32 - n))) if sa == 1 and n else (b >> n))
        elif f == 0x07:
            setr(rd, s32(b) >> (a & 31))
        elif f == 0x08:
            return (a, False)
        elif f == 0x09:
            setr(rd, pc + 8)
            return (a, True)
        elif f == 0x0A:
            if b == 0:
                setr(rd, a)
        elif f == 0x0B:
            if b != 0:
                setr(rd, a)
        elif f == 0x0D:
            raise Refused("break", f"break instruction (code 0x{(w >> 6) & 0xFFFFF:x})")
        elif f == 0x0F:                                          # sync
            pass
        elif f == 0x10:
            setr(rd, s.hi)
        elif f == 0x11:
            s.hi = a
        elif f == 0x12:
            setr(rd, s.lo)
        elif f == 0x13:
            s.lo = a
        elif f == 0x16:
            setr(rd, 32 - a.bit_length())
        elif f == 0x17:
            setr(rd, 32 - ((~a) & MASK).bit_length())
        elif f in (0x18, 0x19, 0x1C, 0x1D, 0x2E, 0x2F):
            product = s32(a) * s32(b) if f in (0x18, 0x1C, 0x2E) else a * b
            acc = (s.hi << 32) | s.lo
            if f in (0x1C, 0x1D):
                product = acc + product
            elif f in (0x2E, 0x2F):
                product = acc - product
            product &= (1 << 64) - 1
            s.hi, s.lo = product >> 32, product & MASK
        elif f in (0x1A, 0x1B):
            if b == 0:
                raise Refused("unsupported", "division by zero (the hardware result is not modelled)")
            if f == 0x1A:
                x, y = s32(a), s32(b)
                if x == -(1 << 31) and y == -1:
                    raise Refused("unsupported", "signed division overflow (the hardware result is not modelled)")
                q = abs(x) // abs(y)
                q = -q if (x < 0) != (y < 0) else q
                s.lo, s.hi = q & MASK, (x - q * y) & MASK
            else:
                s.lo, s.hi = a // b, a % b
        elif f in (0x20, 0x22):
            v = s32(a) + s32(b) if f == 0x20 else s32(a) - s32(b)
            if not -(1 << 31) <= v < (1 << 31):
                raise Refused("trap", f"{'add' if f == 0x20 else 'sub'} integer overflow trap")
            setr(rd, v)
        elif f == 0x21:
            setr(rd, a + b)
        elif f == 0x23:
            setr(rd, a - b)
        elif f == 0x24:
            setr(rd, a & b)
        elif f == 0x25:
            setr(rd, a | b)
        elif f == 0x26:
            setr(rd, a ^ b)
        elif f == 0x27:
            setr(rd, ~(a | b))
        elif f == 0x2A:
            setr(rd, 1 if s32(a) < s32(b) else 0)
        elif f == 0x2B:
            setr(rd, 1 if a < b else 0)
        elif f == 0x2C:
            setr(rd, a if s32(a) > s32(b) else b)
        elif f == 0x2D:
            setr(rd, a if s32(a) < s32(b) else b)
        elif 0x30 <= f <= 0x36 and f != 0x35:
            cond = {0x30: s32(a) >= s32(b), 0x31: a >= b, 0x32: s32(a) < s32(b), 0x33: a < b,
                    0x34: a == b, 0x36: a != b}[f]
            if cond:
                raise Refused("trap", "conditional trap taken")
        else:
            raise Refused("unsupported", f"SPECIAL function 0x{f:02x} (reserved)")
        return None

    def special3(self, w: int, rs: int, rt: int, rd: int, sa: int) -> None:
        f = w & 0x3F
        a, b = self.r(rs), self.r(rt)
        if f == 0x00:                                            # ext rt, rs, pos, size
            size = rd + 1
            if sa + size > 32:
                raise Refused("unsupported", "ext with pos + size above 32")
            self.setr(rt, (a >> sa) & ((1 << size) - 1))
        elif f == 0x04:                                          # ins rt, rs, pos, size
            if rd < sa:
                raise Refused("unsupported", "ins with msb below pos")
            size = rd - sa + 1
            mask = ((1 << size) - 1) << sa
            self.setr(rt, (b & ~mask) | ((a << sa) & mask))
        elif f == 0x20:
            if sa == 0x02:                                       # wsbh
                self.setr(rd, ((b & 0x00FF00FF) << 8) | ((b >> 8) & 0x00FF00FF))
            elif sa == 0x03:                                     # wsbw
                self.setr(rd, int.from_bytes(b.to_bytes(4, "little"), "big"))
            elif sa == 0x10:                                     # seb
                self.setr(rd, (b & 0xFF) - 0x100 if b & 0x80 else b & 0xFF)
            elif sa == 0x18:                                     # seh
                self.setr(rd, sx16(b & 0xFFFF))
            elif sa == 0x14:                                     # bitrev
                self.setr(rd, int(f"{b:032b}"[::-1], 2))
            else:
                raise Refused("unsupported", f"BSHFL function 0x{sa:02x} (reserved)")
        else:
            raise Refused("unsupported", f"SPECIAL3 function 0x{f:02x} (reserved)")

    def cop1(self, w: int, pc: int, rs: int, rt: int, rd: int, sa: int):
        s = self.s
        fs, ft, fd = rd, rt, sa
        if rs == 0x00:
            self.setr(rt, s.fpr[fs])
        elif rs == 0x02:
            if fs != 31:
                raise Refused("unsupported", f"cfc1 of FPU control register {fs}")
            self.setr(rt, s.fcr31)
        elif rs == 0x04:
            s.fpr[fs] = self.r(rt)
        elif rs == 0x06:
            if fs != 31:
                raise Refused("unsupported", f"ctc1 to FPU control register {fs}")
            s.fcr31 = self.r(rt) & FCR31_BITS
        elif rs == 0x08:
            cond = bool(s.fcr31 & FCC0)
            nd_tf = rt & 3
            want = bool(nd_tf & 1)
            return self.branch(cond == want, (pc + 4 + (sx16(w & 0xFFFF) << 2)) & MASK, likely=bool(nd_tf & 2),
                               link=False, pc=pc)
        elif rs == 0x10:
            self.single(w & 0x3F, fs, ft, fd)
        elif rs == 0x14 and (w & 0x3F) == 0x20:                  # cvt.s.w
            self.fpu_check()
            s.fpr[fd] = bits_of(float(s32(s.fpr[fs])))
        else:
            raise Refused("unsupported", f"COP1 format 0x{rs:02x} function 0x{w & 0x3F:02x}")
        return None

    def single(self, f: int, fs: int, ft: int, fd: int) -> None:
        s = self.s
        xa, xb = s.fpr[fs], s.fpr[ft]
        if f == 0x05:                                            # abs.s, mov.s, neg.s: sign-bit operations
            s.fpr[fd] = xa & 0x7FFFFFFF
            return
        if f == 0x06:
            s.fpr[fd] = xa
            return
        if f == 0x07:
            s.fpr[fd] = xa ^ 0x80000000
            return
        self.fpu_check()
        a, b = f32(xa), f32(xb)
        if f == 0x00:
            s.fpr[fd] = bits_of(a + b)
        elif f == 0x01:
            s.fpr[fd] = bits_of(a - b)
        elif f == 0x02:
            s.fpr[fd] = bits_of(a * b)
        elif f == 0x03:
            s.fpr[fd] = bits_of(fdiv(a, b))
        elif f == 0x04:
            s.fpr[fd] = bits_of(math.nan if math.isnan(a) or a < 0 else math.sqrt(a))
        elif f in (0x0C, 0x0D, 0x0E, 0x0F, 0x24):
            how = {0x0C: "nearest", 0x0D: "trunc", 0x0E: "ceil", 0x0F: "floor", 0x24: "nearest"}[f]
            s.fpr[fd] = to_word(a, how)
        elif 0x30 <= f <= 0x3F:                                  # c.cond.s
            cond = f & 0x0F
            unordered = is_nan(xa) or is_nan(xb)
            result = bool(cond & 1) and unordered
            if not unordered:
                result = result or (bool(cond & 2) and a == b) or (bool(cond & 4) and a < b)
            s.fcr31 = (s.fcr31 | FCC0) if result else (s.fcr31 & ~FCC0)
        else:
            raise Refused("unsupported", f"COP1 single-precision function 0x{f:02x}")


def run(memory: Memory, state: CpuState, start: int, end: int, max_steps: int = 100000) -> Outcome:
    """Run the function at [start, end) from `state` (its ra should be RETURN_SENTINEL) over `memory`."""
    return Interpreter(memory, state.copy(), start, end, max_steps).run()


# ------------------------------------------------------------------------------------------------------------------

def self_test(quiet: bool = False) -> int:
    def word(*fields_shifts: tuple[int, int]) -> int:
        return sum(v << sh for v, sh in fields_shifts)

    mem = Memory()
    code = [
        word((9, 26), (4, 21), (2, 16), (5, 0)),               # addiu v0, a0, 5
        word((31, 21), (8, 0)),                                # jr ra
        0,                                                     # nop
    ]
    mem.map(0x08900000, b"".join(c.to_bytes(4, "little") for c in code))
    st = CpuState()
    st.gpr[4] = 10
    st.gpr[31] = RETURN_SENTINEL
    out = run(mem, st, 0x08900000, 0x0890000C)
    ok = out.kind == "return" and out.state.gpr[2] == 15
    if not quiet:
        print("lbpinterp self-test:", "OK" if ok else f"FAILED ({out})")
    return 0 if ok else 1


if __name__ == "__main__":
    if sys.argv[1:] == ["--self-test"]:
        sys.exit(self_test())
    print(__doc__)
    sys.exit(0 if sys.argv[1:] in ([], ["-h"], ["--help"]) else 2)
