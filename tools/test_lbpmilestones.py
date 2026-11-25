#!/usr/bin/env python3
"""Tests for the milestones and their record (lbpmilestones.py): every measure on synthetic evidence, the first
reached date kept forever and the REGRESSED flag. No WSL, no retail bytes."""
from __future__ import annotations

import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmilestones as MS  # noqa: E402
import lbpreport as R  # noqa: E402
from test_lbpprogress import FIGURES, MODULES, NO_INPUTS, make_tree, run  # noqa: E402

DEFS = {"schema": MS.DEFS_SCHEMA, "milestones": [
    {"id": "a", "title": "A", "criterion": "a", "measure": "t1", "modules": ["EBOOT.BIN"]},
    {"id": "b", "title": "B", "criterion": "b", "measure": "inventory"},
    {"id": "c", "title": "C", "criterion": "c", "measure": "evidence"}]}


def stub_image(nids: list[int], lib: bytes = b"sceCtrl") -> bytes:
    """An ELF32 image with one PT_LOAD (file offset 0x100 at 0x1000) holding a .lib.stub table of one library."""
    base, off = 0x1000, 0x100
    data = bytearray(0x80)
    data[0:len(lib)] = lib
    for k, nid in enumerate(nids):
        struct.pack_into("<I", data, 0x10 + 4 * k, nid)
    struct.pack_into("<IHHBBHII", data, 0x40, base, 0x11, 0x4001, 5, 0, len(nids), base + 0x10, base + 0x60)
    names = b"\0.lib.stub\0.shstrtab\0"
    shoff = off + len(data) + len(names)
    eh = b"\x7fELF" + bytes([1, 1, 1]) + bytes(9) + struct.pack("<HHIIIIIHHHHHH", 2, 8, 1, base, 52, shoff, 0, 52,
                                                                  32, 1, 40, 3, 2)
    ph = struct.pack("<IIIIIIII", 1, off, base, base, len(data), len(data), 7, 16)
    image = bytearray(eh + ph)
    image += bytes(off - len(image)) + data + names
    image += struct.pack("<IIIIIIIIII", *([0] * 10))
    image += struct.pack("<IIIIIIIIII", 1, 1, 2, base + 0x40, off + 0x40, 20, 0, 0, 4, 0)
    image += struct.pack("<IIIIIIIIII", 11, 3, 0, 0, off + len(data), len(names), 0, 0, 1, 0)
    return bytes(image)


