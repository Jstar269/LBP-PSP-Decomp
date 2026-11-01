#!/usr/bin/env python3
"""Tests for tools/lbptu.py: translation units cut at the program's static constructors."""
from __future__ import annotations

import json
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpreport  # noqa: E402
import lbptu as T  # noqa: E402

# a startup function, then the game region 0x100..0x200 with seven functions, then library code
FUNCS = [(0x80, 0x100), (0x100, 0x110), (0x110, 0x120), (0x120, 0x130), (0x130, 0x140), (0x140, 0x150),
         (0x150, 0x160), (0x160, 0x200), (0x200, 0x210)]
REGION = (0x100, 0x200)


class RecoverTests(unittest.TestCase):
    def test_each_constructor_ends_a_unit_and_the_region_closes_the_last(self) -> None:
        units = T.recover(FUNCS, [0x120, 0x140, 0x208], REGION)
        self.assertEqual([(u["first"], u["last"], u["functions"], u["start"], u["end"]) for u in units],
                         [(0x100, 0x120, 3, "region", "ctors"), (0x130, 0x140, 2, "ctors", "ctors"),
                          (0x150, 0x160, 2, "ctors", "region")])
        self.assertEqual(sum(u["functions"] for u in units), 7)

    def test_a_constructor_at_the_end_of_the_region_leaves_no_empty_unit(self) -> None:
        units = T.recover(FUNCS, [0x160], REGION)
        self.assertEqual([(u["first"], u["last"], u["end"]) for u in units], [(0x100, 0x160, "ctors")])

    def test_without_constructors_the_region_is_one_unit(self) -> None:
        self.assertEqual(len(T.recover(FUNCS, [], REGION)), 1)

    def test_a_constructor_inside_a_listed_function_is_refused(self) -> None:
        with self.assertRaisesRegex(T.Refused, "0x00000124"):
            T.recover(FUNCS, [0x124], REGION)

    def test_entries_after_the_link_order_run_are_not_ends(self) -> None:
        # ld appends the sorted .ctors.<priority> sections: the 0x110 entry after 0x140 says nothing of link order
        self.assertEqual(T.linked([0x120, 0x140, 0x110, 0x150]), [0x120, 0x140])
        units = T.recover(FUNCS, [0x120, 0x140, 0x110], REGION)
        self.assertEqual([u["last"] for u in units], [0x120, 0x140, 0x160])

    def test_names_number_the_units_and_give_their_first_function(self) -> None:
        doc = T.document(T.recover(FUNCS, [0x120], REGION))
        self.assertEqual([u["name"] for u in doc["modules"][T.MODULE]], ["tu-001@00000100", "tu-002@00000130"])
        text = T.render(doc)
        self.assertEqual(json.loads(text), doc)
        self.assertEqual(sum(1 for line in text.splitlines() if line.lstrip().startswith('{"name":')), 2)

    def test_the_report_reads_the_file_the_tool_writes(self) -> None:
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "tu.json"
            path.write_text(T.render(T.document(T.recover(FUNCS, [0x120], REGION))), encoding="utf-8")
            units = lbpreport.read_units(path)[T.MODULE]
        self.assertEqual([lbpreport.unit_of(units, s) for s, _e in FUNCS],
                         [None, "tu-001@00000100", "tu-001@00000100", "tu-001@00000100", "tu-002@00000130",
                          "tu-002@00000130", "tu-002@00000130", "tu-002@00000130", None])

    def test_committed_file_names_only_addresses_counts_and_evidence(self) -> None:
        if not T.TU_PATH.exists():
            self.skipTest("no data/translation-units.json")
        doc = json.loads(T.TU_PATH.read_text(encoding="utf-8"))
        for u in doc["modules"][T.MODULE]:
            self.assertEqual(set(u), {"name", "first", "last", "functions", "start", "end"})
            self.assertIn(u["start"], ("ctors", "region"))
            self.assertIn(u["end"], ("ctors", "region"))


if __name__ == "__main__":
    unittest.main()
