#!/usr/bin/env python3
"""Tests for the compiler-profile sweep (lbpsweep.py): seeds, text keys, cc= and note rewrites, picking, the cache
and the lane it writes. No compiler runs: the matcher is replaced."""
from __future__ import annotations

import json
import sys
import tempfile
import unittest
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpsweep as S  # noqa: E402

PRIMARY = "gcc404xs-Os"
LOCKED = ["gcc171-O2", "gcc402x-O1", "gcc404xs-O2", "gcc404xs-Os", "snc127-O2"]


def hdr(start: int, extra: str = "") -> str:
    return (f"/* LBPMATCH module=EBOOT.BIN start=0x{start:08x} end=0x{start + 16:08x} symbol=FUN_{start:08x} "
            f"lang=c99{extra} */")


def src(start: int, extra: str = "", note: str = "/* Hand-written reconstruction (lane lbp3a); near miss. */",
        body: str = "int f(void) { return 1; }", trailer: str = "") -> str:
    return f"{hdr(start, extra)}\n{note}\n{body}\n{trailer}"


def write(p: Path, text: str) -> Path:
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(text, encoding="utf-8", newline="\n")
    return p


class TextTests(unittest.TestCase):
    def test_key_ignores_cc_and_trailing_notes(self) -> None:
        a = src(0x08800000, " cc=gcc402x-O2", trailer="// near miss from lane lbp2b (unreviewed)\n")
        b = src(0x08800000, " cc=gcc171-O2")
        c = src(0x08800000)
        self.assertEqual(S.text_key(a), S.text_key(b))
        self.assertEqual(S.text_key(a), S.text_key(c))
        self.assertNotEqual(S.text_key(a), S.text_key(src(0x08800000, body="int f(void) { return 2; }")))

    def test_set_cc_replaces_or_adds_and_changes_nothing_else(self) -> None:
        a = src(0x08800000, " cc=gcc402x-O2 rodata=0x08bac160")
        out = S.set_cc(a, "gcc171-O2")
        self.assertIn(" cc=gcc171-O2 rodata=0x08bac160 */", out.split("\n")[0])
        self.assertNotIn("gcc402x", out)
        self.assertEqual(out.split("\n")[1:], a.split("\n")[1:])
        b = S.set_cc(src(0x08800000), "gcc404xs-O2")
        self.assertTrue(b.split("\n")[0].endswith("lang=c99 cc=gcc404xs-O2 */"))

    def test_trailing_comment_lines_are_stripped(self) -> None:
        t = "a\n// keep? no\nb\n// x\n\n//   y\n\n"
        self.assertEqual(S.strip_trailing_comments(t), "a\n// keep? no\nb\n")

    def test_header_of_needs_a_closed_first_line(self) -> None:
        self.assertEqual(S.header_of(src(0x08800010, " cc=gcc171-O2"))["cc"], "gcc171-O2")
        self.assertIsNone(S.header_of("/* LBPMATCH module=EBOOT.BIN start=0x1\n end=0x2 symbol=x */\n"))
        self.assertIsNone(S.header_of("int x;\n"))

    def test_origin_lane(self) -> None:
        self.assertEqual(S.origin_lane(src(1, trailer="// near miss from lane lbp2b (unreviewed)\n")), "lbp2b")
        self.assertEqual(S.origin_lane(src(1)), "lbp3a")
        self.assertEqual(S.origin_lane(src(1, note="/* x */"), "lbp9c"), "lbp9c")


