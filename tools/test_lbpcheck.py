#!/usr/bin/env python3
"""Synthetic tests for the behavioural check (tools/lbpcheck.py) and its interpreter (tools/lbpinterp.py).

No game data: every function here is a few hand-assembled instructions. The acceptance pair of LBP-PSP-Decomp issue #6:
two functions that differ only in register allocation give SAME_RESULTS, and a third with one wrong immediate names
the first differing register. When Nakagawa Recomp's run_elf is available (LBPCHECK_RUN_ELF, or a checkout to build
it from in LBPCHECK_NAKAGAWA_SRC or lbp-config's nakagawa_root, with g++ on PATH), the same functions also run in
it and both interpreters must agree; otherwise those tests are skipped.
"""
from __future__ import annotations

import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpcheck as C  # noqa: E402
import lbpinterp as I  # noqa: E402

R = I.REG_INDEX
BASE = 0x08900000


# A tiny assembler for the encodings the tests use.
def _r(name: str) -> int:
    return R[name]


def special(funct: int, rd: str = "zero", rs: str = "zero", rt: str = "zero", sa: int = 0) -> int:
    return (_r(rs) << 21) | (_r(rt) << 16) | (_r(rd) << 11) | (sa << 6) | funct


def itype(op: int, rt: str, rs: str, imm: int) -> int:
    return (op << 26) | (_r(rs) << 21) | (_r(rt) << 16) | (imm & 0xFFFF)


def addu(rd, rs, rt): return special(0x21, rd, rs, rt)
def subu(rd, rs, rt): return special(0x23, rd, rs, rt)
def sll(rd, rt, sa): return special(0x00, rd, "zero", rt, sa)
def mult(rs, rt): return special(0x18, "zero", rs, rt)
def div(rs, rt): return special(0x1A, "zero", rs, rt)
def mflo(rd): return special(0x12, rd)
def jr(rs): return special(0x08, "zero", rs)
def addiu(rt, rs, imm): return itype(0x09, rt, rs, imm)
def ori(rt, rs, imm): return itype(0x0D, rt, rs, imm)
def lw(rt, base, imm): return itype(0x23, rt, base, imm)
def sw(rt, base, imm): return itype(0x2B, rt, base, imm)
def lwl(rt, base, imm): return itype(0x22, rt, base, imm)
def lwr(rt, base, imm): return itype(0x26, rt, base, imm)
def bne(rs, rt, off): return itype(0x05, rt, rs, off)
def beql(rs, rt, off): return itype(0x14, rt, rs, off)
def jal(target): return (3 << 26) | ((target >> 2) & 0x03FFFFFF)
def ext(rt, rs, pos, size): return (0x1F << 26) | (_r(rs) << 21) | (_r(rt) << 16) | ((size - 1) << 11) | (pos << 6)
def ins(rt, rs, pos, size): return (0x1F << 26) | (_r(rs) << 21) | (_r(rt) << 16) | ((pos + size - 1) << 11) | (pos << 6) | 4
def mtc1(rt, fs): return (0x11 << 26) | (4 << 21) | (_r(rt) << 16) | (fs << 11)
def mfc1(rt, fs): return (0x11 << 26) | (_r(rt) << 16) | (fs << 11)
def add_s(fd, fs, ft): return (0x11 << 26) | (0x10 << 21) | (ft << 16) | (fs << 11) | (fd << 6)
def c_lt_s(fs, ft): return (0x11 << 26) | (0x10 << 21) | (ft << 16) | (fs << 11) | 0x3C
def bc1t(off): return (0x11 << 26) | (8 << 21) | (1 << 16) | (off & 0xFFFF)
NOP = 0
VFPU_VADD = 0x60000000                                     # opcode 0x18: a VFPU arithmetic instruction


def code(*words: int) -> bytes:
    return b"".join(w.to_bytes(4, "little") for w in words)


def body(regs: tuple[str, str, str], imm: int = 7) -> bytes:
    """(a0 + a1) * 3 + *a2 + imm, stored to a2[1] and returned; one saved register and two temporaries."""
    s, t, u = regs
    slot = 0 if s == "s0" else 4
    return code(addiu("sp", "sp", -16), sw(s, "sp", slot),
                addu(s, "a0", "a1"), sll(t, s, 1), addu("v0", t, s),
                lw(u, "a2", 0), addu("v0", "v0", u), addiu("v0", "v0", imm), sw("v0", "a2", 4),
                lw(s, "sp", slot), jr("ra"), addiu("sp", "sp", 16))


