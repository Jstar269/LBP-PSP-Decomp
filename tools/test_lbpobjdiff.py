#!/usr/bin/env python3
"""Tests for the objdiff project generator (lbpobjdiff.py) on synthetic objects: the unit plan, the objects it writes
and the check against the ledger and lbpcov.py. No WSL, no retail bytes, no objdiff."""
from __future__ import annotations

import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402
import lbpobjdiff as O  # noqa: E402

LEDGER = {"M.BIN": [(0x1000, 0x1010), (0x1010, 0x1020), (0x1020, 0x1040), (0x1040, 0x1050)]}
RETAIL = bytes(range(256)) * 2          # the module's code at 0x1000.., a synthetic stand-in


def retail(_module: str, start: int, size: int) -> bytes:
    return RETAIL[start - 0x1000:start - 0x1000 + size]


def header(start: int, end: int, symbol: str | None = None) -> dict:
    return {"module": "M.BIN", "start": start, "end": end, "symbol": symbol or f"FUN_{start:08x}"}


class ElfTests(unittest.TestCase):
    def test_an_object_round_trips_and_has_the_header_objdiff_reads(self) -> None:
        obj = O.elf32(0x1000, b"\x01\x02\x03\x04" * 3, [("FUN_00001000", 0x1000, 12)], e_flags=0x10A23001)
        self.assertEqual(O.read_text(obj), (0x1000, b"\x01\x02\x03\x04" * 3, [("FUN_00001000", 0x1000, 12)]))
        machine, = struct.unpack_from("<H", obj, 18)
        flags, = struct.unpack_from("<I", obj, 0x24)
        self.assertEqual((obj[:4], obj[4], obj[5], machine, flags), (b"\x7fELF", 1, 1, 8, 0x10A23001))
        names = [n for n, _ in M.elf_sections(obj)]
        self.assertEqual(names, ["", ".text", ".symtab", ".strtab", ".shstrtab"])
        self.assertEqual(O.read_text(O.elf32(0, b"", [])), (0, b"", []))

    def test_symbols_are_clamped_to_what_an_object_holds(self) -> None:
        self.assertEqual(O.clamp([("a", 0x1000, 16), ("b", 0x1010, 16)], 0x1000, 20), [("a", 0x1000, 16),
                                                                                      ("b", 0x1010, 4)])

    def test_linked_code_is_what_the_matcher_compares(self) -> None:
        linked = O.elf32(0x1000, RETAIL[:0x18], [("f", 0x1000, 0x18)])       # a function longer than its span
        code, notes = O.linked_code(linked, "f", 0x1000, 0x10)
        self.assertEqual((len(code), len(notes)), (0x18, 1))
        code, notes = O.linked_code(O.elf32(0x1000, RETAIL[:0x10], [("f", 0x1000, 0x10)]), "f", 0x1000, 0x10)
        self.assertEqual((code, notes), (RETAIL[:0x10], []))
        with self.assertRaises(ValueError):
            O.linked_code(O.elf32(0x1004, RETAIL[:0x10], [("f", 0x1004, 0x10)]), "f", 0x1000, 0x10)


class PlanTests(unittest.TestCase):
    def test_lbpcovs_rule_and_one_unit_per_function(self) -> None:
        fns = LEDGER["M.BIN"]
        self.assertEqual(O.covered(fns, 0x1000, 0x1020), [(0x1000, 0x1010), (0x1010, 0x1020)])
        self.assertEqual(O.covered(fns, 0x1020, 0x1030), [(0x1020, 0x1040)])     # a near miss shorter than its function
        self.assertEqual(O.covered(fns, 0x1004, 0x1010), [])
        units, rest, superseded, skipped = O.plan([
            ("wip/x.c", header(0x1000, 0x1010)), ("src/m/a.c", header(0x1000, 0x1020)),
            ("src/m/b.c", header(0x1010, 0x1020)), ("wip/y.c", header(0x1040, 0x1050)),
            ("src/m/q.c", {**header(0x1000, 0x1010), "module": "Q.BIN"})], LEDGER)
        self.assertEqual([(u["source"], u["complete"]) for u in units], [("src/m/a.c", True), ("wip/y.c", False)])
        self.assertEqual(rest, {"M.BIN": [(0x1020, 0x1040)]})
        self.assertEqual([s.split(":")[0] for s in superseded], ["wip/x.c"])
        self.assertEqual(sorted(s.split(":")[0] for s in skipped), ["src/m/b.c", "src/m/q.c"])


class ProjectTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.out = self.root / "build" / "objdiff"
        self.project = self.root / "objdiff.json"
        self.linked = {"src/m/a.c": O.elf32(0x1000, RETAIL[:0x20], [("FUN_00001000", 0x1000, 0x20)]),
                       "wip/y.c": O.elf32(0x1040, b"\0" * 0x10, [("FUN_00001040", 0x1040, 0x10)])}

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def generate(self, sources: list[tuple[str, dict]], subset: bool = False) -> dict:
        units, rest, _sup, _skip = O.plan(sources, LEDGER)
        return O.write_project(self.root, units, rest, retail,
                               lambda rel, _h: (self.linked[rel], None) if rel in self.linked else None,
                               self.out, self.project, subset)

    def test_a_project_that_agrees_with_the_ledger_and_lbpcov(self) -> None:
        summary = self.generate([("src/m/a.c", header(0x1000, 0x1020)), ("wip/y.c", header(0x1040, 0x1050))])
        self.assertEqual((summary["units"], summary["unmatched_functions"]), (3, {"M.BIN": 1}))
        doc = json.loads(self.project.read_text(encoding="utf-8"))
        self.assertEqual(doc["options"], {"mips_instr_category": "r4000allegrex"})
        self.assertFalse(doc["build_base"] or doc["build_target"])
        self.assertEqual([u["name"] for u in doc["units"]], ["src/m/a", "wip/y", "unmatched/M.BIN"])
        cov = {"M.BIN": {"code_bytes": 0x50, "exact_bytes": 0x20}}
        problems, totals = O.project_problems(self.root, doc, LEDGER, cov)
        self.assertEqual(problems, [])
        self.assertEqual(totals["M.BIN"], {"total_code": 0x50, "complete_code": 0x20})
        unmatched = O.read_text((self.root / doc["units"][2]["target_path"]).read_bytes())
        self.assertEqual(unmatched[0:2], (0x1020, RETAIL[0x20:0x40]))
        report = {"categories": [{"id": "M.BIN", "measures": {"total_code": "80", "complete_code": "32"}}],
                  "units": [{"name": "src/m/a", "measures": {"total_code": "32", "matched_code": "32"}}]}
        self.assertEqual(O.report_problems(report, totals, ["src/m/a"]), [])
        report["units"][0]["measures"]["matched_code"] = "28"
        report["categories"][0]["measures"]["complete_code"] = "28"
        self.assertEqual(len(O.report_problems(report, totals, ["src/m/a"])), 2)

    def test_check_finds_what_does_not_add_up(self) -> None:
        self.linked["src/m/a.c"] = O.elf32(0x1000, b"\xff" + RETAIL[1:0x20], [("FUN_00001000", 0x1000, 0x20)])
        self.linked["wip/y.c"] = O.elf32(0x1040, RETAIL[0x40:0x50], [("FUN_00001040", 0x1040, 0x10)])
        self.generate([("src/m/a.c", header(0x1000, 0x1020)), ("wip/y.c", header(0x1040, 0x1050))])
        doc = json.loads(self.project.read_text(encoding="utf-8"))
        problems, _ = O.project_problems(self.root, doc, LEDGER, {"M.BIN": {"code_bytes": 0x50, "exact_bytes": 0x20}})
        self.assertTrue(any("differ from retail" in p for p in problems))
        self.assertTrue(any("near miss whose linked bytes equal retail" in p for p in problems))
        self.assertTrue(any("lbpcov.py" in p for p in problems))
        bigger = {"M.BIN": LEDGER["M.BIN"] + [(0x1050, 0x1060)]}
        self.assertTrue(any("in no unit" in p for p in O.project_problems(self.root, doc, bigger, {})[0]))

    def test_a_missing_artifact_is_skipped_and_its_functions_are_unmatched(self) -> None:
        summary = self.generate([("src/m/a.c", header(0x1000, 0x1020)), ("src/m/z.c", header(0x1040, 0x1050))])
        self.assertEqual([s.split(":")[0] for s in summary["skipped"]], ["src/m/z.c"])
        doc = json.loads(self.project.read_text(encoding="utf-8"))
        problems, _ = O.project_problems(self.root, doc, LEDGER, {"M.BIN": {"code_bytes": 0x50, "exact_bytes": 0x30}})
        self.assertTrue(any("in no unit" in p for p in problems), problems)

    def test_a_subset_writes_no_unmatched_units_and_leaves_other_objects(self) -> None:
        self.generate([("src/m/a.c", header(0x1000, 0x1020)), ("wip/y.c", header(0x1040, 0x1050))])
        unmatched = self.out / "unmatched" / "M.BIN.target.o"
        before = unmatched.read_bytes()
        self.project = self.root / "subset.json"
        summary = self.generate([("src/m/a.c", header(0x1000, 0x1020))], subset=True)
        self.assertEqual(summary["unmatched_functions"], {})
        self.assertEqual(unmatched.read_bytes(), before)
        self.assertTrue((self.out / "units" / "wip" / "y.base.o").is_file())
        self.project = self.root / "objdiff.json"
        self.generate([("src/m/a.c", header(0x1000, 0x1020))])           # a full run removes what it no longer names
        self.assertFalse((self.out / "units" / "wip" / "y.base.o").exists())


if __name__ == "__main__":
    unittest.main()
