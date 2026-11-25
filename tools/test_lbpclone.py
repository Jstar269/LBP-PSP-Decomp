#!/usr/bin/env python3
"""Game-free tests for the clone retargeting of tools/lbpclone.py (ported from hst-decomp's test_hstclone.py where the
behaviour is the same)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

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


if __name__ == "__main__":
    unittest.main()
