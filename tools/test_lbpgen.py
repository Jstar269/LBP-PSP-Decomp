"""Tests for the Ghidra translator (lbpgen.py): division-trap guards, retail call arguments."""
import sys
import unittest
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpgen as G  # noqa: E402

PSEUDO = """int FUN_0894a7bc(int param_1,uint param_2)

{
  uint uVar1;
  int iVar2;

  uVar1 = *(uint *)(param_1 + 8);
  if (uVar1 == 0) {
    trap(7);
  }
  iVar2 = uVar1 - param_2 % uVar1;
  if (param_2 % uVar1 == 0) {
    iVar2 = 0;
  }
  return iVar2;
}
"""


class DivisionTrapTest(unittest.TestCase):
    def test_guard_removed(self):
        out = G.drop_division_traps(PSEUDO)
        self.assertNotIn("trap", out)
        self.assertIn("iVar2 = uVar1 - param_2 % uVar1;", out)
        self.assertIn("if (param_2 % uVar1 == 0) {", out)       # an ordinary test is kept
        self.assertIsNone(G.FORBIDDEN.search(out))

    def test_guard_on_expression(self):
        text = "  if (*(int *)(p + 0x10) == 0) {\n    trap(7);\n  }\n  x = y / *(int *)(p + 0x10);\n"
        self.assertEqual(G.drop_division_traps(text), "  x = y / *(int *)(p + 0x10);\n")

    def test_bare_trap7_removed(self):
        text = "    else {\n      trap(7);\n    }\n"
        self.assertEqual(G.drop_division_traps(text), "    else {\n    }\n")

    def test_other_traps_stay_forbidden(self):
        for code in ("0", "0x1c00"):
            text = f"  if (x == 0) {{\n    trap({code});\n  }}\n"
            out = G.drop_division_traps(text)
            self.assertEqual(out, text)
            self.assertIsNotNone(G.FORBIDDEN.search(out))

    def test_guard_with_other_statement_kept(self):
        text = "  if (x == 0) {\n    y = 1;\n    trap(7);\n  }\n"
        self.assertNotRegex(G.drop_division_traps(text), r"if \(x == 0\) \{\n\s*\}")
        self.assertIn("y = 1;", G.drop_division_traps(text))


A0, A1, V0, T0, S0, RA = 4, 5, 2, 8, 16, 31
START, CALLEE, OTHER = 0x1000, 0x2000, 0x2100
NOP = 0


def i_type(op: int, rs: int, rt: int, imm: int) -> int:
    return (op << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)


