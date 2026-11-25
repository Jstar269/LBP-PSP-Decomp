#!/usr/bin/env python3
"""Behavioural check of a near miss: does the candidate compute the same results as the original function?

    python tools/lbpcheck.py wip/<file>.c [more sources] [--states 200] [--seed 1] [--captured PATH]
                             [--returns int|int64|float|void] [--strict] [--interp builtin|nakagawa] [--json FILE]
    python tools/lbpcheck.py --raw ORIGINAL.bin CANDIDATE.bin --address 0x08900000 [same options]

A near miss compiles to code that differs from the retail bytes in a few instructions. lbpmatch.py can only say
EXACT or MISMATCH; this tool runs the original function's bytes and the candidate's linked bytes from the same
starting states and compares what each computes. The candidate side is what `python tools/lbpmatch.py <file>` last
linked at the retail address (`build/<module>/<key>/text.bin`, read as lbpnearmiss.py reads it, and its placed
.rodata/.data/.sdata); the original side is the retail function from your module. Both run over the same memory: the
module's load segments, a 64 KiB scratch region and a 64 KiB stack.

States:
  * random (`--states N`, default 200, `--seed S`): every register, HI/LO, every FPU register and the scratch and stack
    bytes are drawn from edge values (0, 1, -1, 0x80000000, 0x7fffffff, small and boundary integers), pointers into
    the scratch region and special floats (+-0, +-1, +-inf, NaN, the largest and smallest normal single, 2^24), so a
    pointer argument usually points at readable memory. State i of seed S is the same on every machine.
  * captured (`--captured PATH`, a JSON file or a directory of them; owner's machine only): starting states taken from
    a real run, in the format `psp-check-state.v1` (docs/GUIDE.md, "Behavioural check of a near miss"). A capture
    holds game memory: keep it under `try/` or outside the checkout, never commit it.

Each side runs until control leaves the function's bytes: it returns (to the return sentinel the run starts with in
ra), calls or jumps elsewhere (its first call, import or tail call), or reaches a syscall. Compared, in this order:
how control left (and where to), the registers, then every byte either side wrote. By default (the ABI view) the
registers are those a caller or callee can observe: at a return the return value (`--returns`, default `int`: v0;
`int64`: v0 and v1; `float`: f0; `void`: none), s0-s7, gp, sp, fp, ra and f20-f31; at a call or syscall the
argument registers a0-a3, t0-t3 and f12-f19, s0-s7, gp and fp (sp too at a tail jump). A register that points into
the function's own stack frame on both sides is not compared at a call, and the bytes of that frame (below the
entry sp) are not compared, because a different frame layout moves them without changing behaviour. `--strict`
compares every GPR (but ra at a call), HI/LO, every FPU register, FCR31 and every written byte, frame included.

Verdicts, one line per function:
  SAME_RESULTS  "behaves like the original on N states": no checked state differs. This is a separate tier and is
                NEVER counted as matched: only EXACT counts (AGENTS.md), so no ledger, proof or progress figure reads it.
  DIFFERENT     the first state that differs, and the first differing exit, register (in the order above) or address.
  NOT_CHECKED   nothing could be compared, with the reason (no build products, no configuration, an instruction the
                interpreter does not model, ...). A state the original side cannot finish (it leaves the modelled
                memory, runs out of steps, or reaches an unmodelled instruction) is not checked and is counted by
                reason; a state the candidate cannot finish where the original did is a difference.

The interpreter (`--interp builtin`, the default) is tools/lbpinterp.py, project-authored from the MIPS ISA, which
refuses by name what it does not model. `--interp nakagawa` runs both sides in Nakagawa Recomp's reference
interpreter instead (its `run_elf` program, run as a separate program: set LBPCHECK_RUN_ELF to a run_elf built with
`make run_elf` in your Nakagawa checkout); it models only its 64 MiB RAM at 0x08000000, which holds the EBOOT, the
scratch region and the stack, so LBP functions can run there; a function whose memory lies elsewhere is NOT_CHECKED.

Exit status: 0 when every function is SAME_RESULTS, 1 when any is DIFFERENT, 2 otherwise (not checked, usage).
"""
from __future__ import annotations

import argparse
import json
import os
import random
import re
import struct
import subprocess
import sys
import tempfile
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpinterp as I  # noqa: E402

