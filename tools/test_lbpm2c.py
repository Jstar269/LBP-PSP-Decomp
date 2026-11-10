"""Tests of tools/lbpm2c.py's pure parts (no m2c, objdump or compiler needed)."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpm2c as X

DIS = """
08835920 <.data>:
 8835920:\tac80003c \tsw\tzero,60(a0)
 8835924:\ta0800024 \tsb\tzero,36(a0)
 8835928:\t0a20cf16 \tj\t0x8833c58
 883592c:\ta0800038 \tsb\tzero,56(a0)
"""


class Conversion(unittest.TestCase):
    def test_parse_objdump_adds_register_dollars(self):
        ins = X.parse_objdump(DIS)
        self.assertEqual(ins[0], (0x08835920, "sw", ["$zero", "60($a0)"]))
        self.assertEqual(ins[2][1:], ("j", ["0x8833c58"]))

    def test_tail_call_keeps_its_delay_slot(self):
        s = X.to_m2c(0x08835920, 0x08835930, X.parse_objdump(DIS))
        body = [l for l in s.splitlines() if l.startswith("/*") or l in ("nop", "jr $ra")]
        self.assertEqual(body[2], "/* 0883592C */ sb $zero, 56($a0)")
        self.assertEqual(body[3], "/* 08835928 */ jal FUN_08833c58")
        self.assertEqual(body[4:], ["nop", "jr $ra", "nop"][:len(body) - 4])

    def test_jr_register_becomes_call(self):
        ins = [(0x100, "lw", ["$t9", "72($v0)"]), (0x104, "jr", ["$t9"]), (0x108, "move", ["$a3", "$zero"])]
        s = X.to_m2c(0x100, 0x10c, ins)
        self.assertIn("/* 00000108 */ move $a3, $zero\n/* 00000104 */ jalr $t9\nnop\njr $ra", s)

    def test_lui_with_one_use_becomes_symbol(self):
        ins = [(0x100, "lui", ["$v0", "0x8bf"]), (0x104, "lw", ["$v0", "-8(" + "$v0)"]), (0x108, "jr", ["$ra"]),
               (0x10c, "nop", [])]
        s = X.to_m2c(0x100, 0x110, ins)
        self.assertIn("lui $v0, %hi(DAT_08befff8)", s)
        self.assertIn("lw $v0, %lo(DAT_08befff8)($v0)", s)

    def test_branch_target_label(self):
        ins = [(0x100, "beqz", ["$a0", "0x10c"]), (0x104, "nop", []), (0x108, "li", ["$v0", "1"]),
               (0x10c, "jr", ["$ra"]), (0x110, "nop", [])]
        s = X.to_m2c(0x100, 0x114, ins)
        self.assertIn("beqz $a0, .L0000010c", s)
        self.assertIn(".L0000010c:\n/* 0000010C */ jr $ra", s)


class PlainC(unittest.TestCase):
    def test_field_macro_nested(self):
        t = "M2C_FIELD(M2C_FIELD(arg0, void **, 4), s32 *, 0x3C) = 0;"
        self.assertEqual(X.plain_c(t), "(*(int *)((char *)((*(void **)((char *)(arg0) + 4))) + 0x3C)) = 0;")

    def test_types_and_bitwise(self):
        self.assertEqual(X.plain_c("u16 f(M2C_UNK a) { return M2C_BITWISE(u16, a); }"),
                         "unsigned short f(int a) { return ((unsigned short)(a)); }")

    def test_null_becomes_zero(self):
        self.assertEqual(X.plain_c("if (arg0 == NULL) { x = NULL; }"), "if (arg0 == 0) { x = 0; }")

    def test_no_m2c_names_left(self):
        out = X.plain_c("M2C_UNK32 x; s8 y; M2C_UNK64 z;")
        self.assertNotIn("M2C", out)


if __name__ == "__main__":
    unittest.main()