def r_type(rs: int, rt: int, rd: int, funct: int, sa: int = 0, op: int = 0) -> int:
    return (op << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (sa << 6) | funct


def move(rd: int, rs: int) -> int:
    return r_type(rs, 0, rd, 0x25)


def addiu(rt: int, rs: int, imm: int) -> int:
    return i_type(0x09, rs, rt, imm)


def jal(target: int) -> int:
    return (0x03 << 26) | (target >> 2)


def j(target: int) -> int:
    return (0x02 << 26) | (target >> 2)


def seb(rd: int, rt: int) -> int:
    return r_type(0, rt, rd, 0x20, 0x10, 0x1F)


JR_RA = r_type(RA, 0, 0, 0x08)


class RetailCallArgsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.saved = dict(G.RETAIL), G.M.module_bytes

    def tearDown(self) -> None:
        G.RETAIL.clear()
        G.RETAIL.update(self.saved[0])
        G.M.module_bytes = self.saved[1]

    def args(self, code: list[int], arity: int = 1) -> list[list[str | None]]:
        """retail_call_args of `code` at START for its calls to CALLEE, a function that reads a0..a(arity-1)."""
        callee = [r_type(4 + k, 4 + k, V0, 0x21) for k in range(arity)] + [JR_RA, NOP]
        data = bytearray(0x3000)
        for base, ws in ((START, code), (CALLEE, callee), (OTHER, [JR_RA, NOP])):
            for k, w in enumerate(ws):
                data[base + 4 * k:base + 4 * k + 4] = w.to_bytes(4, "little")
        seg = {"start": 0, "file_offset": 0, "file_end": len(data)}
        G.M.module_bytes = lambda cache, module: (bytes(data), seg)
        G.RETAIL.clear()
        G.RETAIL.update({"module": "SYNTH", "ends": {START: START + 4 * len(code), CALLEE: CALLEE + 4 * len(callee),
                                                     OTHER: OTHER + 8}})
        return G.retail_call_args(START)[CALLEE]

    def test_parameters_passed_through(self) -> None:
        self.assertEqual(self.args([jal(CALLEE), NOP, JR_RA, NOP], arity=2), [["param_1", "param_2"]])

    def test_prologue_copy_survives_a_call(self) -> None:
        code = [move(S0, A0), jal(OTHER), NOP, move(A0, S0), jal(CALLEE), NOP, JR_RA, NOP]
        self.assertEqual(self.args(code)[0], ["param_1"])

    def test_sign_extension_writes_rd(self) -> None:
        # jal f; seb a0, a1 (delay slot): f((signed char)param_2, param_2)
        self.assertEqual(self.args([jal(CALLEE), seb(A0, A1), JR_RA, NOP], arity=2), [[None, "param_2"]])

    def test_prologue_copy_rewritten_later(self) -> None:
        # move s0, a0; lw s0, 0(s0); move a0, s0; jal f: a0 is *param_1, not param_1
        code = [move(S0, A0), i_type(0x23, S0, S0, 0), move(A0, S0), jal(CALLEE), NOP, JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_prologue_copy_of_a_rewritten_parameter(self) -> None:
        # lw a0, 4(a0); move s0, a0; ... move a0, s0; jal f: s0 never held param_1
        code = [i_type(0x23, A0, A0, 4), move(S0, A0), jal(OTHER), NOP, move(A0, S0), jal(CALLEE), NOP, JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_prologue_copy_advanced_in_a_loop(self) -> None:
        # move s0, a0; L: move a0, s0; jal f; addiu s0, s0, 4; bne s0, a1?, L: only the first pass gets param_1
        code = [move(S0, A0), move(A0, S0), jal(CALLEE), NOP, addiu(S0, S0, 4), i_type(0x05, S0, V0, -5), NOP,
                JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_parameter_register_rewritten_around_a_loop(self) -> None:
        # L: jal f; nop; bnez v0, L; move a0, v0 (delay slot): from the second pass a0 is f's result
        code = [jal(CALLEE), NOP, i_type(0x05, V0, 0, -3), move(A0, V0), JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_join_reached_by_j(self) -> None:
        # beqz a1, ELSE; nop; li a0, 1; j JOIN; nop; ELSE: li a0, 2; JOIN: jal f: a0 is 1 or 2
        code = [i_type(0x04, A1, 0, 4), NOP, addiu(A0, 0, 1), j(START + 6 * 4), NOP, addiu(A0, 0, 2),
                jal(CALLEE), NOP, JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_join_reached_by_b(self) -> None:
        code = [i_type(0x04, A1, 0, 4), NOP, addiu(A0, 0, 1), i_type(0x04, 0, 0, 2), NOP, addiu(A0, 0, 2),
                jal(CALLEE), NOP, JR_RA, NOP]
        self.assertEqual(self.args(code), [[None]])

    def test_constant_in_one_block(self) -> None:
        self.assertEqual(self.args([addiu(A0, 0, 7), jal(CALLEE), NOP, JR_RA, NOP]), [["7"]])

    def test_mfc1_overwrites_a_constant(self) -> None:
        mfc1 = (0x11 << 26) | (A0 << 16) | (0 << 11)
        self.assertEqual(self.args([addiu(A0, 0, 5), mfc1, jal(CALLEE), NOP, JR_RA, NOP]), [[None]])


if __name__ == "__main__":
    unittest.main()