TOOL = "lbpcheck"
RUN_ELF_ENV = "LBPCHECK_RUN_ELF"
BUILD_ROOT_ENV = "LBPMATCH_BUILD_ROOT"
STATE_FORMAT = "psp-check-state.v1"

SCRATCH_BASE, SCRATCH_SIZE = 0x0A000000, 0x10000
STACK_BASE, STACK_SIZE = 0x0A100000, 0x10000
ENTRY_SP = STACK_BASE + 0xC000           # 16 KiB of caller frame above, 48 KiB for the function below
FRAME_WINDOW = 0x10000                  # the bytes below the entry sp that count as the function's own frame
POINTER_SLACK = 0x1000                  # a random pointer leaves this much readable memory after it
MAX_STEPS = 100000
NAKAGAWA_RAM = (0x08000000, 0x0C000000)

EDGE_INTS = (0, 1, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 2, 3, 0xFFFFFFFE, 0x80000001, 0x7FFFFFFE,
             0x7FFF, 0x8000, 0xFFFF, 0x10000, 0xFFFF8000, 0x7F, 0x80, 0xFF, 0x100)
SPECIAL_FLOATS = (0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x7F800000, 0xFF800000,
                  0x7FC00000, 0x7F7FFFFF, 0xFF7FFFFF, 0x00800000, 0x80800000, 0x4B800000, 0x40490FDB)

ARG_GPRS = (4, 5, 6, 7, 8, 9, 10, 11)                 # a0-a3, t0-t3 (the PSP's EABI passes eight)
SAVED_GPRS = (16, 17, 18, 19, 20, 21, 22, 23, 28, 30)  # s0-s7, gp, fp
RETURN_GPRS = {"int": (2,), "int64": (2, 3), "float": (), "void": ()}
RETURN_FPRS = {"int": (), "int64": (), "float": (0,), "void": ()}
ARG_FPRS = tuple(range(12, 20))
SAVED_FPRS = tuple(range(20, 32))


class NotChecked(Exception):
    pass


@dataclass
class StartState:
    label: str
    cpu: I.CpuState
    blocks: list[tuple[int, bytes]]
    sp: int


@dataclass
class Function:
    label: str
    start: int
    original: bytes
    candidate: bytes
    segments: list[tuple[int, bytes]] = field(default_factory=list)   # the module's load segments (both sides)
    overlays: list[tuple[int, bytes]] = field(default_factory=list)   # the candidate's placed data sections
    names: dict[int, str] = field(default_factory=dict)


# ------------------------------------------------------------------------------------------------------------------
# starting states

