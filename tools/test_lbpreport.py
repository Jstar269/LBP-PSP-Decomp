#!/usr/bin/env python3
"""Tests for the public objdiff report (lbpreport.py): it is a pure function of the published figures (bytes and
function counts, tier T1) and holds no per-function or game-derived detail. No WSL, no retail bytes. The tests on the
files of a tree (data/public-figures.json, docs/objdiff-report.json) run where the tree has them (the public
repository); in the private repository lbppublish.py writes them into a built tree and its tests cover that."""
from __future__ import annotations

import json
import re
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpreport as R  # noqa: E402

T1_ONLY = {"M.BIN": {"public_scope_bytes": 1000, "public_t1_bytes": 250, "public_t1_percent": 25.0,
                     "public_scope_functions": 40, "public_t1_functions": 10,
                     "t2": "not measured (stale)", "t3": "not measured"}}
WITH_T2 = {"M.BIN": {"public_scope_bytes": 1000, "public_t1_bytes": 250, "public_t1_percent": 25.0,
                     "public_scope_functions": 40, "public_t1_functions": 10,
                     "t2": {"public_t2_bytes": 200, "public_t2_data_bytes": 16, "public_t2_percent": 20.0},
                     "t3": "not measured"}}
TWO = {**T1_ONLY, "N.PRX": {"public_scope_bytes": 500, "public_t1_bytes": 100, "public_t1_percent": 20.0,
                            "public_scope_functions": 20, "public_t1_functions": 5, "t2": "x", "t3": "x"}}
KEYS = {"measures", "units", "version", "categories", "name", "metadata", "module_name", "progress_categories", "id",
        "fuzzy_match_percent", "total_code", "matched_code", "matched_code_percent", "total_data", "matched_data",
        "matched_data_percent", "total_functions", "matched_functions", "matched_functions_percent", "complete_code",
        "complete_code_percent", "complete_data", "complete_data_percent", "total_units", "complete_units"}
HAVE_TREE = R.FIGURES_PATH.exists() and R.REPORT_PATH.exists()


def scalars(node, key=None):
    """Every (key, scalar) in a JSON value; list items carry the key of their list."""
    if isinstance(node, dict):
        for k, v in node.items():
            yield from scalars(v, k)
    elif isinstance(node, list):
        for v in node:
            yield from scalars(v, key)
    else:
        yield key, node


class ReportTests(unittest.TestCase):
    def test_bytes_and_function_counts_are_reproduced(self) -> None:
        report = R.build_report(T1_ONLY)
        m = report["measures"]
        self.assertEqual((m["total_code"], m["matched_code"], m["matched_code_percent"]), ("1000", "250", 25.0))
        self.assertEqual((m["total_functions"], m["matched_functions"], m["matched_functions_percent"]), (40, 10, 25.0))
        self.assertEqual(m["fuzzy_match_percent"], 25.0)
        self.assertEqual(report["units"][0]["measures"], m)
        self.assertEqual(report["categories"][0]["measures"], m)

    def test_the_report_is_tier_t1_whatever_t2_says(self) -> None:
        self.assertEqual(R.build_report(WITH_T2), R.build_report(T1_ONLY))
        m = R.build_report(WITH_T2)["measures"]
        self.assertEqual((m["total_data"], m["matched_data"], m["complete_code"]), ("0", "0", "0"))

    def test_modules_add_up(self) -> None:
        m = R.build_report(TWO)["measures"]
        self.assertEqual((m["total_code"], m["matched_code"], m["total_functions"], m["matched_functions"],
                          m["total_units"]), ("1500", "350", 60, 15, 2))
        self.assertEqual(m["matched_code_percent"], round(100 * 350 / 1500, 3))

    def test_empty_scope_is_zero_percent(self) -> None:
        m = R.measures(0, 0, 0, 0, 1)
        self.assertEqual((m["matched_code_percent"], m["matched_functions_percent"]), (0.0, 0.0))

    def test_inconsistent_figures_are_refused(self) -> None:
        for field, value in (("public_t1_percent", 26.0), ("public_t1_bytes", 1001), ("public_t1_functions", 41)):
            bad = json.loads(json.dumps(T1_ONLY))
            bad["M.BIN"][field] = value
            with self.assertRaises(ValueError, msg=field):
                R.build_report(bad)

    def test_figures_without_function_counts_are_refused(self) -> None:
        old = json.loads(json.dumps(T1_ONLY))
        del old["M.BIN"]["public_scope_functions"]
        with self.assertRaises(KeyError):
            R.build_report(old)

    def test_only_counts_and_module_names(self) -> None:
        text = R.render(R.build_report(TWO))
        for key, value in scalars(json.loads(text)):
            self.assertIn(key, KEYS)
            if isinstance(value, str):
                self.assertTrue(value in TWO or value.isdigit(), f"unexpected string {value!r}")
        for pattern in (r"FUN_", r"DAT_", r"0x[0-9a-fA-F]+", r"\b0[0-9a-f]{7}\b", r"[/\\]", r"\.c\b", r"sha256"):
            self.assertIsNone(re.search(pattern, text), pattern)

    def test_check_notices_a_stale_report(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "r.json"
            self.assertEqual(R.differences(T1_ONLY, path), ["r.json does not exist"])
            path.write_text(R.render(R.build_report(T1_ONLY)), encoding="utf-8")
            self.assertEqual(R.differences(T1_ONLY, path), [])
            newer = json.loads(json.dumps(T1_ONLY))
            newer["M.BIN"].update(public_t1_functions=11)
            self.assertIn("r.json matched_functions: 10, figures say 11", R.differences(newer, path))
            path.write_text("{", encoding="utf-8")
            self.assertTrue(R.differences(T1_ONLY, path)[0].startswith("r.json is not valid JSON"))

    def test_cli_writes_then_checks_a_tree(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tree = Path(tmp)
            (tree / "data").mkdir()
            (tree / R.FIGURES_REL).write_text(json.dumps(T1_ONLY), encoding="utf-8")
            argv = sys.argv
            try:
                sys.argv = ["lbpreport.py", "--tree", str(tree), "--check"]
                self.assertEqual(R.main(), 1)               # no report yet
                sys.argv = ["lbpreport.py", "--tree", str(tree)]
                self.assertEqual(R.main(), 0)
                sys.argv = ["lbpreport.py", "--tree", str(tree), "--check"]
                self.assertEqual(R.main(), 0)
            finally:
                sys.argv = argv
            self.assertEqual((tree / R.REPORT_REL).read_text(encoding="utf-8"), R.render(R.build_report(T1_ONLY)))

    @unittest.skipUnless(HAVE_TREE, "this tree has no data/public-figures.json and docs/objdiff-report.json")
    def test_committed_report_is_current(self) -> None:
        figures = R.load_figures()
        self.assertEqual(R.differences(figures), [], "the report differs from the figures")
        committed = json.loads(R.REPORT_PATH.read_text(encoding="utf-8"))
        self.assertEqual(len(committed["units"]), len(figures))
        for key, value in scalars(committed):
            self.assertIn(key, KEYS)
            if isinstance(value, str):
                self.assertTrue(value in figures or value.isdigit(), f"unexpected string {value!r}")


if __name__ == "__main__":
    unittest.main()
