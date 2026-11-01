#!/usr/bin/env python3
"""Tests for the origin classes (lbporigin.py) and the public-tree builder (lbppublish.py). No WSL, no retail bytes."""
from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbporigin as O  # noqa: E402
import lbppublish as P  # noqa: E402
import lbpreport as R  # noqa: E402

REGIONS = {"EBOOT.BIN": [(0x1000, 0x1100, "startup"), (0x1100, 0x5000, "game"), (0x5000, 0x5100, "unattributed"),
                         (0x5100, 0x9000, "sony-sdk"), (0x7000, 0x8000, "libstdc++")]}


def src(start: int, note: str = "/* Hand-written reconstruction from the disassembly. */", lang: str = "c99") -> str:
    return (f"/* LBPMATCH module=EBOOT.BIN start=0x{start:08x} end=0x{start + 0x10:08x} symbol=FUN_{start:08x} "
            f"lang={lang} */\n{note}\nint FUN_{start:08x};\n")


class OriginTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "src" / "eboot").mkdir(parents=True)
        (self.root / "wip").mkdir()
        (self.root / "data").mkdir()

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def put(self, name: str, text: str, tree: str = "src/eboot") -> str:
        (self.root / tree / name).write_text(text, encoding="utf-8")
        return f"{tree}/{name}"

    def test_the_innermost_region_wins(self) -> None:
        self.assertEqual(O.region_class(REGIONS, "EBOOT.BIN", 0x7100), "libstdc++")
        self.assertEqual(O.region_class(REGIONS, "EBOOT.BIN", 0x6000), "sony-sdk")
        self.assertEqual(O.region_class(REGIONS, "EBOOT.BIN", 0x1200), "game")
        self.assertEqual(O.region_class(REGIONS, "EBOOT.BIN", 0x9000), "unattributed")
        self.assertEqual(O.region_class(REGIONS, "OTHER.PRX", 0x1200), "unattributed")

    def test_classes_from_notes_regions_and_clones(self) -> None:
        g = self.put("g_00001200.c", src(0x1200))
        n = self.put("n_00006000.c", src(0x6000, "/* Library source: newlib 1.14.0 libc/string/memset.c function memset */"))
        z = self.put("z_00006100.c", src(0x6100, "/* zlib 1.2.3 (zlib.net, zlib-1.2.3.tar.gz md5 x), inflate.c */"))
        s = self.put("s_00006200.c", src(0x6200))
        u = self.put("u_00005010.c", src(0x5010))
        # a clone of game code inside a library is library code; a clone of a newlib source is newlib
        c1 = self.put("c_00006300.c", src(0x6300, f"/* Clone of {g} (lbpclone.py tag c1); exact match verified. */"))
        c2 = self.put("c_00001300.c", src(0x1300, f"/* Clone of {n} (lbpclone.py tag c1); exact match verified. */"))
        c3 = self.put("c_00001400.c", src(0x1400, f"/* Clone of {g} (lbpclone.py tag c1); exact match verified. */"))
        # cross-project clones carry their hst-decomp exemplar's evidence: newlib there -> newlib, Sony SDK there ->
        # sony-sdk even in the game region (owner decision 2026-10-28), and so do their own clones
        x1 = self.put("x_00006400.c", src(0x6400, "/* Clone of hst-decomp src/eboot/a.c (lbpxclone.py tag x1) */"))
        x2 = self.put("x_00001500.c", src(0x1500, "/* Clone of hst-decomp src/libfont/b.c (lbpxclone.py tag x1) */"))
        c4 = self.put("c_00001600.c", src(0x1600, f"/* Clone of src/eboot/x_00001500.c (lbpclone.py tag c1) */"))
        w = self.put("w_00006500.c", src(0x6500), tree="wip")
        classes, warnings = O.classify(self.root, REGIONS, {"src/eboot/a.c": "newlib", "src/libfont/b.c": "sony-prx"})
        self.assertEqual(warnings, [])
        self.assertEqual({k: classes[k] for k in (g, n, z, s, u, c1, c2, c3, x1, x2, c4, w)},
                         {g: "game", n: "newlib", z: "zlib", s: "sony-sdk", u: "unattributed", c1: "sony-sdk",
                          c2: "newlib", c3: "game", x1: "newlib", x2: "sony-sdk", c4: "sony-sdk", w: "sony-sdk"})

    def test_a_sony_prx_module_is_sony_prx(self) -> None:
        (self.root / "data" / "modules.json").write_text(json.dumps({"modules": [
            {"name": "EBOOT.BIN", "kind": "game"}, {"name": "x.prx", "kind": "sony-prx"}]}), encoding="utf-8")
        p = self.put("p_00000100.c", src(0x100).replace("EBOOT.BIN", "x.prx"))
        self.assertEqual(O.classify(self.root, REGIONS, {})[0][p], "sony-prx")

    def test_the_project_regions_file_loads_and_is_ordered(self) -> None:
        regions = O.load_regions()
        self.assertIn("EBOOT.BIN", regions)
        for s, e, c in regions["EBOOT.BIN"]:
            self.assertLess(s, e)
            self.assertIn(c, O.CLASSES)