def random_state(index: int, seed: int) -> StartState:
    rng = random.Random(f"psp-check:{seed}:{index}")   # the same states in both decomps
    profile = index % 4                  # 0: pointer arguments, 1: edge values, 2: anything, 3: small integers

    def pointer() -> int:
        return SCRATCH_BASE + rng.randrange(0, SCRATCH_SIZE - POINTER_SLACK, 4)

    def value(arg: bool) -> int:
        x = rng.random()
        if profile == 0 and arg:
            return pointer() if x < 0.85 else rng.choice(EDGE_INTS)
        if profile == 1:
            return rng.choice(EDGE_INTS) if x < 0.8 else pointer()
        if profile == 3:
            return rng.randrange(-8, 64) & I.MASK if x < 0.8 else pointer()
        if x < 0.35:
            return rng.choice(EDGE_INTS)
        if x < 0.7:
            return pointer()
        return rng.getrandbits(32)

    def float_word() -> int:
        x = rng.random()
        if x < 0.5:
            return rng.choice(SPECIAL_FLOATS)
        if x < 0.85:
            return I.bits_of(rng.uniform(-1000.0, 1000.0))
        return rng.getrandbits(32)

    cpu = I.CpuState()
    for i in range(1, 32):
        cpu.gpr[i] = value(i in ARG_GPRS)
    cpu.gpr[29] = ENTRY_SP
    cpu.gpr[31] = I.RETURN_SENTINEL
    cpu.hi, cpu.lo = value(False), value(False)
    cpu.fpr = [float_word() for _ in range(32)]
    cpu.fcr31 = I.FCC0 if rng.random() < 0.5 else 0

    def fill(size: int) -> bytes:
        out = bytearray()
        for _ in range(size // 4):
            x = rng.random()
            w = pointer() if x < 0.25 else rng.choice(EDGE_INTS) if x < 0.5 else float_word() if x < 0.6 \
                else rng.getrandbits(32)
            out += struct.pack("<I", w)
        return bytes(out)

    return StartState(f"state {index}", cpu, [(SCRATCH_BASE, fill(SCRATCH_SIZE)), (STACK_BASE, fill(STACK_SIZE))],
                      ENTRY_SP)


def _word(text, what: str) -> int:
    if isinstance(text, int):
        v = text
    elif isinstance(text, str) and re.fullmatch(r"(0x)?[0-9A-Fa-f]{1,8}", text.strip()):
        v = int(text.strip(), 16)
    else:
        raise NotChecked(f"{what}: {text!r} is not a 32-bit hex value")
    if not 0 <= v <= I.MASK:
        raise NotChecked(f"{what}: {text!r} is not a 32-bit value")
    return v


def captured_states(path: Path) -> list[StartState]:
    """Starting states in the psp-check-state.v1 format: one object (or a list of them) per file."""
    files = sorted(path.glob("*.json")) if path.is_dir() else [path]
    out: list[StartState] = []
    for f in files:
        data = json.loads(f.read_text(encoding="utf-8"))
        for n, item in enumerate(data if isinstance(data, list) else [data]):
            where = f"{f.name}[{n}]"
            if not isinstance(item, dict) or item.get("format") != STATE_FORMAT:
                raise NotChecked(f"{where}: not a {STATE_FORMAT} state")
            cpu = I.CpuState()
            for name, v in (item.get("gpr") or {}).items():
                if name not in I.REG_INDEX:
                    raise NotChecked(f"{where}: unknown register {name!r}")
                cpu.gpr[I.REG_INDEX[name]] = _word(v, f"{where} {name}")
            for name, v in (item.get("fpr") or {}).items():
                m = re.fullmatch(r"f(\d+)", name)
                if not m or int(m.group(1)) > 31:
                    raise NotChecked(f"{where}: unknown FPU register {name!r}")
                cpu.fpr[int(m.group(1))] = _word(v, f"{where} {name}")
            cpu.hi = _word(item.get("hi", 0), f"{where} hi")
            cpu.lo = _word(item.get("lo", 0), f"{where} lo")
            cpu.fcr31 = _word(item.get("fcr31", 0), f"{where} fcr31") & I.FCR31_BITS
            cpu.gpr[0] = 0
            cpu.gpr[31] = I.RETURN_SENTINEL      # the captured return address means nothing here
            blocks = []
            for b in item.get("memory") or []:
                hexdata = b.get("hex", "")
                if not re.fullmatch(r"(?:[0-9A-Fa-f]{2})*", hexdata):
                    raise NotChecked(f"{where}: a memory block's hex is not bytes")
                blocks.append((_word(b.get("address"), f"{where} memory address"), bytes.fromhex(hexdata)))
            out.append(StartState(f"captured {where}", cpu, blocks, cpu.gpr[29]))
    return out


# ------------------------------------------------------------------------------------------------------------------
# running one side

def side_memory(fn: Function, state: StartState, candidate: bool) -> tuple[I.Memory, int]:
    """The memory a side starts with, and the end of its code."""
    mem = I.Memory()
    code = fn.candidate if candidate else fn.original
    try:
        for addr, data in fn.segments:
            mem.map(addr, data)
        for addr, data in state.blocks:
            mem.map(addr, data)
        if candidate:
            for addr, data in fn.overlays:
                mem.map(addr, data)
        mem.map(fn.start, code)
    except ValueError as e:
        raise NotChecked(f"the memory map does not fit: {e}") from None
    return mem, fn.start + len(code)


def run_builtin(fn: Function, state: StartState, candidate: bool, max_steps: int) -> I.Outcome:
    mem, end = side_memory(fn, state, candidate)
    return I.run(mem, state.cpu, fn.start, end, max_steps)


class Nakagawa:
    """Nakagawa Recomp's reference interpreter, driven as an external program: its run_elf loads a non-relocated
    ELF, seeds the registers from a `# init` line and writes a per-instruction trace (its tools/TRACE_FORMAT.md)."""

    STOP = re.compile(r"run_elf: stop_reason=(\S+) pc=0x([0-9a-fA-F]+) op=0x([0-9a-fA-F]+) executed=(\d+)")

    def __init__(self, exe: Path):
        self.exe = exe

    @staticmethod
    def elf(entry: int, regions: list[tuple[int, bytes]]) -> bytes:
        phoff, n = 52, len(regions)
        data_off = phoff + 32 * n
        head = struct.pack("<4sBBBB8xHHIIIIIHHHHHH", b"\x7fELF", 1, 1, 1, 0, 2, 8, 1, entry, phoff, 0, 0,
                           52, 32, n, 40, 0, 0)
        phs, blobs, off = b"", b"", data_off
        for addr, data in regions:
            phs += struct.pack("<8I", 1, off, addr, addr, len(data), len(data), 7, 4)
            blobs += data
            off += len(data)
        return head + phs + blobs

    def run(self, fn: Function, state: StartState, candidate: bool, max_steps: int) -> I.Outcome:
        mem, end = side_memory(fn, state, candidate)
        regions = [(b, bytes(buf)) for b, buf in mem.regions]
        for b, data in regions:
            if not (NAKAGAWA_RAM[0] <= b and b + len(data) <= NAKAGAWA_RAM[1]):
                raise NotChecked(f"Nakagawa's reference interpreter models RAM 0x{NAKAGAWA_RAM[0]:08x}-"
                                 f"0x{NAKAGAWA_RAM[1] - 1:08x} only; this function's memory includes "
                                 f"0x{b:08x}-0x{b + len(data) - 1:08x}")
        cpu = state.cpu
        init = ["# init"] + [f"r{i}=0x{cpu.gpr[i]:08x}" for i in range(1, 32)] + \
               [f"f{i}=0x{cpu.fpr[i]:08x}" for i in range(32)] + \
               [f"hi=0x{cpu.hi:08x}", f"lo=0x{cpu.lo:08x}", f"fcr31=0x{cpu.fcr31:08x}"]
        with tempfile.TemporaryDirectory(prefix=f"{TOOL}-") as tmp:
            t = Path(tmp)
            (t / "f.elf").write_bytes(self.elf(fn.start, regions))
            (t / "init.trace").write_text(" ".join(init) + "\n", encoding="ascii")
            env = dict(os.environ, SR_BREAK_FATAL="1")
            try:
                proc = subprocess.run([str(self.exe), str(t / "f.elf"), str(t / "init.trace"), str(t / "out.trace"),
                                       str(max_steps)], capture_output=True, text=True, timeout=120, env=env)
            except (OSError, subprocess.TimeoutExpired) as e:
                raise NotChecked(f"run_elf could not be run: {e}") from None
            m = self.STOP.search(proc.stderr)
            if not m or not (t / "out.trace").exists():
                tail = proc.stderr.strip().splitlines()[-1:] or ["no output"]
                raise NotChecked(f"run_elf gave no stop line ({tail[0]})")
            lines = (t / "out.trace").read_text(encoding="ascii", errors="replace").splitlines()
        return self.outcome(fn.start, end, cpu, mem, lines, m.group(1), int(m.group(2), 16), int(m.group(3), 16))

    @staticmethod
    def outcome(start: int, end: int, init: I.CpuState, mem: I.Memory, lines: list[str], reason: str, stop_pc: int,
                stop_op: int) -> I.Outcome:
        s = init.copy()
        writes: dict[int, int] = {}
        ops: list[int] = []
        steps = 0

        def finish(kind: str, pc: int, target: int | None = None, detail: str = "") -> I.Outcome:
            before = {a: mem.byte(a) for a in sorted(writes)}
            return I.Outcome(kind, s, pc, target, detail, steps, before, dict(sorted(writes.items())))

        for line in lines:
            if not line or line.startswith("#"):
                continue
            tokens = line.split()
            pc, op = int(tokens[1][3:], 16), int(tokens[2][3:], 16)
            if not start <= pc < end:          # control left the function: the transfer was two steps back
                linked = len(ops) >= 2 and I.is_link_transfer(ops[-2])
                return finish("call" if linked else "jump", pc, target=pc)
            for tok in tokens[3:]:
                name, value = tok.split("=", 1)
                v = int(value, 16)
                if name.startswith("m"):
                    size, addr = re.fullmatch(r"m(8|16|32)\[0x([0-9a-fA-F]+)\]", name).groups()
                    for i in range(int(size) // 8):
                        writes[int(addr, 16) + i] = (v >> (8 * i)) & 0xFF
                elif name == "hi":
                    s.hi = v
                elif name == "lo":
                    s.lo = v
                elif name == "fcr31":
                    s.fcr31 = v
                elif name[0] == "r":
                    s.gpr[int(name[1:])] = v
                elif name[0] == "f":
                    s.fpr[int(name[1:])] = v
            ops.append(op)
            steps += 1
        if reason == "returned":
            return finish("return", I.RETURN_SENTINEL)
        if reason == "memory-fault" and not start <= stop_pc < end:
            linked = len(ops) >= 2 and I.is_link_transfer(ops[-2])
            return finish("call" if linked else "jump", stop_pc, target=stop_pc)
        if reason == "syscall":
            return finish("syscall", stop_pc, target=(stop_op >> 6) & 0xFFFFF)
        kind = {"memory-fault": "fault", "unimplemented": "unsupported", "break": "break",
                "step-limit": "limit"}.get(reason, "unsupported")
        return finish(kind, stop_pc, detail=f"Nakagawa's interpreter stopped: {reason} at instruction "
                                            f"0x{stop_op:08x} at 0x{stop_pc:08x}")


# ------------------------------------------------------------------------------------------------------------------
# comparing two outcomes

def _reg(i: int) -> str:
    return I.REG_NAMES[i]


def exit_text(o: I.Outcome, names: dict[int, str]) -> str:
    if o.kind == "return":
        return "returned"
    if o.kind == "syscall":
        return f"reached syscall 0x{o.target:x}"
    if o.kind in ("call", "jump"):
        name = names.get(o.target)
        return f"{'called' if o.kind == 'call' else 'jumped to'} 0x{o.target:08x}" + (f" ({name})" if name else "")
    return f"stopped ({o.kind}): {o.detail}"


def compare(o: I.Outcome, c: I.Outcome, entry_sp: int, strict: bool, returns: str,
            names: dict[int, str]) -> str | None:
    """None when the two completed runs agree; otherwise the first difference, in words."""
    if o.kind != c.kind or o.target != c.target:
        return f"exit: original {exit_text(o, names)}, candidate {exit_text(c, names)}"
    frame = (entry_sp - FRAME_WINDOW, entry_sp)
    at_call = o.kind in ("call", "syscall")
    if strict:
        gprs = [i for i in range(1, 32) if not (at_call and i == 31)]
        fprs = list(range(32))
    elif o.kind == "return":
        gprs = [*RETURN_GPRS[returns], *SAVED_GPRS, 29, 31]
        fprs = [*RETURN_FPRS[returns], *SAVED_FPRS]
    else:
        gprs = [*ARG_GPRS, *SAVED_GPRS] + ([29] if o.kind == "jump" else [])
        fprs = [*ARG_FPRS]
    for i in sorted(gprs):
        a, b = o.state.gpr[i], c.state.gpr[i]
        if a != b and not (not strict and at_call and frame[0] <= a < frame[1] and frame[0] <= b < frame[1]):
            return f"{_reg(i)}: original 0x{a:08x}, candidate 0x{b:08x}"
    if strict:
        for name in ("hi", "lo"):
            a, b = getattr(o.state, name), getattr(c.state, name)
            if a != b:
                return f"{name}: original 0x{a:08x}, candidate 0x{b:08x}"
    for i in sorted(fprs):
        a, b = o.state.fpr[i], c.state.fpr[i]
        if a != b:
            return f"f{i}: original 0x{a:08x} ({I.f32(a)!r}), candidate 0x{b:08x} ({I.f32(b)!r})"
    if strict and o.state.fcr31 != c.state.fcr31:
        return f"fcr31: original 0x{o.state.fcr31:08x}, candidate 0x{c.state.fcr31:08x}"
    for a in sorted(set(o.after) | set(c.after)):
        if not strict and frame[0] <= a < frame[1]:
            continue
        initial = o.before.get(a, c.before.get(a))
        x, y = o.after.get(a, initial), c.after.get(a, initial)
        if x != y:
            return f"memory 0x{a:08x}: original 0x{x:02x}, candidate 0x{y:02x} (" + \
                   ("both wrote it" if a in o.after and a in c.after else
                    "only the original wrote it" if a in o.after else "only the candidate wrote it") + ")"
    return None


def reason_key(side: str, o: I.Outcome) -> str:
    detail = re.split(r" \(instruction | at 0x", o.detail, maxsplit=1)[0]
    return f"{side} {o.kind}: {detail}" if detail else f"{side} {o.kind}"


@dataclass
class Report:
    label: str
    verdict: str
    text: str
    states: int = 0
    same: int = 0
    different: int = 0
    not_checked: Counter = field(default_factory=Counter)
    examples: dict[str, str] = field(default_factory=dict)
    first_difference: dict | None = None
    interpreter: str = "builtin"

    def line(self) -> str:
        return f"{self.verdict:<12} {self.label}: {self.text}"

    def as_json(self) -> dict:
        return {"source": self.label, "verdict": self.verdict, "text": self.text, "interpreter": self.interpreter,
                "states": self.states, "same": self.same, "different": self.different,
                "not_checked": dict(self.not_checked), "not_checked_examples": self.examples,
                "first_difference": self.first_difference}


def check(fn: Function, states: list[StartState], runner, strict: bool = False, returns: str = "int",
          max_steps: int = MAX_STEPS, interpreter: str = "builtin") -> Report:
    rep = Report(fn.label, "NOT_CHECKED", "", states=len(states), interpreter=interpreter)
    try:
        for st in states:
            o = runner(fn, st, False, max_steps)
            if not o.completed:
                key = reason_key("original", o)
                rep.not_checked[key] += 1
                rep.examples.setdefault(key, o.detail)
                continue
            c = runner(fn, st, True, max_steps)
            if not c.completed and c.kind == "unsupported":
                key = reason_key("candidate", c)
                rep.not_checked[key] += 1
                rep.examples.setdefault(key, c.detail)
                continue
            diff = compare(o, c, st.sp, strict, returns, fn.names)
            if diff is None:
                rep.same += 1
                continue
            rep.different += 1
            if rep.first_difference is None:
                rep.first_difference = {"state": st.label, "difference": diff,
                                        "original_exit": exit_text(o, fn.names), "candidate_exit": exit_text(c, fn.names)}
    except NotChecked as e:
        rep.text = str(e)
        return rep
    skipped = sum(rep.not_checked.values())
    reasons = "; ".join(f"{n} {k}" for k, n in rep.not_checked.most_common(3))
    if rep.different:
        d = rep.first_difference
        rep.verdict = "DIFFERENT"
        rep.text = (f"{d['state']} of {len(states)}: first difference {d['difference']}; "
                    f"{rep.different} of {rep.different + rep.same} checked states differ")
    elif rep.same:
        rep.verdict = "SAME_RESULTS"
        rep.text = (f"same results on all {rep.same} checked states: behaves like the original on {rep.same} states"
                    + (f" ({skipped} not checked: {reasons})" if skipped else "")
                    + ". Not a match: only EXACT counts")
    else:
        rep.text = f"none of the {len(states)} states could be checked: {reasons}" if states else "no states to run"
        first = next(iter(rep.examples.values()), "")
        if first:
            rep.text += f" (first: {first})"
    if fn.original == fn.candidate and rep.verdict != "DIFFERENT":
        rep.text += " (the candidate's bytes are identical to the original; lbpmatch.py gives the verdict)"
    return rep


# ------------------------------------------------------------------------------------------------------------------
# getting the two sides of a source

def source_function(path: Path) -> Function:
    """Both sides of a source from lbpmatch.py's build products and the retail module (needs lbp-config.json)."""
    import lbpmatch as M
    import lbpnearmiss as N
    try:
        h = M.parse_header(path)
    except (ValueError, OSError) as e:
        raise NotChecked(f"no usable LBPMATCH header: {e}") from None
    if h.get("helpers"):
        raise NotChecked("a helpers= source (several functions at their own addresses) is not supported yet")
    build_root = Path(os.environ[BUILD_ROOT_ENV]) if os.environ.get(BUILD_ROOT_ENV) else ROOT / "build"
    out = build_root / h["module"] / M.build_key(path)
    body = N.our_body(out, h)
    if body is None:
        raise NotChecked(f"no build products in {out} (run python tools/lbpmatch.py {path} first)")
    cache: dict = {}
    try:
        data, seg = M.module_bytes(cache, h["module"])
        mod = next(m for m in cache["_ledger"]["modules"] if m["name"] == h["module"])
    except SystemExit as e:                     # lbpconfig.ConfigError: no campaign configured
        raise NotChecked(f"the retail module is not available: {e}") from None
    except (OSError, KeyError, StopIteration, ValueError) as e:
        raise NotChecked(f"the retail module could not be read: {e}") from None
    segments = []
    for s in mod["load_segments"]:
        raw = data[s["file_offset"]:s["file_end"]]
        segments.append((s["start"], raw + bytes(max(0, s["memory_end"] - s["start"] - len(raw)))))
    off = seg["file_offset"] + h["start"] - seg["start"]
    original = data[off:off + h["end"] - h["start"]]
    overlays = []
    for sec in M.PLACED_WITH_CONTENTS:
        key = sec.strip(".")
        blob = out / f"{key}.bin"
        if key in h and blob.exists():
            overlays.append((int(h[key], 16), blob.read_bytes()))
    try:
        names = {a: n for n, a in M.load_symbols(h["module"]).items()}
    except (OSError, ValueError):
        names = {}
    return Function(str(path), h["start"], original, body, segments, overlays, names)


def make_runner(name: str):
    if name == "builtin":
        return run_builtin
    exe = os.environ.get(RUN_ELF_ENV, "").strip()
    if not exe or not Path(exe).is_file():
        def refuse(*_args):
            raise NotChecked(f"Nakagawa's interpreter is not configured: set {RUN_ELF_ENV} to a run_elf built with "
                             f"`make run_elf` in your Nakagawa Recomp checkout")
        return refuse
    return Nakagawa(Path(exe)).run


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("sources", nargs="*", type=Path, help="near-miss sources (wip/ or try/) already run through lbpmatch.py")
    ap.add_argument("--raw", nargs=2, type=Path, metavar=("ORIGINAL", "CANDIDATE"),
                    help="compare two raw little-endian code files instead (no module memory)")
    ap.add_argument("--address", type=lambda v: int(v, 0), help="with --raw: the address both run at")
    ap.add_argument("--states", type=int, default=200, help="random states (default 200; 0 with --captured only)")
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--captured", type=Path, action="append", default=[],
                    help=f"captured starting states ({STATE_FORMAT} JSON file or directory); private, never committed")
    ap.add_argument("--returns", choices=sorted(RETURN_GPRS), default="int")
    ap.add_argument("--strict", action="store_true", help="compare every register and every written byte")
    ap.add_argument("--interp", choices=("builtin", "nakagawa"), default="builtin")
    ap.add_argument("--max-steps", type=int, default=MAX_STEPS)
    ap.add_argument("--json", type=Path, help="write the reports as JSON")
    args = ap.parse_args(argv)
    if bool(args.raw) == bool(args.sources) or (args.raw and args.address is None):
        ap.error("give sources, or --raw ORIGINAL CANDIDATE with --address")
    states = [random_state(i, args.seed) for i in range(max(0, args.states))]
    try:
        for p in args.captured:
            states += captured_states(p)
    except (OSError, ValueError, NotChecked) as e:
        print(f"{TOOL}: captured states: {e}", file=sys.stderr)
        return 2
    runner = make_runner(args.interp)
    reports = []
    if args.raw:
        fns = [Function(f"{args.raw[1]}", args.address, args.raw[0].read_bytes(), args.raw[1].read_bytes())]
    else:
        fns = []
        for p in args.sources:
            try:
                fns.append(source_function(p))
            except NotChecked as e:
                reports.append(Report(str(p), "NOT_CHECKED", str(e), interpreter=args.interp))
    for fn in fns:
        reports.append(check(fn, states, runner, args.strict, args.returns, args.max_steps, args.interp))
    for r in reports:
        print(r.line())
    if args.json:
        args.json.write_text(json.dumps([r.as_json() for r in reports], indent=1) + "\n", encoding="utf-8")
    verdicts = {r.verdict for r in reports}
    return 1 if "DIFFERENT" in verdicts else 0 if verdicts == {"SAME_RESULTS"} else 2


if __name__ == "__main__":
    sys.exit(main())