class MeasureTests(unittest.TestCase):
    def test_t1_counts_the_public_scope_and_names_kept_out_modules(self) -> None:
        r = MS.measure_t1(["EBOOT.BIN", "x.prx"], FIGURES)
        self.assertEqual(r["progress"], 28.571)
        self.assertIn("x.prx: Sony code kept out of this repository, not counted", r["detail"])
        self.assertIsNone(MS.measure_t1(["x.prx"], FIGURES)["progress"])

    def test_a_share_reaches_100_only_when_complete(self) -> None:
        self.assertEqual(MS.share(999_999, 1_000_000), MS.CAP)        # rounds to 100.0, but is not complete
        self.assertEqual(MS.share(5, 5), 100.0)
        self.assertEqual(MS.share(0, 0), 0.0)

    def test_builds_are_reached_only_when_every_byte_is_built_and_identical(self) -> None:
        self.assertIsNone(MS.measure_build(["EBOOT.BIN"], FIGURES, "t2")["progress"])
        self.assertIn("no whole-module build report", MS.measure_build(["EBOOT.BIN"], FIGURES, "t2")["detail"])
        figs = json.loads(json.dumps(FIGURES))
        figs["EBOOT.BIN"]["t2"] = {"public_t2_bytes": 0x1C0, "image_identical": True}
        self.assertEqual(MS.measure_build(["EBOOT.BIN"], figs, "t2")["progress"], 100.0)
        figs["EBOOT.BIN"]["t2"]["image_identical"] = False
        self.assertEqual(MS.measure_build(["EBOOT.BIN"], figs, "t2")["progress"], MS.CAP)
        figs["EBOOT.BIN"]["t2"] = {"public_t2_bytes": 0xE0, "image_identical": True}
        self.assertEqual(MS.measure_build(["EBOOT.BIN"], figs, "t2")["progress"], 50.0)
        figs["N.PRX"] = {**FIGURES["EBOOT.BIN"]}                        # a module without a build: never 100%
        figs["EBOOT.BIN"]["t2"]["public_t2_bytes"] = 0x1C0
        r = MS.measure_build(["EBOOT.BIN", "N.PRX"], figs, "t2")
        self.assertEqual(r["progress"], 50.0)
        self.assertIn("not measured for N.PRX", r["detail"])

    def test_inventory(self) -> None:
        self.assertIsInstance(MS.measure_inventory(None), MS.Unavailable)
        fns = [{"start": 0, "end": 16, "boundary_status": "reviewed"}, {"start": 16, "end": 32, "boundary_status": "auto"}]
        r = MS.measure_inventory({"modules": [{"functions": fns}]})
        self.assertEqual(r["progress"], 50.0)
        self.assertIn("no region inventory", r["detail"])
        done = [dict(f, boundary_status="reviewed") for f in fns]
        self.assertEqual(MS.measure_inventory({"modules": [{"functions": done}]})["progress"], MS.CAP)
        regions = [{"start": 0, "end": 64, "review_status": "reviewed"}]
        self.assertEqual(MS.measure_inventory({"modules": [{"functions": done, "regions": regions}]})["progress"], 100.0)
        open_ = {"functions": done, "regions": regions, "function_candidates": [{"boundary_status": "auto"}]}
        self.assertEqual(MS.measure_inventory({"modules": [open_]})["progress"], MS.CAP)

    def test_platform_from_the_stub_table_and_nakagawas_audit(self) -> None:
        image = stub_image([0x11111111, 0x22222222, 0x33333333])
        self.assertEqual(MS.stub_imports(image), [("sceCtrl", 0x11111111), ("sceCtrl", 0x22222222),
                                                  ("sceCtrl", 0x33333333)])
        audit = {0x11111111: {"classification": "dedicated", "status": "complete"},
                 0x22222222: {"classification": "generic", "status": "complete"}}
        self.assertEqual(MS.classify_imports([0x11111111, 0x22222222, 0x33333333], audit),
                         {"complete": 1, "partial or unreviewed": 0, "refused or fake": 1, "missing": 1})
        r = MS.measure_platform(MS.stub_imports(image), audit)
        self.assertEqual(r["progress"], 33.333)
        self.assertIsInstance(MS.measure_platform(None, audit), MS.Unavailable)

    def test_evidence_and_the_strict_tier(self) -> None:
        rows = MS.compute({**DEFS, "evidence": [{"id": "c", "evidence": "docs/x.md"}]}, FIGURES, MODULES)
        self.assertEqual([(r["id"], r["progress"]) for r in rows][0::2], [("a", 28.571), ("c", 100.0)])
        self.assertIsInstance(rows[1]["progress"], MS.Unavailable)
        with self.assertRaises(ValueError):
            MS.compute({**DEFS, "milestones": [{"id": "z", "title": "Z", "criterion": "z", "measure": "guess"}]},
                       FIGURES, MODULES)


