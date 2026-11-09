"""Tests for the Ghidra translator (lbpgen.py): division-trap guards."""
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


if __name__ == "__main__":
    unittest.main()
