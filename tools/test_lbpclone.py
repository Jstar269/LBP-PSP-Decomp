#!/usr/bin/env python3
"""Game-free tests for tools/lbpclone.py: clone retargeting (ported from hst-decomp's test_hstclone.py where
the behaviour is the same) and the masked signature and references of analyse (synthetic code only)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C  # noqa: E402


class MappingTests(unittest.TestCase):
    def test_consistent_references_map(self) -> None:
        e_refs = [("call", 0, 0x00400000), ("hi", 1, 0x00300000), ("data", 2, 0x00301000)]
        t_refs = [("call", 0, 0x00500000), ("hi", 1, 0x00310000), ("data", 2, 0x00311000)]
        self.assertEqual(C.mapping(e_refs, t_refs),
                         ({0x00400000: 0x00500000}, {0x00301000: 0x00311000}, {0x00300000: 0x00310000}))

    def test_inconsistent_call_or_data_mapping_refuses(self) -> None:
        self.assertIsNone(C.mapping([("call", 0, 0x00400000), ("call", 2, 0x00400000)],
                                    [("call", 0, 0x00500000), ("call", 2, 0x00600000)]))
        self.assertIsNone(C.mapping([("data", 0, 0x00301000)], [("call", 0, 0x00301000)]))

    def test_ambiguous_high_half_is_dropped_not_fatal(self) -> None:
        # two exemplar globals in one 64 KiB page whose copies straddle a page boundary in the target: the lui high
        # halves disagree, but the data references themselves map one to one, so the clone is still a candidate
        e_refs = [("hi", 0, 0x00300000), ("data", 1, 0x00301000), ("hi", 2, 0x00300000), ("data", 3, 0x00302000)]
        t_refs = [("hi", 0, 0x00310000), ("data", 1, 0x0031f000), ("hi", 2, 0x00320000), ("data", 3, 0x00320000)]
        res = C.mapping(e_refs, t_refs)
        self.assertIsNotNone(res)
        calls, data, his = res
        self.assertEqual(data, {0x00301000: 0x0031f000, 0x00302000: 0x00320000})
        self.assertEqual(his, {})


class PlacedKeyTests(unittest.TestCase):
    def test_bss_follows_the_data_mapping(self) -> None:
        # the form of the static initialisers: the header's bss= is the copy's lowest global, which the code names
        # (src/eboot/static_init_089d19dc.cpp, whose bss= had to be rebased by hand because lbpclone kept the
        # exemplar's)
        text = ("/* LBPMATCH module=EBOOT.BIN start=0x0882d5a0 end=0x0882d5c8 symbol=f lang=c++ cc=gcc404xs-Os "
                "bss=0x08bed9f0 rodata=0x08bab598 */\n"
                "float DAT_08bed9f0 = 1.0f;\n")
        out = C.retarget(text, 0x0882d5a0, 0x089d19dc, 0x089d1a04, {}, {0x08bed9f0: 0x08bf3104})
        head = out.split("\n", 1)[0]
        self.assertIn("bss=0x08bf3104", head)
        self.assertIn("rodata=0x08bab598", head)        # no reference reveals the table: the exemplar's stays
        self.assertIn("float DAT_08bf3104", out)

    def test_every_placed_data_key_follows_the_data_mapping(self) -> None:
        text = ("/* LBPMATCH module=EBOOT.BIN start=0x00001000 end=0x00001020 symbol=f rodata=0x002c0000 "
                "data=0x002d8af8 sdata=0x002e0000 sbss=0x002f0000 */\nvoid f(void) {}\n")
        out = C.retarget(text, 0x1000, 0x2000, 0x2020, {},
                         {0x002c0000: 0x00360000, 0x002d8af8: 0x0037f8c4, 0x002e0000: 0x00380000,
                          0x002f0000: 0x00390000})
        head = out.split("\n", 1)[0]
        for want in ("rodata=0x00360000", "data=0x0037f8c4", "sdata=0x00380000", "sbss=0x00390000"):
            self.assertIn(want, head)

    def test_rodata_and_data_with_the_same_address_both_move(self) -> None:
        text = ("/* LBPMATCH module=EBOOT.BIN start=0x00001000 end=0x00001020 symbol=f rodata=0x002d0000 "
                "data=0x002d0000 */\nvoid f(void) {}\n")
        out = C.retarget(text, 0x1000, 0x2000, 0x2020, {}, {0x002d0000: 0x00370000})
        self.assertIn("rodata=0x00370000 data=0x00370000", out)


class HelperRangeTests(unittest.TestCase):
    HEAD = ("/* LBPMATCH module=EBOOT.BIN start=0x00001000 end=0x00001020 symbol=FUN_00001000 "
            "helpers=FUN_00001020@0x00001020-0x0000104c */\n")

    def test_helper_range_follows_the_call_mapping(self) -> None:
        text = self.HEAD + "static void FUN_00001020(void) {}\nvoid FUN_00001000(void) { FUN_00001020(); }\n"
        out = C.retarget(text, 0x1000, 0x2000, 0x2020, {0x00001020: 0x00003000}, {})
        self.assertIn("helpers=FUN_00003000@0x00003000-0x0000302c", out)
        self.assertNotIn("0x00001020", out)

    def test_unmapped_helper_refuses_the_clone(self) -> None:
        text = self.HEAD + "void FUN_00001000(void) {}\n"
        self.assertIsNone(C.retarget(text, 0x1000, 0x2000, 0x2020, {}, {}))


class OperandDecodingTests(unittest.TestCase):
    """Only instructions whose rs is a GPR operand complete a lui; every GPR write ends the lui's value."""
    LUI_S0 = (0x0F << 26) | (16 << 16) | 0x0809          # lui s0, 0x0809
    LUI_T0 = (0x0F << 26) | (8 << 16) | 0x0809           # lui t0, 0x0809
    LW_A0_4_T0 = (0x23 << 26) | (8 << 21) | (4 << 16) | 4  # lw a0, 4(t0)

    @staticmethod
    def fpu(funct: int) -> int:                         # <op>.s $f0, $f0, $f12: rs field = fmt S = 16 (s0's number)
        return (0x11 << 26) | (16 << 21) | (12 << 16) | funct

    @staticmethod
    def ext(rt: int, rs: int, pos: int, size: int) -> int:
        return (0x1F << 26) | (rs << 21) | (rt << 16) | ((size - 1) << 11) | (pos << 6)

    def test_fpu_arithmetic_is_not_a_low_half(self) -> None:
        add_sig, add_refs = C.analyse([self.LUI_S0, self.fpu(0x00), 0x03E00008, 0])   # add.s
        mul_sig, _ = C.analyse([self.LUI_S0, self.fpu(0x02), 0x03E00008, 0])          # mul.s
        self.assertNotEqual(add_sig, mul_sig)
        self.assertEqual(add_refs, [("hi", 0, 0x08090000)])

    def test_ext_field_is_not_a_low_half(self) -> None:
        sig0, refs = C.analyse([self.LUI_T0, self.ext(9, 8, 0, 8)])
        sig8, _ = C.analyse([self.LUI_T0, self.ext(9, 8, 8, 8)])
        self.assertNotEqual(sig0, sig8)
        self.assertEqual(refs, [("hi", 0, 0x08090000)])

    def test_branch_likely_offset_is_not_a_low_half(self) -> None:
        beql = lambda off: (0x14 << 26) | (8 << 21) | off  # noqa: E731  beql t0, zero, off
        self.assertNotEqual(C.analyse([self.LUI_T0, beql(2)])[0], C.analyse([self.LUI_T0, beql(3)])[0])

    def test_fpu_load_is_still_a_low_half(self) -> None:
        lwc1 = (0x31 << 26) | (16 << 21) | 0x10           # lwc1 $f0, 0x10(s0)
        self.assertEqual(C.analyse([self.LUI_S0, lwc1])[1], [("hi", 0, 0x08090000), ("data", 1, 0x08090010)])

    def test_writes_through_ext_ll_sc_and_cfc1_end_the_high_half(self) -> None:
        writers = {"ext": self.ext(8, 9, 0, 8), "ll": (0x30 << 26) | (9 << 21) | (8 << 16),
                   "sc": (0x38 << 26) | (9 << 21) | (8 << 16), "cfc1": (0x11 << 26) | (2 << 21) | (8 << 16) | (31 << 11)}
        for name, w in writers.items():
            with self.subTest(name):
                sig, refs = C.analyse([self.LUI_T0, w, self.LW_A0_4_T0])
                self.assertEqual(refs, [("hi", 0, 0x08090000)])
                self.assertEqual(sig[2], self.LW_A0_4_T0)

    def test_seb_writes_rd_and_keeps_the_source(self) -> None:
        seb_t0_t1 = (0x1F << 26) | (9 << 16) | (8 << 11) | (0x10 << 6) | 0x20   # seb t0, t1
        _sig, refs = C.analyse([self.LUI_T0, seb_t0_t1, self.LW_A0_4_T0])
        self.assertEqual(refs, [("hi", 0, 0x08090000)])

    def test_jal_ends_a_high_half_in_ra(self) -> None:
        lui_ra = (0x0F << 26) | (31 << 16) | 0x0809
        lw_ra = (0x23 << 26) | (31 << 21) | (4 << 16) | 4
        _sig, refs = C.analyse([lui_ra, (3 << 26) | 0x100000, 0, lw_ra])
        self.assertEqual([r for r in refs if r[0] == "data"], [])


if __name__ == "__main__":
    unittest.main()
