#!/usr/bin/env python3
"""Tests for the tier ladder and its public files (lbpprogress.py), on a synthetic public tree. No WSL, no retail
bytes. make_tree() is shared with test_lbpmilestones.py and test_lbpxverify.py."""
from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmilestones as MS  # noqa: E402
import lbpprogress as G  # noqa: E402
import lbppublish as P  # noqa: E402
import lbpreport as R  # noqa: E402

TOOLS = Path(__file__).resolve().parent
NO_INPUTS = {"campaign": None, "imports": None, "audit": None}
MODULES = {"schema": "lbp-decomp.modules.v1", "modules": [
    {"name": "EBOOT.BIN", "kind": "game", "size": 0x10000, "sha256": "0" * 64},
    {"name": "x.prx", "kind": "sony-prx", "size": 0x100, "sha256": "1" * 64}]}
FUNCTIONS = [(0x1000, 0x1040), (0x1040, 0x1080), (0x1080, 0x10C0), (0x2000, 0x2100), (0x3000, 0x3010)]
# the public scope: every function but 0x3000 (Sony code, out of the public scope)
ROWS = {"EBOOT.BIN": [(0x1000, 0x40, 0x40, "src/eboot/a_00001000.c", "do_a"), (0x1040, 0x40, 0, None, None),
                      (0x1080, 0x40, 0x40, "src/eboot/b_00001080.c", "FUN_00001080"),
                      (0x2000, 0x100, 0, None, None)]}
FIGURES = {"EBOOT.BIN": {"code_bytes": 0x1D0, "ledger_date": "2026-11-20", "public_scope_bytes": 0x1C0,
                         "public_scope_functions": 4, "public_t1_bytes": 0x80, "public_t1_functions": 2,
                         "public_t1_percent": 28.571, "strict_scope_bytes": 0x1D0,
                         "t2": "not measured (no whole-module build report: python tools/lbpbuild.py)",
                         "t3": "not measured"}}
PROOFS = [{"module": "EBOOT.BIN", "start": "0x00001000", "end": "0x00001040", "source": "src/eboot/a_00001000.c",
           "status": "EXACT"},
          {"module": "EBOOT.BIN", "start": "0x00001080", "end": "0x000010c0", "source": "src/eboot/b_00001080.c",
           "status": "EXACT"}]
README = ("# t\n\n<!-- lbpfigures:begin readme-glance -->\n\nx\n\n<!-- lbpfigures:end -->\n\n"
          "<!-- lbpfigures:begin readme-progress -->\n\nx\n\n<!-- lbpfigures:end -->\n")
COMPLETION = "# What 100% means\n\n<!-- lbpprogress:begin tiers -->\n\nx\n\n<!-- lbpprogress:end -->\n\nEnd.\n"


def source(start: int, end: int, symbol: str) -> str:
    return (f"/* LBPMATCH module=EBOOT.BIN start=0x{start:08x} end=0x{end:08x} symbol={symbol} lang=c99 */\n"
            "/* Hand-written reconstruction. */\nvoid f(void) {}\n")


def make_tree(root: Path, figures: dict | None = None) -> Path:
    """A complete public tree: figures, rows, boundary list, sources, ORIGINS.json, the objdiff report, the README
    blocks, progress.json, the ladder block and the milestone record, all consistent."""
    figures = json.loads(json.dumps(figures or FIGURES))
    for d in ("data", "docs", "src/eboot"):
        (root / d).mkdir(parents=True, exist_ok=True)
    (root / "data/modules.json").write_text(json.dumps(MODULES), encoding="utf-8")
    (root / "data/functions-EBOOT.BIN.txt").write_text(P.public_function_list(
        "".join(f"{s:08x} {e:08x}\n" for s, e in FUNCTIONS), "data/functions-EBOOT.BIN.txt"), encoding="utf-8")
    (root / R.FIGURES_REL).write_text(json.dumps(figures, indent=1, sort_keys=True) + "\n", encoding="utf-8")
    (root / R.ROWS_REL).write_text(R.render_rows(ROWS), encoding="utf-8")
    (root / "src/eboot/a_00001000.c").write_text(source(0x1000, 0x1040, "do_a"), encoding="utf-8")
    (root / "src/eboot/b_00001080.c").write_text(source(0x1080, 0x10C0, "FUN_00001080"), encoding="utf-8")
    (root / "ORIGINS.json").write_text(json.dumps({"src/eboot/a_00001000.c": "game",
                                                   "src/eboot/b_00001080.c": "newlib"}), encoding="utf-8")
    (root / R.REPORT_REL).write_text(R.render(R.build_report(figures, ROWS)), encoding="utf-8")
    (root / "README.md").write_text(P.fill_blocks(README, P.render_blocks(figures)), encoding="utf-8")
    (root / G.COMPLETION_REL).write_text(COMPLETION, encoding="utf-8")
    shutil.copyfile(TOOLS.parent / MS.DEFS_REL, root / MS.DEFS_REL)
    G.write_tree(root)
    MS.write_tree(root, inputs=NO_INPUTS)
    return root