ORIGINAL = body(("s0", "t0", "t1"))
REALLOCATED = body(("s1", "t2", "t3"))                     # differs only in register allocation (and the save slot)
WRONG_IMMEDIATE = body(("s0", "t0", "t1"), imm=8)          # one wrong immediate


_STATES: dict[tuple[int, int], list[C.StartState]] = {}


def states(n: int = 40, seed: int = 1) -> list[C.StartState]:
    """Fresh copies of the first n random states of a seed (made once; the tests may change their copies)."""
    if (n, seed) not in _STATES:
        _STATES[(n, seed)] = [C.random_state(i, seed) for i in range(n)]
    return [C.StartState(s.label, s.cpu.copy(), list(s.blocks), s.sp) for s in _STATES[(n, seed)]]


def check(original: bytes, candidate: bytes, **kw) -> C.Report:
    return C.check(C.Function("synthetic", BASE, original, candidate), kw.pop("states", None) or states(),
                   kw.pop("runner", C.run_builtin), **kw)


def run_one(words: bytes, setup: dict[str, int], memory: list[tuple[int, bytes]] = (), fpr: dict[int, int] = ()):
    mem = I.Memory()
    for a, d in memory:
        mem.map(a, d)
    mem.map(BASE, words)
    st = I.CpuState()
    for k, v in setup.items():
        st.gpr[R[k]] = v & I.MASK
    for k, v in dict(fpr).items():
        st.fpr[k] = v
    st.gpr[31] = I.RETURN_SENTINEL
    return I.run(mem, st, BASE, BASE + len(words))


class AcceptanceTest(unittest.TestCase):
    def test_register_allocation_only_gives_same_results(self):
        rep = check(ORIGINAL, REALLOCATED)
        self.assertEqual(rep.verdict, "SAME_RESULTS", rep.line())
        self.assertIn("same results", rep.text)
        self.assertIn(f"behaves like the original on {rep.same} states", rep.text)
        self.assertIn("Not a match", rep.text)
        self.assertGreater(rep.same, 10)

    def test_one_wrong_immediate_names_the_first_differing_register(self):
        rep = check(ORIGINAL, WRONG_IMMEDIATE)
        self.assertEqual(rep.verdict, "DIFFERENT", rep.line())
        self.assertRegex(rep.first_difference["difference"], r"^v0: original 0x[0-9a-f]{8}, candidate 0x[0-9a-f]{8}$")
        self.assertIn("first difference v0", rep.text)

    def test_strict_mode_sees_the_temporaries(self):
        rep = check(ORIGINAL, REALLOCATED, strict=True)
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertRegex(rep.first_difference["difference"], r"^(t0|s1|t1|t2|t3): ")

    def test_states_are_reproducible(self):
        a, b = C.random_state(5, 3), C.random_state(5, 3)
        self.assertEqual(a.cpu, b.cpu)
        self.assertEqual(a.blocks, b.blocks)
        self.assertNotEqual(a.cpu, C.random_state(6, 3).cpu)
        self.assertEqual(a.cpu.gpr[29], C.ENTRY_SP)
        self.assertEqual(a.cpu.gpr[31], I.RETURN_SENTINEL)

    def test_edge_values_appear(self):
        seen = {w for i in range(40) for w in C.random_state(i, 1).cpu.gpr}
        for v in (0, 1, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF):
            self.assertIn(v, seen)
        floats = {w for i in range(40) for w in C.random_state(i, 1).cpu.fpr}
        for v in (0x7F800000, 0xFF800000, 0x7FC00000, 0x80000000):
            self.assertIn(v, floats)