class NoteTests(unittest.TestCase):
    def note(self, line: str) -> str:
        return S.rewrite_note(src(0x08800000, note=line), "gcc171-O2", "lbp21s", "lbp3a").split("\n")[1]

    def test_hand_note_loses_its_near_miss_clause(self) -> None:
        n = self.note("/* Hand-written reconstruction from the disassembly (lane lbp3a, M); near miss, 2 word(s) "
                      "differ; names and types await human review. */")
        self.assertEqual(n, "/* Hand-written reconstruction from the disassembly (lane lbp3a, M); exact match verified "
                            "under cc=gcc171-O2 after a compiler-profile sweep (lane lbp21s, tools/lbpsweep.py); names "
                            "and types await human review. */")
        for line in ("/* Hand-written reconstruction (lane lbp3a); NEAR. */",
                     "/* Hand-written reconstruction from the disassembly (lane lbp3a, M); NEAR MISS, not exact. */",
                     "/* Hand-written reconstruction from the disassembly (lane lbp3a, M); best form remains 3 words "
                     "off; names and types await human review. */"):
            n = self.note(line)
            self.assertNotRegex(n, r"(?i)near|words off|not exact")
            self.assertIn("exact match verified under cc=gcc171-O2 after a compiler-profile sweep", n)

    def test_corrected_note_keeps_its_kind_and_description(self) -> None:
        n = self.note("/* Ghidra pseudocode (lbpgen.py tag g3) corrected by hand (lane lbp3a, M): see trailing notes; "
                      "NOT exact. */")
        self.assertTrue(n.startswith("/* Ghidra pseudocode (lbpgen.py tag g3) corrected by hand (lane lbp3a, M); "
                                     "exact match verified under cc=gcc171-O2"), n)
        n = self.note("/* Hand-written reconstruction (lane lbp3a, M); exact match verified; names and types await "
                      "human review. A destructor body. */")
        self.assertEqual(n.count("exact match verified"), 1)
        self.assertTrue(n.endswith("names and types await human review. A destructor body. */"), n)

    def test_unknown_stale_note_is_replaced_and_missing_note_added(self) -> None:
        std = ("/* Hand-written reconstruction (external matching lane lbp3a); exact match verified under cc=gcc171-O2 "
               "after a compiler-profile sweep (lane lbp21s, tools/lbpsweep.py); names and types await human review. */")
        self.assertEqual(self.note("/* Hand-written candidate; current result is a mismatch. */"), std)
        t = S.rewrite_note(f"{hdr(1)}\nint x;\n", "gcc171-O2", "lbp21s", "lbp3a").split("\n")
        self.assertEqual(t[1:], [std, "int x;", ""])
        t = S.rewrite_note(f"{hdr(1)}\n/* see docs/TOOLCHAIN.md */\nint x;\n", "gcc171-O2", "lbp21s", "").split("\n")
        self.assertTrue(t[1].startswith("/* Hand-written reconstruction; exact match"))
        self.assertEqual(t[2], "/* see docs/TOOLCHAIN.md */")

    def test_installed_text(self) -> None:
        t = src(0x08800000, " cc=gcc402x-O2 cc=gcc402x-O2", trailer="// differs at +0x8\n// near miss from lane x\n")
        out = S.installed_text(t, "gcc171-O2", "lbp21s", "lbp3a")
        lines = out.split("\n")
        self.assertEqual(lines[0].count("cc="), 1)
        self.assertIn("cc=gcc171-O2", lines[0])
        self.assertNotIn("//", out)
        self.assertTrue(out.endswith("}\n"))