def run(tool: str, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run([sys.executable, str(TOOLS / tool), *args], capture_output=True, text=True)


class LadderTests(unittest.TestCase):
    def test_tiers_are_shares_of_the_strict_scope_of_every_module(self) -> None:
        lad = G.ladder(FIGURES, MODULES)
        self.assertEqual(lad["scope_bytes"], 0x10100)                # both modules, the Sony one included
        self.assertEqual((lad["t1_bytes"], lad["t2_bytes"], lad["t3_bytes"], lad["t4_bytes"]), (0x80, 0, 0, 0))
        self.assertEqual(lad["t1_percent"], round(100 * 0x80 / 0x10100, 3))
        self.assertEqual(lad["strict_percent"], 0.0)                 # nothing is estimated
        self.assertEqual((lad["t2_measured"], lad["ledger_date"]), ([], "2026-11-20"))

    def test_t2_and_t3_come_only_from_measured_figures(self) -> None:
        figs = json.loads(json.dumps(FIGURES))
        figs["EBOOT.BIN"]["t2"] = {"public_t2_bytes": 0x80, "image_t2_bytes": 0x90, "image_identical": True}
        self.assertEqual(G.ladder(figs, MODULES)["t2_bytes"], 0x90)  # code and data the published sources produce
        figs["EBOOT.BIN"]["t3"] = {"public_t3_bytes": 0x80, "image_t3_bytes": 0x100}
        with self.assertRaises(ValueError):                         # T3 inside T2, or the figures are wrong
            G.ladder(figs, MODULES)
        figs["EBOOT.BIN"]["t3"]["image_t3_bytes"] = 0x40
        self.assertEqual(G.ladder(figs, MODULES)["t3_bytes"], 0x40)
        with self.assertRaises(ValueError):
            G.ladder({"OTHER.BIN": FIGURES["EBOOT.BIN"]}, MODULES)
        with self.assertRaises(ValueError):
            G.ladder(FIGURES, {"modules": [{"name": "EBOOT.BIN", "size": 0}]})

    def test_the_badge_is_hst_decomps_format(self) -> None:
        lad = G.ladder(FIGURES, MODULES)
        self.assertEqual(G.badge(lad), {"schemaVersion": 1, "label": "strict match", "message": "0.0% (tier 1: 0.2%)",
                                        "color": "red"})
        for strict, color in ((10, "orange"), (30, "yellow"), (60, "yellowgreen"), (90, "brightgreen")):
            self.assertEqual(G.badge({**lad, "strict_percent": strict})["color"], color)

    def test_the_ladder_block(self) -> None:
        lad = G.ladder(FIGURES, MODULES)
        text = G.render_blocks(lad)["tiers"]
        self.assertIn("| T1: function matches (published sources) | 128 B | 0.195% |", text)
        self.assertIn("| T2: whole-module build | not measured | 0.000% |", text)
        self.assertIn("| T4: strict (reviewed) | 0 B | 0.000% |", text)
        self.assertIn("Share of the strict scope (65,792 B)", text)
        filled = G.fill_blocks(COMPLETION, G.render_blocks(lad))
        self.assertTrue(filled.endswith("<!-- lbpprogress:end -->\n\nEnd.\n"))
        self.assertEqual(G.block_differences(filled, lad), [])
        self.assertEqual(G.block_differences(COMPLETION, lad), [f"{G.COMPLETION_REL}: block tiers differs from the "
                                                                "figures"])
        self.assertEqual(G.block_differences("none\n", lad), [f"{G.COMPLETION_REL}: block tiers is missing"])


class LedgerTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.tree = make_tree(Path(self.tmp.name))

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def test_a_consistent_tree_has_no_problems(self) -> None:
        self.assertEqual(G.ledger_problems(self.tree, FIGURES, ROWS), [])
        self.assertEqual(G.ledger_problems(self.tree, FIGURES, ROWS, PROOFS), [])
        self.assertEqual(G.tree_problems(self.tree, require=True), [])

    def test_figures_that_disagree_with_the_rows_are_found(self) -> None:
        figs = json.loads(json.dumps(FIGURES))
        figs["EBOOT.BIN"].update(public_t1_bytes=0x40, public_t1_percent=14.286)    # self-consistent, but not the rows
        self.assertEqual(len(G.ledger_problems(self.tree, figs, ROWS)), 1)
        figs["EBOOT.BIN"]["public_t1_percent"] = 20.0                                # and now inconsistent in itself
        self.assertIn("publish", G.ledger_problems(self.tree, figs, ROWS)[0])
        self.assertEqual(len(G.ledger_problems(self.tree, FIGURES, None)), 1)        # no rows: nothing checks them

    def test_rows_that_are_no_function_or_no_proof_are_found(self) -> None:
        rows = {"EBOOT.BIN": [(0x1000, 0x30, 0x30, "src/eboot/a_00001000.c", "do_a"), *ROWS["EBOOT.BIN"][1:]]}
        figs = json.loads(json.dumps(FIGURES))
        figs["EBOOT.BIN"].update(public_scope_bytes=0x1B0, public_t1_bytes=0x70, public_t1_percent=25.926)
        self.assertEqual(G.ledger_problems(self.tree, figs, rows),
                         ["EBOOT.BIN 0x00001000: not a function of data/functions-EBOOT.BIN.txt"])
        other = [dict(PROOFS[0], source="src/eboot/other.c"), PROOFS[1]]
        self.assertEqual(G.ledger_problems(self.tree, FIGURES, ROWS, other),
                         ["EBOOT.BIN 0x00001000: src/eboot/a_00001000.c has no accepted proof at this address"])
        short = [dict(PROOFS[0], end="0x00001020"), PROOFS[1]]
        self.assertEqual(len(G.ledger_problems(self.tree, FIGURES, ROWS, short)), 1)

    def test_check_fails_when_committed_figures_disagree_with_the_ledger(self) -> None:
        ok = run("lbpprogress.py", "--check", "--tree", str(self.tree))
        self.assertEqual(ok.returncode, 0, ok.stdout + ok.stderr)
        figs = json.loads(json.dumps(FIGURES))
        figs["EBOOT.BIN"].update(public_t1_bytes=0x40, public_t1_percent=14.286, public_t1_functions=1)
        (self.tree / R.FIGURES_REL).write_text(json.dumps(figs), encoding="utf-8")
        G.write_tree(self.tree)                     # progress.json follows the edited figures; the rows do not
        bad = run("lbpprogress.py", "--check", "--tree", str(self.tree))
        self.assertEqual(bad.returncode, 1, bad.stdout)
        self.assertIn("the ledger rows give", bad.stdout)

    def test_check_fails_on_a_stale_badge_or_block(self) -> None:
        (self.tree / G.BADGE_REL).write_text(json.dumps({**G.badge(G.tree_ladder(self.tree)), "message": "1%"}),
                                             encoding="utf-8")
        self.assertEqual(G.tree_problems(self.tree)[0].split(" differs")[0], G.BADGE_REL)
        self.assertEqual(run("lbpprogress.py", "--check", "--tree", str(self.tree)).returncode, 1)
        G.write_tree(self.tree)
        page = self.tree / G.COMPLETION_REL
        page.write_text(page.read_text(encoding="utf-8").replace("0.195%", "0.200%"), encoding="utf-8")
        self.assertEqual(G.tree_problems(self.tree), [f"{G.COMPLETION_REL}: block tiers differs from the figures"])
        self.assertEqual(run("lbpprogress.py", "--check", "--tree", str(self.tree)).returncode, 1)
        G.write_tree(self.tree)
        self.assertEqual(run("lbpprogress.py", "--check", "--tree", str(self.tree)).returncode, 0)

    def test_the_publisher_audit_refuses_a_stale_badge_and_record(self) -> None:
        self.assertEqual(P.audit(self.tree), 0)
        (self.tree / G.BADGE_REL).write_text("{}\n", encoding="utf-8")
        self.assertEqual(P.audit(self.tree), 1)
        G.write_tree(self.tree)
        self.assertEqual(P.audit(self.tree), 0)
        record = json.loads((self.tree / MS.RECORD_REL).read_text(encoding="utf-8"))
        record["milestones"][1]["progress"] = 50.0
        (self.tree / MS.RECORD_REL).write_text(json.dumps(record), encoding="utf-8")
        self.assertEqual(P.audit(self.tree), 1)

    def test_a_tree_without_the_files_is_not_audited_for_them(self) -> None:
        (self.tree / G.BADGE_REL).unlink()
        page = self.tree / G.COMPLETION_REL
        page.write_text("no block\n", encoding="utf-8")
        self.assertEqual(G.tree_problems(self.tree), [])
        self.assertEqual(G.tree_problems(self.tree, require=True), [f"{G.BADGE_REL} is missing",
                                                                    f"{G.COMPLETION_REL}: block tiers is missing"])


@unittest.skipUnless((G.ROOT / R.FIGURES_REL).is_file(), "this tree has no data/public-figures.json")
class CommittedTests(unittest.TestCase):
    def test_committed_badge_and_block_are_current(self) -> None:
        self.assertEqual(G.tree_problems(G.ROOT, require=True), [])

    def test_committed_figures_follow_from_the_ledger_rows(self) -> None:
        figures = R.load_figures(G.ROOT / R.FIGURES_REL)
        self.assertEqual(G.ledger_problems(G.ROOT, figures, R.load_rows(G.ROOT / R.ROWS_REL)), [])


if __name__ == "__main__":
    unittest.main()