class ExitTest(unittest.TestCase):
    def call(self, arg_imm: int) -> bytes:
        return code(addiu("sp", "sp", -8), sw("ra", "sp", 0), jal(0x08A00000), addiu("a0", "a1", arg_imm),
                    lw("ra", "sp", 0), jr("ra"), addiu("sp", "sp", 8))

    def test_call_compares_arguments_not_the_return_address(self):
        a = self.call(4)
        b = code(addiu("sp", "sp", -16), sw("ra", "sp", 8), jal(0x08A00000), addiu("a0", "a1", 4),
                 lw("ra", "sp", 8), jr("ra"), addiu("sp", "sp", 16))
        rep = check(a, b)
        self.assertEqual(rep.verdict, "SAME_RESULTS", rep.line())

    def test_call_with_a_wrong_argument(self):
        rep = check(self.call(4), self.call(5))
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertTrue(rep.first_difference["difference"].startswith("a0: "), rep.line())
        self.assertIn("called 0x08a00000", rep.first_difference["original_exit"])

    def test_different_exits(self):
        returns = code(jr("ra"), addiu("v0", "zero", 1))
        rep = check(returns, self.call(4))
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertIn("exit: original returned, candidate called 0x08a00000", rep.text)

    def test_unmodelled_instruction_is_not_checked_by_name(self):
        rep = check(code(VFPU_VADD, jr("ra"), NOP), code(jr("ra"), NOP))
        self.assertEqual(rep.verdict, "NOT_CHECKED")
        self.assertIn("VFPU instruction (opcode 0x18)", rep.text)

    def test_division_by_zero_is_not_checked(self):
        f = code(div("a0", "a1"), mflo("v0"), jr("ra"), NOP)
        rep = check(f, f, states=[self.state(a0=7, a1=0)])
        self.assertEqual(rep.verdict, "NOT_CHECKED")
        self.assertIn("division by zero", rep.text)

    def test_candidate_fault_where_the_original_finished_is_a_difference(self):
        rep = check(code(jr("ra"), NOP), code(lw("v0", "zero", 0), jr("ra"), NOP), states=[self.state()])
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertIn("outside the modelled memory", rep.text)

    @staticmethod
    def state(**regs: int) -> C.StartState:
        st = C.random_state(0, 1)
        for k, v in regs.items():
            st.cpu.gpr[R[k]] = v
        return st


class InterpreterTest(unittest.TestCase):
    def test_unaligned_word_load(self):
        mem = [(0x0A000000, bytes(range(16)))]
        out = run_one(code(lwl("v0", "a0", 3), lwr("v0", "a0", 0), jr("ra"), NOP), {"a0": 0x0A000005}, mem)
        self.assertEqual(out.kind, "return")
        self.assertEqual(out.state.gpr[2], 0x08070605)

    def test_ext_ins(self):
        out = run_one(code(ext("v0", "a0", 4, 8), ins("v1", "a0", 8, 4), jr("ra"), NOP),
                      {"a0": 0x12345678, "v1": 0xFFFFFFFF})
        self.assertEqual(out.state.gpr[2], 0x67)
        self.assertEqual(out.state.gpr[3], 0xFFFFF8FF)

    def test_multiply_and_loop(self):
        # v0 = a0 * a1 by repeated addition (a branch with its delay slot), then checked with mult
        f = code(ori("v0", "zero", 0), addu("v0", "v0", "a0"), addiu("a1", "a1", -1), bne("a1", "zero", -3), NOP,
                 mult("a0", "a2"), mflo("v1"), jr("ra"), NOP)
        out = run_one(f, {"a0": 7, "a1": 6, "a2": 6})
        self.assertEqual((out.state.gpr[2], out.state.gpr[3]), (42, 42))

    def test_likely_branch_annuls_its_delay_slot(self):
        f = code(beql("a0", "zero", 2), addiu("v0", "zero", 1), addiu("v1", "zero", 2), jr("ra"), NOP)
        out = run_one(f, {"a0": 5, "v0": 9, "v1": 9})
        self.assertEqual((out.state.gpr[2], out.state.gpr[3]), (9, 2))

    def test_single_float_add_and_compare(self):
        f = code(mtc1("a0", 0), mtc1("a1", 1), add_s(2, 0, 1), c_lt_s(0, 2), bc1t(2), addiu("v1", "zero", 1),
                 addiu("v1", "zero", 2), mfc1("v0", 2), jr("ra"), NOP)
        out = run_one(f, {"a0": 0x3F800000, "a1": 0x3F000000})
        self.assertEqual(out.state.gpr[2], 0x3FC00000)              # 1.0 + 0.5
        self.assertEqual(out.state.gpr[3], 1)                       # taken: the delay slot ran, the next did not

    def test_nan_and_infinity(self):
        self.assertEqual(I.bits_of(I.f32(0x7F800000) - I.f32(0x7F800000)), I.CANONICAL_NAN)
        self.assertEqual(I.bits_of(I.f32(0x7F7FFFFF) * 2.0), 0x7F800000)
        self.assertEqual(I.to_word(I.f32(0x7FC00000), "trunc"), I.INVALID_WORD)
        self.assertEqual(I.to_word(2.5, "nearest"), 2)

    def test_branch_in_a_delay_slot_is_refused(self):
        out = run_one(code(jr("ra"), jr("ra"), NOP), {})
        self.assertEqual(out.kind, "unsupported")
        self.assertIn("delay slot", out.detail)

    def test_written_bytes_keep_their_old_value(self):
        mem = [(0x0A000000, bytes([0xAA] * 8))]
        out = run_one(code(sw("a1", "a0", 0), jr("ra"), NOP), {"a0": 0x0A000000, "a1": 0x01020304}, mem)
        self.assertEqual(out.before, {0x0A000000 + i: 0xAA for i in range(4)})
        self.assertEqual(out.after, {0x0A000000: 4, 0x0A000001: 3, 0x0A000002: 2, 0x0A000003: 1})

    def test_self_test(self):
        self.assertEqual(I.self_test(quiet=True), 0)