class PublishTests(unittest.TestCase):
    def test_plan_excludes_by_reason(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            tracked = ["src/eboot/g.c", "src/eboot/s.c", "src/eboot/n.c", "src/eboot/x.c", "symbols/EBOOT.BIN.txt",
                       "data/functions-EBOOT.BIN.txt", "docs/HANDOFF.md", "tools/lbplane.py", "docs/GUIDE.md",
                       "secret.json", "gone.txt"]
            for t in tracked[:-1]:
                (root / t).parent.mkdir(parents=True, exist_ok=True)
                (root / t).write_text("x", encoding="utf-8")
            classes = {"src/eboot/g.c": "game", "src/eboot/s.c": "sony-sdk", "src/eboot/n.c": "newlib"}
            keep, excluded = P.plan(tracked, {"secret.json"}, classes, False, root)
            self.assertEqual(keep, ["src/eboot/g.c", "src/eboot/n.c", "docs/GUIDE.md"])
            self.assertEqual(excluded["origin class sony-sdk"], ["src/eboot/s.c"])
            self.assertEqual(excluded["source without an origin class"], ["src/eboot/x.c"])
            self.assertEqual(excluded["gitignored (tracked by force)"], ["secret.json"])
            self.assertEqual(excluded["missing in the work tree"], ["gone.txt"])
            self.assertIn("symbols/EBOOT.BIN.txt", excluded["symbol map (PUBLISH_SYMBOL_MAP is off)"])
            self.assertEqual(sorted(excluded["maintainer-only path (PRIVATE_PATHS)"]),
                             ["docs/HANDOFF.md", "tools/lbplane.py"])
            keep2, _ = P.plan(tracked, set(), classes, root=root)   # the policy default publishes the symbol map
            self.assertIn("symbols/EBOOT.BIN.txt", keep2)

    def test_public_figures_leave_sony_code_out_of_the_denominator(self) -> None:
        inv = {"EBOOT.BIN": {0x1100: 0x1200, 0x1200: 0x1300, 0x6000: 0x6040, 0x6100: 0x6140, 0x7000: 0x7020}}
        proofs = [{"module": "EBOOT.BIN", "start": "0x00001100", "end": "0x00001200", "source": "src/g.c",
                   "status": "EXACT"},
                  {"module": "EBOOT.BIN", "start": "0x00006000", "end": "0x00006040", "source": "src/n.c",
                   "status": "EXACT"},
                  {"module": "EBOOT.BIN", "start": "0x00006100", "end": "0x00006140", "source": "src/s.c",
                   "status": "EXACT"}]
        classes = {"src/g.c": "game", "src/n.c": "newlib", "src/s.c": "sony-sdk"}
        f = P.figures({"src/g.c", "src/n.c"}, classes, REGIONS, inv, proofs)["EBOOT.BIN"]
        # scope: two game functions (0x200), the libstdc++ one (0x20) and the published newlib one (0x40)
        self.assertEqual((f["public_scope_bytes"], f["public_t1_bytes"]), (0x260, 0x140))
        # function counts follow the same rules: four in scope, two with an accepted proof of a published source
        self.assertEqual((f["public_scope_functions"], f["public_t1_functions"]), (4, 2))
        # a held-back Sony source in the game region leaves the public scope
        proofs.append({"module": "EBOOT.BIN", "start": "0x00001200", "end": "0x00001300", "source": "src/h.c",
                       "status": "EXACT"})
        classes["src/h.c"] = "sony-sdk"
        f = P.figures({"src/g.c", "src/n.c"}, classes, REGIONS, inv, proofs)["EBOOT.BIN"]
        self.assertEqual((f["public_scope_bytes"], f["public_t1_bytes"]), (0x160, 0x140))
        self.assertEqual((f["public_scope_functions"], f["public_t1_functions"]), (3, 2))
        self.assertEqual(f["strict_scope_bytes"], 0x2a0)
        self.assertTrue(f["t2"].startswith("not measured"))

    def test_public_t2_counts_published_source_code_in_scope_from_an_identical_build(self) -> None:
        inv = {"EBOOT.BIN": {0x1100: 0x1200, 0x1200: 0x1300, 0x6000: 0x6040, 0x6100: 0x6140}}
        proofs = [{"module": "EBOOT.BIN", "start": "0x00001100", "end": "0x00001200", "source": "src/g.c",
                   "status": "EXACT"},
                  {"module": "EBOOT.BIN", "start": "0x00006100", "end": "0x00006140", "source": "src/s.c",
                   "status": "EXACT"}]
        classes = {"src/g.c": "game", "src/s.c": "sony-sdk"}
        report = {"image_bytes": 0x10000,
                  "t2_source_bytes": {"src/g.c": 0x108, "src/s.c": 0x40},
                  "t2_source_ranges": {"src/g.c": [["0x00001100", "0x00001200", "source-code"],
                                                   ["0x00008000", "0x00008008", "source-data"]],
                                       "src/s.c": [["0x00006100", "0x00006140", "source-code"]]}}
        f = P.figures({"src/g.c"}, classes, REGIONS, inv, proofs, {"EBOOT.BIN": report})["EBOOT.BIN"]
        t2 = f["t2"]
        self.assertEqual(f["public_scope_bytes"], 0x200)            # the two game functions
        self.assertEqual((f["public_scope_functions"], f["public_t1_functions"]), (2, 1))
        self.assertEqual((t2["public_t2_bytes"], t2["public_t2_data_bytes"], t2["image_t2_bytes"]), (0x100, 8, 0x108))
        self.assertEqual(t2["public_t2_percent"], 50.0)
        self.assertEqual(f["t3"], "not measured")
        why = "not measured (the build report is stale: the accepted ledger changed since the build)"
        self.assertEqual(P.figures({"src/g.c"}, classes, REGIONS, inv, proofs, {"EBOOT.BIN": why})["EBOOT.BIN"]["t2"], why)

    def test_overlap_bytes(self) -> None:
        scope = [(0x10, 0x20), (0x30, 0x40)]
        self.assertEqual(P.overlap_bytes([(0x0, 0x50)], scope), 0x20)
        self.assertEqual(P.overlap_bytes([(0x18, 0x34), (0x3c, 0x3e)], scope), 8 + 4 + 2)
        self.assertEqual(P.overlap_bytes([(0x20, 0x30)], scope), 0)

    def test_audit_refuses_private_paths_keys_and_unlisted_sources(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tree = Path(tmp)
            (tree / "src" / "eboot").mkdir(parents=True)
            (tree / "docs").mkdir()
            (tree / "src" / "eboot" / "a.c").write_text(src(0x1200), encoding="utf-8")
            (tree / "src" / "eboot" / "b.c").write_text(src(0x1300), encoding="utf-8")
            (tree / "ORIGINS.json").write_text(json.dumps({"src/eboot/a.c": "game", "src/eboot/b.c": "sony-sdk"}),
                                               encoding="utf-8")
            (tree / "README.md").write_text("ok\n", encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)    # b.c has an excluded class
            (tree / "ORIGINS.json").write_text(json.dumps({"src/eboot/a.c": "game", "src/eboot/b.c": "game"}),
                                               encoding="utf-8")
            self.assertEqual(P.audit(tree), 0)
            (tree / "docs" / "x.md").write_text("see C:\\nk\\private\\lbp-decomp\n", encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)
            (tree / "docs" / "x.md").write_text("https://bytematch.dev/Jstar269/lbp-psp-decomp\n"
                                                "github.com/Jstar269/lbp-psp-decomp\n"
                                                '"name": "Jstar269"\n', encoding="utf-8")
            self.assertEqual(P.audit(tree), 0)    # the owner's public project URLs and handle are allowed
            (tree / "docs" / "x.md").write_text("written by Jstar269\n", encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)    # a bare user name is not
            (tree / "docs" / "x.md").write_text("api_key = abcdefghijklmnop\n", encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)
            (tree / "docs" / "x.md").write_text("sha256 0123456789abcdef0123456789abcdef\n", encoding="utf-8")
            self.assertEqual(P.audit(tree), 0)    # a digest is not a key

    def test_the_report_is_made_from_the_figures_and_the_audit_checks_it(self) -> None:
        inv = {"EBOOT.BIN": {0x1100: 0x1200, 0x1200: 0x1300, 0x6000: 0x6040}}
        proofs = [{"module": "EBOOT.BIN", "start": "0x00001100", "end": "0x00001200", "source": "src/g.c",
                   "status": "EXACT"}]
        figs = P.figures({"src/g.c"}, {"src/g.c": "game"}, REGIONS, inv, proofs)
        report = R.build_report(figs)
        m = report["measures"]
        self.assertEqual((m["total_functions"], m["matched_functions"]), (2, 1))
        self.assertEqual((m["total_code"], m["matched_code"]), ("512", "256"))
        with tempfile.TemporaryDirectory() as tmp:
            tree = Path(tmp)
            (tree / "data").mkdir()
            (tree / "docs").mkdir()
            (tree / "README.md").write_text("ok\n", encoding="utf-8")
            (tree / "ORIGINS.json").write_text("{}", encoding="utf-8")
            (tree / P.FIGURES).write_text(json.dumps(figs), encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)                        # the report is missing
            (tree / R.REPORT_REL).write_text(R.render(report), encoding="utf-8")
            self.assertEqual(P.audit(tree), 0)
            stale = json.loads(json.dumps(report))
            stale["measures"]["matched_functions"] = 2
            (tree / R.REPORT_REL).write_text(R.render(stale), encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)                        # the report disagrees with the figures

    def test_readme_blocks_are_made_from_the_figures_and_the_audit_checks_them(self) -> None:
        inv = {"EBOOT.BIN": {0x1100: 0x1200, 0x1200: 0x1300, 0x6000: 0x6040}}
        proofs = [{"module": "EBOOT.BIN", "start": "0x00001100", "end": "0x00001200", "source": "src/g.c",
                   "status": "EXACT", "run": "20261101T082408Z-1"},
                  {"module": "EBOOT.BIN", "start": "0x00001200", "end": "0x00001300", "source": "src/s.c",
                   "status": "EXACT", "run": "20261102T000000Z-2"}]          # held back: its date does not count
        figs = P.figures({"src/g.c"}, {"src/g.c": "game", "src/s.c": "sony-sdk"}, REGIONS, inv, proofs)
        self.assertEqual(figs["EBOOT.BIN"]["ledger_date"], "2026-11-01")
        b = P.render_blocks(figs)
        self.assertIn("| **Code matched** (T1) | **100.0%** | 256 of the 256 code bytes", b["readme-glance"])
        self.assertIn("| **Functions matched** (T1) | **100.0%** | 1 of the 1 functions", b["readme-glance"])
        self.assertIn("| **Built from source** (T2) | not measured |", b["readme-glance"])
        self.assertIn("Measured from the accepted proofs of 2026-11-01 with `python tools/lbppublish.py`.",
                      b["readme-glance"])
        self.assertIn("| `EBOOT.BIN` | 1 / 1 | 256 / 256 | 100.000 % |", b["readme-progress"])
        page = "# T\n\n<!-- lbpfigures:begin readme-glance -->\n\nold\n\n<!-- lbpfigures:end -->\n\nAfter.\n"
        filled = P.fill_blocks(page, b)
        self.assertTrue(filled.endswith("<!-- lbpfigures:end -->\n\nAfter.\n"))
        self.assertEqual(P.block_differences(filled, figs), ["README.md: block readme-progress is missing"])
        filled += "\n<!-- lbpfigures:begin readme-progress -->\n\nx\n\n<!-- lbpfigures:end -->\n"
        self.assertEqual(P.block_differences(P.fill_blocks(filled, b), figs), [])
        self.assertEqual(P.block_differences(filled.replace("100.0%", "99.0%"), figs),
                         ["README.md: block readme-glance differs from the figures",
                          "README.md: block readme-progress differs from the figures"])
        self.assertEqual(P.block_differences("no blocks\n", figs), [])      # a README without blocks is not checked
        with tempfile.TemporaryDirectory() as tmp:
            tree = Path(tmp)
            (tree / "data").mkdir()
            (tree / "docs").mkdir()
            (tree / "ORIGINS.json").write_text("{}", encoding="utf-8")
            (tree / P.FIGURES).write_text(json.dumps(figs), encoding="utf-8")
            (tree / R.REPORT_REL).write_text(R.render(R.build_report(figs)), encoding="utf-8")
            (tree / "README.md").write_text(P.fill_blocks(filled, b), encoding="utf-8")
            self.assertEqual(P.audit(tree), 0)
            (tree / "README.md").write_text(filled, encoding="utf-8")
            self.assertEqual(P.audit(tree), 1)                        # the README disagrees with the figures

    def test_the_report_files_are_published(self) -> None:
        tracked = ["tools/lbpreport.py", "tools/test_lbpreport.py", ".github/workflows/progress-report.yml",
                   ".recomp.json", "docs/objdiff-report.json"]
        root = Path(__file__).resolve().parent.parent
        keep, excluded = P.plan(tracked, set(), {}, root=root)
        self.assertEqual([p for p in tracked if p not in keep and (root / p).is_file()], [], excluded)
        for p in tracked:
            self.assertFalse(P.matches(p, P.PRIVATE_PATHS) or P.matches(p, P.DISC_DERIVED), p)

    def test_public_check_refuses_unlisted_or_excluded_sources(self) -> None:
        import lbppublic
        self.assertEqual(lbppublic.check_tree(["src/eboot/a.c", "docs/x.md"], {"src/eboot/a.c": "game"}), [])
        self.assertEqual(len(lbppublic.check_tree(["src/eboot/a.c", "src/eboot/b.c"],
                                                  {"src/eboot/a.c": "game", "src/eboot/b.c": "sony-sdk"})), 2)
        self.assertEqual(len(lbppublic.check_tree(["wip/c.cpp"], {})), 1)

    def test_the_tool_runs_git_ls_files_only(self) -> None:
        with self.assertRaises(AssertionError):
            P.git_lines("push")


if __name__ == "__main__":
    unittest.main()
