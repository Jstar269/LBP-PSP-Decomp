#!/usr/bin/env python3
"""Tests for the batch builder (lbpqueue.py): retail-word flags, candidate selection, reservations."""
from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpqueue as Q  # noqa: E402


def ins(rt: int, rs: int, lsb: int, msb: int) -> int:
    return (0x1F << 26) | (rs << 21) | (rt << 16) | (msb << 11) | (lsb << 6) | 0x04


class FlagTests(unittest.TestCase):
    def test_ins_from_zero_is_counted_and_other_ins_are_not(self) -> None:
        ws = [ins(2, 0, 0, 2),      # ins v0,$zero,0,3: x & ~7
              ins(2, 0, 4, 7),      # ins v0,$zero,4,4: a bit-field clear
              ins(2, 3, 0, 2),      # ins v0,v1,0,3: not from $zero
              0x24420001]           # addiu v0,v0,1
        self.assertEqual(Q.flags(ws), {"ins_zero": 2, "vfpu": 0, "unaligned": 0, "div": 0})

    def test_vfpu_words_are_counted(self) -> None:
        self.assertEqual(Q.flags([0xD8000000, 0x48600000, 0x03E00008])["vfpu"], 2)   # lv.q, mtv; jr ra is not

    def test_unaligned_words_are_counted(self) -> None:
        ws = [0x88820004, 0x98820001, 0xA8850004, 0xB8850001,   # lwl, lwr, swl, swr
              0x8C820004, 0xAC850004]                          # lw, sw are not
        self.assertEqual(Q.flags(ws)["unaligned"], 4)

    def test_division_words_are_counted(self) -> None:
        ws = [0x00A2001B,                                       # divu zero,a1,v0
              0x0082001A,                                       # div zero,a0,v0
              0x000001CD,                                       # break 7 is not
              0x00001810, 0x00A20019]                           # mfhi, multu are not
        self.assertEqual(Q.flags(ws)["div"], 2)

    def test_single_weight_peaks_near_192_bytes(self) -> None:
        self.assertGreater(Q.single_weight(192), Q.single_weight(64))
        self.assertGreater(Q.single_weight(192), Q.single_weight(1024))


class FileTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def test_best_generated_keeps_the_fewest_differing_words(self) -> None:
        for tag, w in (("g1", 9), ("g14", 3)):
            d = self.root / "try" / f"gen-{tag}"
            d.mkdir(parents=True)
            (d / "summary-pass1-0.json").write_text(json.dumps([
                {"source": f"try/gen-{tag}/v0/FUN_08804000.c", "start": "0x08804000", "status": "MISMATCH",
                 "detail": f"9/9 words; {w} differ", "words_differ": w},
                {"source": "x", "start": "0x08804100", "status": "COMPILE_FAILED", "detail": "", "words_differ": None}]),
                encoding="utf-8")
        best = Q.best_generated(self.root)
        self.assertEqual(best[0x08804000], {"source": "try/gen-g14/v0/FUN_08804000.c", "tag": "g14",
                                            "differing_words": 3, "words": 9})
        self.assertNotIn(0x08804100, best)

    def test_reservations(self) -> None:
        queue = self.root / "try" / "agent-queue"
        queue.mkdir(parents=True)

        def batch(path: Path, *addrs: int) -> None:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(json.dumps({"items": [{"address": f"0x{a:08x}"} for a in addrs]}), encoding="utf-8")

        batch(queue / "q-01.json", 0x1000)                 # queued, no lane yet: reserved
        batch(queue / "q-02.json", 0x2000, 0x2100, 0x2200)  # taken by a harvested lane
        batch(queue / "q-03.json", 0x3000)                 # taken by a running lane
        ext = self.root / "ext"
        for lane, b in (("done", "q-02.json"), ("busy", "q-03.json")):
            (ext / lane).mkdir(parents=True)
            (ext / lane / "LANE.json").write_text(json.dumps({"lane": lane, "batch": b}), encoding="utf-8")
            batch(ext / lane / "try" / "agent-queue" / b, *Q.batch_addresses(queue / b))
        (ext / "done" / "PROGRESS.md").write_text("0x00002000 EXACT src/eboot/a.c\n0x00002100 SKIP too big\n",
                                                  encoding="utf-8")
        (self.root / "try" / "harvest-done").mkdir()
        reserved, attempted = Q.reservations(self.root, sorted(p for p in ext.iterdir()), queue)
        self.assertEqual(reserved, {0x1000, 0x3000})
        self.assertEqual(attempted, {0x2000, 0x2100})   # 0x2200 was never reached: free again


if __name__ == "__main__":
    unittest.main()