class RecordTests(unittest.TestCase):
    def rows(self, a, b) -> list[dict]:
        return [{"id": "a", "title": "A", "progress": a, "detail": f"a {a}"},
                {"id": "b", "title": "B", "progress": b, "detail": f"b {b}"}]

    def test_the_first_reached_date_is_kept_and_a_drop_is_regressed(self) -> None:
        one = MS.record(self.rows(100.0, 10.0), None, "2026-11-01")
        self.assertEqual([(m["reached"], m["regressed"]) for m in one["milestones"]],
                         [("2026-11-01", False), (None, False)])
        two = MS.record(self.rows(99.0, 100.0), one, "2026-11-02")
        self.assertEqual([(m["reached"], m["regressed"]) for m in two["milestones"]],
                         [("2026-11-01", True), ("2026-11-02", False)])
        three = MS.record(self.rows(None, 100.0), two, "2026-11-03")      # no longer measured: still REGRESSED
        self.assertEqual(three["milestones"][0]["reached"], "2026-11-01")
        self.assertTrue(three["milestones"][0]["regressed"])
        back = MS.record(self.rows(100.0, 100.0), three, "2026-11-04")
        self.assertEqual((back["milestones"][0]["reached"], back["milestones"][0]["regressed"]), ("2026-11-01", False))

    def test_a_measure_not_available_here_keeps_the_maintainers_value(self) -> None:
        old = MS.record(self.rows(50.0, 100.0), None, "2026-11-01")
        new = MS.record(self.rows(50.0, MS.Unavailable("needs the campaign")), old, "2026-11-02")
        self.assertEqual(new["milestones"][1], old["milestones"][1])
        fresh = MS.record(self.rows(50.0, MS.Unavailable("needs the campaign")), None, "2026-11-02")
        self.assertIsNone(fresh["milestones"][1]["progress"])
        problems, skipped = MS.record_problems(self.rows(50.0, MS.Unavailable("x")), new, "2026-11-02")
        self.assertEqual((problems, skipped), ([], ["b"]))

    def test_record_problems(self) -> None:
        rows = self.rows(100.0, 20.0)
        doc = MS.record(rows, None, "2026-11-01")
        self.assertEqual(MS.record_problems(rows, doc, "2026-11-01"), ([], []))
        self.assertEqual(len(MS.record_problems(rows, doc, "2026-11-05")[0]), 1)          # figures are newer
        self.assertEqual(MS.record_problems(rows, None, None)[0], [f"{MS.RECORD_REL} is missing"])
        flag = json.loads(json.dumps(doc))
        flag["milestones"][0]["regressed"] = True
        self.assertIn("REGRESSED", MS.record_problems(rows, flag, "2026-11-01")[0][0])
        hidden = json.loads(json.dumps(doc))
        hidden["milestones"][0]["reached"] = None                                         # history rewritten
        self.assertIn("without a reached date", MS.record_problems(rows, hidden, "2026-11-01")[0][0])
        moved = json.loads(json.dumps(doc))
        moved["milestones"][1]["progress"] = 30.0
        self.assertIn("measured 20.0", MS.record_problems(rows, moved, "2026-11-01")[0][0])
        self.assertIn("in that order", MS.record_problems(rows[::-1], doc, "2026-11-01")[0][0])


class TreeTests(unittest.TestCase):
    def test_check_on_a_tree(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tree = make_tree(Path(tmp))
            self.assertEqual(MS.tree_problems(tree, require=True, inputs=NO_INPUTS), [])
            ok = run("lbpmilestones.py", "--check", "--tree", str(tree))
            self.assertEqual(ok.returncode, 0, ok.stdout + ok.stderr)
            self.assertIn("skipped", ok.stdout)                     # inventory and platform need the owner's machine
            figs = json.loads(json.dumps(FIGURES))
            figs["EBOOT.BIN"].update(public_t1_bytes=0x40, public_t1_percent=14.286, public_t1_functions=1)
            (tree / R.FIGURES_REL).write_text(json.dumps(figs), encoding="utf-8")
            self.assertEqual(run("lbpmilestones.py", "--check", "--tree", str(tree)).returncode, 1)
            MS.write_tree(tree, inputs=NO_INPUTS)
            self.assertEqual(MS.tree_problems(tree, inputs=NO_INPUTS), [])


@unittest.skipUnless((MS.ROOT / R.FIGURES_REL).is_file(), "this tree has no data/public-figures.json")
class CommittedTests(unittest.TestCase):
    def test_committed_record_is_current(self) -> None:
        self.assertEqual(MS.tree_problems(MS.ROOT, require=True, inputs=NO_INPUTS), [])


if __name__ == "__main__":
    unittest.main()