class CapturedStateTest(unittest.TestCase):
    def test_captured_state_runs(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / "one.json"
            p.write_text(json.dumps({"format": C.STATE_FORMAT, "gpr": {"a0": "0x3", "a1": "4", "a2": "0x0a000010",
                                                                       "sp": "0x0a10c000", "ra": "0x1234"},
                                     "fpr": {"f12": "0x3f800000"},
                                     "memory": [{"address": "0x0a000000", "hex": "00" * 64},
                                                {"address": "0x0a100000", "hex": "00" * 0xC000}]}))
            sts = C.captured_states(Path(tmp))
        self.assertEqual(len(sts), 1)
        self.assertEqual(sts[0].cpu.gpr[31], I.RETURN_SENTINEL)
        self.assertEqual(sts[0].cpu.fpr[12], 0x3F800000)
        rep = check(ORIGINAL, WRONG_IMMEDIATE, states=sts)
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertEqual(rep.first_difference["difference"], "v0: original 0x0000001c, candidate 0x0000001d")

    def test_other_formats_are_refused(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / "x.json"
            p.write_text(json.dumps({"format": "something-else"}))
            with self.assertRaises(C.NotChecked):
                C.captured_states(p)


class CommandLineTest(unittest.TestCase):
    def run_cli(self, original: bytes, candidate: bytes, *extra: str) -> tuple[int, str, list | None]:
        with tempfile.TemporaryDirectory() as tmp:
            a, b = Path(tmp) / "orig.bin", Path(tmp) / "cand.bin"
            a.write_bytes(original)
            b.write_bytes(candidate)
            out = Path(tmp) / "r.json"
            p = subprocess.run([sys.executable, str(Path(C.__file__)), "--raw", str(a), str(b), "--address",
                                hex(BASE), "--states", "24", "--json", str(out), *extra],
                               capture_output=True, text=True)
            data = json.loads(out.read_text()) if out.exists() else None
        return p.returncode, p.stdout + p.stderr, data

    def test_raw_same(self):
        code_, text, data = self.run_cli(ORIGINAL, REALLOCATED)
        self.assertEqual(code_, 0, text)
        self.assertIn("SAME_RESULTS", text)
        self.assertEqual(data[0]["verdict"], "SAME_RESULTS")

    def test_raw_different(self):
        code_, text, data = self.run_cli(ORIGINAL, WRONG_IMMEDIATE)
        self.assertEqual(code_, 1, text)
        self.assertIn("first difference v0", text)

    def test_nakagawa_not_configured(self):
        env_backup = os.environ.pop(C.RUN_ELF_ENV, None)
        try:
            code_, text, _ = self.run_cli(ORIGINAL, REALLOCATED, "--interp", "nakagawa")
        finally:
            if env_backup is not None:
                os.environ[C.RUN_ELF_ENV] = env_backup
        self.assertEqual(code_, 2, text)
        self.assertIn("NOT_CHECKED", text)
        self.assertIn("not configured", text)

    def test_source_without_build_products_is_not_checked(self):
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "FUN_08900000.c"
            src.write_text("/* LBPMATCH module=EBOOT.BIN start=0x08900000 end=0x08900030 symbol=FUN_08900000 "
                           "lang=c99 */\nint FUN_08900000(void) { return 0; }\n")
            env = dict(os.environ, LBPMATCH_BUILD_ROOT=str(Path(tmp) / "build"))
            p = subprocess.run([sys.executable, str(Path(C.__file__)), str(src)], capture_output=True, text=True,
                               env=env)
        self.assertEqual(p.returncode, 2, p.stdout + p.stderr)
        self.assertIn("NOT_CHECKED", p.stdout)
        self.assertIn("no build products", p.stdout)

    def test_never_writes_matching_state(self):
        text = Path(C.__file__).read_text(encoding="utf-8")
        for name in ("accepted-ledger", "accepted-journal", "progress.json", "lbppromote"):
            self.assertNotIn(name, text)


def relocatable(text_size: int, symbol: str) -> bytes:
    """A minimal ELF32 object: .text of text_size bytes and one function symbol at its start (what lbpmatch.py
    leaves in obj.o, as far as lbpnearmiss.our_body reads it)."""
    shstr = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    strtab = b"\0" + symbol.encode() + b"\0"
    symtab = bytes(16) + struct.pack("<IIIBBH", 1, 0, text_size, 0x12, 0, 1)
    body_ = bytes(text_size)
    off_text = 52
    off_sym = off_text + len(body_)
    off_str = off_sym + len(symtab)
    off_shstr = off_str + len(strtab)
    shoff = (off_shstr + len(shstr) + 3) & ~3
    head = struct.pack("<4sBBBB8xHHIIIIIHHHHHH", b"\x7fELF", 1, 1, 1, 0, 1, 8, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 5, 4)
    data = head + body_ + symtab + strtab + shstr
    data += bytes(shoff - len(data))
    sections = [bytes(40),
                struct.pack("<10I", 1, 1, 6, 0, off_text, text_size, 0, 0, 4, 0),
                struct.pack("<10I", 7, 2, 0, 0, off_sym, len(symtab), 3, 1, 4, 16),
                struct.pack("<10I", 15, 3, 0, 0, off_str, len(strtab), 0, 0, 1, 0),
                struct.pack("<10I", 23, 3, 0, 0, off_shstr, len(shstr), 0, 0, 1, 0)]
    return data + b"".join(sections)


class SourceModeTest(unittest.TestCase):
    """A source checked end to end against a synthetic campaign and synthetic build products (no game data)."""

    def side(self, candidate: bytes) -> C.Report:
        import lbpmatch as M
        with tempfile.TemporaryDirectory() as tmp:
            t = Path(tmp)
            module = ORIGINAL + bytes(64)
            (t / "mod.bin").write_bytes(module)
            (t / "campaign" / "ledger").mkdir(parents=True)
            (t / "campaign" / "inputs").mkdir()
            seg = {"start": BASE, "file_offset": 0, "file_end": len(module), "memory_end": BASE + 0x200, "flags": 5}
            (t / "campaign" / "ledger" / "campaign.json").write_text(json.dumps(
                {"modules": [{"name": "EBOOT.BIN", "input": {"root": "r", "path": "mod.bin"}, "load_segments": [seg],
                              "functions": [{"start": BASE, "end": BASE + len(ORIGINAL)}]}]}))
            (t / "campaign" / "inputs" / "revision-selection.json").write_text(json.dumps({"roots": {"r": str(t)}}))
            src = t / "wip" / "near_08900000.c"
            src.parent.mkdir()
            src.write_text(f"/* LBPMATCH module=EBOOT.BIN start=0x{BASE:08x} end=0x{BASE + len(ORIGINAL):08x} "
                           f"symbol=near_08900000 lang=c99 */\n")
            out = t / "build" / "EBOOT.BIN" / M.build_key(src)
            out.mkdir(parents=True)
            (out / "text.bin").write_bytes(candidate)
            (out / "obj.o").write_bytes(relocatable(len(candidate), "near_08900000"))
            saved = {k: os.environ.get(k) for k in ("LBP_CAMPAIGN_ROOT", C.BUILD_ROOT_ENV)}
            os.environ["LBP_CAMPAIGN_ROOT"] = str(t / "campaign")
            os.environ[C.BUILD_ROOT_ENV] = str(t / "build")
            try:
                fn = C.source_function(src)
            finally:
                for k, v in saved.items():
                    if v is None:
                        os.environ.pop(k, None)
                    else:
                        os.environ[k] = v
        self.assertEqual(fn.original, ORIGINAL)
        self.assertEqual(fn.segments, [(BASE, module + bytes(0x200 - len(module)))])
        return C.check(fn, states(), C.run_builtin)

    def test_source_same_results(self):
        self.assertEqual(self.side(REALLOCATED).verdict, "SAME_RESULTS")

    def test_source_different(self):
        rep = self.side(WRONG_IMMEDIATE)
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertIn("first difference v0", rep.text)


def nakagawa_run_elf() -> Path | None:
    """A run_elf to cross-check with: LBPCHECK_RUN_ELF, or one built (once) from a Nakagawa checkout."""
    exe = os.environ.get(C.RUN_ELF_ENV, "").strip()
    if exe and Path(exe).is_file():
        return Path(exe)
    src = os.environ.get("LBPCHECK_NAKAGAWA_SRC", "").strip()
    if not src:
        try:
            import lbpconfig
            src = lbpconfig.get("nakagawa_root") or ""
        except SystemExit:
            src = ""
    cxx = shutil.which("g++")
    if not src or not cxx or not (Path(src) / "src" / "ref" / "run_elf.cpp").is_file():
        return None
    out = Path(tempfile.gettempdir()) / f"lbpcheck-run_elf-{os.getpid()}"
    if not out.exists():
        r = subprocess.run([cxx, "-std=c++17", "-O1", "-Isrc/ref", "-Isrc/rt", "-o", str(out), "src/ref/run_elf.cpp",
                            "src/ref/interp.cpp", "-fno-exceptions"], cwd=src, capture_output=True, text=True)
        if r.returncode != 0:
            return None
    return out


RUN_ELF = nakagawa_run_elf()


@unittest.skipUnless(RUN_ELF, "Nakagawa's run_elf is not available (set LBPCHECK_RUN_ELF or LBPCHECK_NAKAGAWA_SRC)")
class NakagawaTest(unittest.TestCase):
    """Both interpreters run the same synthetic functions from the same states and must agree."""

    def setUp(self):
        self.nk = C.Nakagawa(RUN_ELF).run

    def test_acceptance_in_nakagawa(self):
        self.assertEqual(check(ORIGINAL, REALLOCATED, runner=self.nk, states=states(16)).verdict, "SAME_RESULTS")
        rep = check(ORIGINAL, WRONG_IMMEDIATE, runner=self.nk, states=states(16))
        self.assertEqual(rep.verdict, "DIFFERENT")
        self.assertIn("first difference v0", rep.text)

    def test_outcomes_agree_with_the_builtin_interpreter(self):
        loop = code(ori("v0", "zero", 0), addu("v0", "v0", "a0"), addiu("a1", "a1", -1), bne("a1", "zero", -3), NOP,
                    mult("a0", "a2"), mflo("v1"), jr("ra"), NOP)
        fpu = code(mtc1("a0", 0), mtc1("a1", 1), add_s(2, 0, 1), c_lt_s(0, 2), bc1t(2), addiu("v1", "zero", 1),
                   addiu("v1", "zero", 2), mfc1("v0", 2), jr("ra"), NOP)
        unaligned = code(lwl("v0", "a0", 3), lwr("v0", "a0", 0), ext("v1", "v0", 4, 8), ins("v1", "a0", 8, 4),
                         sw("v1", "a0", 8), jr("ra"), NOP)
        call = ExitTest().call(4)
        sts = states(12)
        for st in sts:
            st.cpu.gpr[R["a1"]] = (st.cpu.gpr[R["a1"]] & 0x1F) or 3   # bounded loop counts
        for name, f in (("acceptance", ORIGINAL), ("loop", loop), ("fpu", fpu), ("unaligned", unaligned),
                        ("call", call)):
            fn = C.Function(name, BASE, f, f)
            for st in sts:
                a = C.run_builtin(fn, st, False, 10000)
                b = self.nk(fn, st, False, 10000)
                with self.subTest(function=name, state=st.label):
                    self.assertEqual(a.kind, b.kind, (a.detail, b.detail))
                    if not a.completed:
                        continue
                    self.assertEqual(a.target, b.target)
                    self.assertEqual(a.state.gpr, b.state.gpr)
                    self.assertEqual((a.state.hi, a.state.lo), (b.state.hi, b.state.lo))
                    self.assertEqual(a.state.fpr, b.state.fpr)
                    self.assertEqual(a.after, b.after)


if __name__ == "__main__":
    unittest.main()