class SeedTests(unittest.TestCase):
    def test_discovery_filters_and_dedupes(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            tree, lane, other = root / "tree", root / "ext" / "lbp1a", root / "ext" / "nolane"
            write(tree / "wip" / "a_08800000.c", src(0x08800000, " cc=gcc402x-O2"))
            write(tree / "wip" / "sub" / "b_08800100.cpp", src(0x08800100))
            write(tree / "wip" / "covered_08800200.c", src(0x08800200))
            write(tree / "wip" / "extra_08800300.c", src(0x08800300, " extra=-O1"))
            write(tree / "wip" / "helpers_08800400.c", src(0x08800400, " helpers=g@0x08800410-0x08800420"))
            write(tree / "wip" / "badcc_08800500.c", src(0x08800500, " cc=gcc999-O9"))
            write(tree / "wip" / "notes.txt", "x")
            write(tree / "wip" / "nohdr.c", "int x;\n")
            (lane / "wip").mkdir(parents=True)
            (lane / "LANE.json").write_text("{}", encoding="utf-8")
            # the same text as a.c with another cc= and a harvest trailer: a duplicate
            write(lane / "wip" / "a_08800000.c", src(0x08800000, " cc=gcc171-O2", trailer="// near miss\n"))
            write(lane / "wip" / "c_08800600.c", src(0x08800600, " cc=gcc171-O2"))
            write(lane / "wip" / "deep" / "d_08800700.c", src(0x08800700))   # lanes: top level only
            write(other / "wip" / "e_08800800.c", src(0x08800800))
            seeds, skipped = S.discover_seeds(tree, [lane], {("EBOOT.BIN", 0x08800200)}, PRIMARY)
            self.assertEqual([s["name"] for s in seeds], ["a_08800000.c", "b_08800100.cpp", "c_08800600.c"])
            self.assertEqual([s["cc"] for s in seeds], ["gcc402x-O2", PRIMARY, "gcc171-O2"])
            self.assertEqual([s["lane"] for s in seeds], ["", "", "lbp1a"])
            self.assertEqual(skipped, Counter(covered=1, extra_or_helpers=2, unknown_cc=1, duplicate=1, bad_header=1))

    def test_moved_header_is_read_as_wip_text(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            tree = Path(d)
            write(tree / "wip" / "m.c", f"/* note */\n{hdr(0x08800000)}\nint x;\n{hdr(0x08800000)}\n")
            seeds, _ = S.discover_seeds(tree, [], set(), PRIMARY)
            self.assertTrue(seeds[0]["text"].startswith("/* LBPMATCH "))
            self.assertEqual(seeds[0]["text"].count("LBPMATCH"), 1)


def seed(start: int, cc: str = PRIMARY, name: str | None = None, key: str | None = None) -> dict:
    text = src(start, f" cc={cc}")
    return {"path": Path(f"/x/{name or start}.c"), "name": name or f"f_{start:08x}.c", "lane": "", "text": text,
            "key": key or S.text_key(text), "cc": cc, "module": "EBOOT.BIN", "start": start, "end": start + 16,
            "symbol": f"FUN_{start:08x}", "origin": "lbp3a"}


class PickAndCacheTests(unittest.TestCase):
    def test_profiles_and_ranks(self) -> None:
        manifest = {"core": {"profiles": {p: {} for p in LOCKED}, "primary": PRIMARY}, "core_digest": "d1"}
        locked, primary, digest = S.lock_profiles(manifest)
        self.assertEqual((locked, primary, digest), (LOCKED, PRIMARY, "d1"))
        gcc, unl = S.sweep_profiles(locked, False)
        self.assertEqual(gcc, LOCKED[:4])
        self.assertEqual(unl, [])
        _, unl = S.sweep_profiles(locked, True)
        self.assertIn("gcc410-O2", unl)
        self.assertFalse(set(unl) & set(LOCKED))
        order = sorted(["gcc402x-O1", PRIMARY, "gcc171-O2", "gcc404xs-O2"],
                       key=lambda p: S.rank("gcc404xs-O2", p, PRIMARY, LOCKED))
        self.assertEqual(order, ["gcc404xs-O2", PRIMARY, "gcc171-O2", "gcc402x-O1"])

    def test_pick_and_leads(self) -> None:
        a, a2, b, c = seed(0x100, "gcc402x-O1"), seed(0x100, PRIMARY, "a2.c", "k2"), seed(0x200), seed(0x300)
        ex, mm = {"status": "EXACT"}, {"status": "MISMATCH"}
        results = {a["key"]: {"gcc402x-O1": mm, "gcc171-O2": ex, PRIMARY: ex},
                   "k2": {PRIMARY: ex},
                   b["key"]: {PRIMARY: mm, "gcc171-O2": ex, "gcc402x-O1": ex},
                   c["key"]: {PRIMARY: mm, "gcc410-O2": ex}}
        picked = S.pick([a, a2, b, c], results, LOCKED, PRIMARY)
        self.assertEqual(sorted((k[1], s["name"], p) for k, (s, p) in picked.items()),
                         [(0x100, "a2.c", PRIMARY),     # EXACT under its own cc= beats a's primary
                          (0x200, b["name"], "gcc171-O2")])
        self.assertEqual([(r["address"], r["profile"]) for r in S.leads([a, a2, b, c], results, LOCKED, picked)],
                         [("0x00000300", "gcc410-O2")])

    def test_cache_key_chunks_linked(self) -> None:
        self.assertNotEqual(S.cache_key("k", "p", "d1"), S.cache_key("k", "p", "d2"))
        self.assertEqual([len(c) for c in S.chunks(list(range(85)))], [40, 40, 5])
        self.assertTrue(S.linked("MISMATCH") and S.linked("EXACT") and S.linked("SECTION_MISMATCH"))
        self.assertFalse(S.linked("COMPILE_FAILED") or S.linked("LINK_FAILED") or S.linked("MISSING_SYMBOLS"))
        self.assertFalse(S.cacheable({"status": "COMPILE_CRASHED"}))
        self.assertTrue(S.cacheable({"status": "COMPILE_FAILED"}))
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "c.json"
            S.save_cache(p, {"x": {"status": "EXACT"}})
            self.assertEqual(S.load_cache(p), {"x": {"status": "EXACT"}})
            p.write_text("not json", encoding="utf-8")
            self.assertEqual(S.load_cache(p), {})


class SweepTests(unittest.TestCase):
    def test_baseline_then_locked_for_linked_seeds_and_cache_on_rerun(self) -> None:
        good, broken = seed(0x100, "gcc402x-O1"), seed(0x200)
        calls: list[list[Path]] = []

        def runner(files, build_root, parallel, out):
            calls.append(files)
            rows = {}
            for f in files:
                prof = f.parent.parent.name
                self.assertIn(f"cc={prof} ", f.read_text(encoding="utf-8").split("\n")[0])
                status = ("COMPILE_FAILED" if "00000200" in f.read_text(encoding="utf-8").split("\n")[0]
                          else "EXACT" if prof == "gcc171-O2" else "MISMATCH")
                rows[f.resolve()] = {"status": status, "words_differ": 0 if status == "EXACT" else None}
            return rows

        gcc = LOCKED[:4]
        with tempfile.TemporaryDirectory() as d:
            sw = Path(d)
            cache: dict = {}
            res, stats = S.sweep([good, broken], gcc, [], "d1", cache, 3, runner, sw, sw / "cache.json")
            self.assertEqual(stats["pairs_compiled"], 2 + 3)       # two baselines, three other profiles for `good`
            self.assertEqual(sorted(res[good["key"]]), sorted(gcc))
            self.assertEqual(list(res[broken["key"]]), [PRIMARY])
            self.assertEqual(res[good["key"]]["gcc171-O2"]["status"], "EXACT")
            calls.clear()
            res2, stats2 = S.sweep([good, broken], gcc, [], "d1", S.load_cache(sw / "cache.json"), 3, runner, sw)
            self.assertEqual((stats2["pairs_compiled"], stats2["cache_hits"], calls), (0, 5, []))
            self.assertEqual(res2, res)
            _, stats3 = S.sweep([good], gcc, [], "d2", S.load_cache(sw / "cache.json"), 3, runner, sw)
            self.assertEqual(stats3["pairs_compiled"], 4)          # a new lock digest measures again


class LaneTests(unittest.TestCase):
    def test_install_lane(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            main, lane = root / "main", root / "ext" / "lbp21s"
            write(main / "src" / "eboot" / "taken.c", "x")
            old = write(root / "ext" / "lbp3a" / "try" / "agent-queue" / "lbp3a-01.json",
                        json.dumps({"items": [{"address": "0x00000100", "end": "0x00000110", "pseudocode": "p"}]}))
            a, b, t = seed(0x100, "gcc402x-O1"), seed(0x200), seed(0x300, name="taken.c")
            a["text"] = src(0x100, " cc=gcc402x-O1", trailer="// near miss from lane lbp3a (unreviewed)\n")
            r = S.install_lane("lbp21s", [(b, PRIMARY), (a, "gcc171-O2"), (t, PRIMARY)], lane, main, [old], "abc",
                               "2026-11-09T00:00:00Z")
            self.assertEqual(r["installed"], 2)
            self.assertEqual(len(r["name_taken"]), 1)
            self.assertEqual(r["harvest"], "python tools/lbplane.py harvest lbp21s")
            text = (lane / "src" / "eboot" / a["name"]).read_text(encoding="utf-8")
            self.assertIn("cc=gcc171-O2 */", text.split("\n")[0])
            self.assertIn("exact match verified under cc=gcc171-O2 after a compiler-profile sweep", text.split("\n")[1])
            self.assertNotIn("//", text)
            info = json.loads((lane / "LANE.json").read_text(encoding="utf-8"))
            self.assertEqual((info["kind"], info["batch"], info["items"], info["main_head"]),
                             ("sweep", "lbp21s-01.json", 2, "abc"))
            for q in (main, lane):
                doc = json.loads((q / "try" / "agent-queue" / "lbp21s-01.json").read_text(encoding="utf-8"))
                self.assertEqual(doc["kind"], "sweep")
                self.assertEqual([it["address"] for it in doc["items"]], ["0x00000100", "0x00000200"])
                self.assertEqual(doc["items"][0]["pseudocode"], "p")          # copied from the earlier batch
                self.assertEqual(doc["items"][1]["cc_hint"], PRIMARY)          # made from the header
            self.assertEqual((lane / "PROGRESS.md").read_text(encoding="utf-8").splitlines(),
                             [f"0x00000100 EXACT src/eboot/{a['name']}", f"0x00000200 EXACT src/eboot/{b['name']}"])
            with self.assertRaises(SystemExit):
                S.install_lane("lbp21s", [], lane, main, [], "", "")


if __name__ == "__main__":
    unittest.main()
