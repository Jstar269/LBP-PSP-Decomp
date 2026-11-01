#!/usr/bin/env python3
"""Tests for the public objdiff report (lbpreport.py): it matches the published figures and holds no per-function or
game-derived detail. No WSL, no retail bytes."""
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
                     "t2": "not measured (stale)", "t3": "not measured"}}
WITH_T2 = {"M.BIN": {"public_scope_bytes": 1000, "public_t1_bytes": 250, "public_t1_percent": 25.0,
                     "t2": {"public_t2_bytes": 200, "public_t2_data_bytes": 16, "public_t2_percent": 20.0},
                     "t3": "not measured"}}
KEYS = {"measures", "units", "version", "categories", "name", "metadata", "module_name", "progress_categories", "id",
        "fuzzy_match_percent", "total_code", "matched_code", "matched_code_percent", "total_data", "matched_data",
        "matched_data_percent", "total_functions", "matched_functions", "matched_functions_percent", "complete_code",
        "complete_code_percent", "complete_data", "complete_data_percent", "total_units", "complete_units"}


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
    def test_published_percentage_is_reproduced(self) -> None:
        figures = R.load_figures()
        report = R.build_report(figures)
        self.assertEqual(len(report["units"]), len(figures))
        for unit in report["units"]:
            fig = figures[unit["name"]]
            t2 = fig["t2"]
            want = t2["public_t2_percent"] if isinstance(t2, dict) else fig["public_t1_percent"]
            self.assertEqual(unit["measures"]["matched_code_percent"], want)
            self.assertEqual(unit["measures"]["fuzzy_match_percent"], want)
            self.assertEqual(unit["measures"]["total_code"], str(fig["public_scope_bytes"]))
        if len(figures) == 1:
            self.assertEqual(report["measures"], report["units"][0]["measures"])

    def test_committed_report_is_current(self) -> None:
        self.assertEqual(R.differences(R.load_figures()), [], "run python tools/lbpreport.py and commit the result")
        self.assertEqual(R.REPORT_PATH.read_text(encoding="utf-8"), R.render(R.build_report(R.load_figures())))

    def test_holds_only_counts_and_module_names(self) -> None:
        figures = R.load_figures()
        committed = json.loads(R.REPORT_PATH.read_text(encoding="utf-8"))
        for key, value in scalars(committed):
            self.assertIn(key, KEYS)
            if isinstance(value, str):
                self.assertTrue(value in figures or value.isdigit(), f"unexpected string {value!r}")
        text = R.REPORT_PATH.read_text(encoding="utf-8")
        for pattern in (r"FUN_", r"DAT_", r"0x[0-9a-fA-F]+", r"\b0[0-9a-f]{7}\b", r"[/\\]", r"\.c\b", r"sha256"):
            self.assertIsNone(re.search(pattern, text), pattern)

    def test_t1_only_and_t2_bases(self) -> None:
        t1 = R.build_report(T1_ONLY)
        self.assertEqual((t1["measures"]["matched_code"], t1["measures"]["matched_code_percent"]), ("250", 25.0))
        self.assertEqual(t1["measures"]["total_data"], "0")
        t2 = R.build_report(WITH_T2)
        self.assertEqual((t2["measures"]["matched_code"], t2["measures"]["matched_code_percent"]), ("200", 20.0))
        self.assertEqual(t2["measures"]["total_data"], "16")
        self.assertEqual(t2["measures"]["complete_code"], "0")

    def test_inconsistent_figures_are_refused(self) -> None:
        bad = json.loads(json.dumps(T1_ONLY))
        bad["M.BIN"]["public_t1_percent"] = 26.0
        with self.assertRaises(ValueError):
            R.build_report(bad)

    def test_check_notices_a_stale_report(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "r.json"
            self.assertEqual(R.differences(T1_ONLY, path), ["r.json does not exist"])
            path.write_text(R.render(R.build_report(T1_ONLY)), encoding="utf-8")
            self.assertEqual(R.differences(T1_ONLY, path), [])
            self.assertTrue(R.differences(WITH_T2, path))

    def test_readme_disagreement(self) -> None:
        report = R.build_report(T1_ONLY)
        self.assertEqual(R.readme_disagreement("no figure here", report), "")
        self.assertEqual(R.readme_disagreement("produce 250 code bytes of it, 25.0% of the public scope", report), "")
        self.assertIn("refresh", R.readme_disagreement("produce 1,000 code bytes of it, 9.5% of the public scope", report))


if __name__ == "__main__":
    unittest.main()
