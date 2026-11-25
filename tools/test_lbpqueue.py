#!/usr/bin/env python3
"""Tests for the batch builder (lbpqueue.py): retail-word flags, candidate selection, reservations."""
from __future__ import annotations

import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from functools import partial
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

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


class StarterTests(unittest.TestCase):
    """Starter functions remain reserved until their entry is removed."""

    HEADER = "/* LBPMATCH module=EBOOT.BIN start=0x{0:08x} end=0x{1:08x} symbol=FUN_{0:08x} lang=c99 */\nint x;\n"

    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "data").mkdir()

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def write(self, entries: list, schema: str = Q.STARTER_SCHEMA) -> None:
        (self.root / "data" / "starter-issues.json").write_text(json.dumps({"schema": schema, "entries": entries}),
                                                                 encoding="utf-8")

    def test_rejects_malformed_entries_duplicates_and_wrong_schema(self) -> None:
        self.assertEqual(Q.load_starters(self.root), {})                  # no file: no starters
        entry = {"module": "EBOOT.BIN", "start": "0x0880b604", "issue": 2}
        self.write([entry])
        self.assertEqual(list(Q.load_starters(self.root)), [("EBOOT.BIN", 0x0880B604)])
        self.assertEqual(Q.starter_starts("EBOOT.BIN", self.root), {0x0880B604})
        self.assertEqual(Q.starter_starts("other.prx", self.root), set())
        for bad in ([entry, entry], [dict(entry, bytes="00")], [dict(entry, issue="2")], [dict(entry, issue=True)],
                    [dict(entry, issue=0)], [dict(entry, start="0x880b604")], [dict(entry, module="")],
                    [dict(entry, notes=1)]):
            self.write(bad)
            with self.assertRaises(ValueError, msg=repr(bad)):
                Q.load_starters(self.root)
        self.write([entry], schema="other")
        with self.assertRaises(ValueError):
            Q.load_starters(self.root)

    def test_a_starter_and_its_clone_family_are_never_queued(self) -> None:
        class Args:
            flag, exclude, min_size, max_size, include_snc = None, [], 0, 4096, False

        funcs = {s: {"start": s, "end": s + 48, "size": 48,
                     "flags": {"ins_zero": 0, "vfpu": 0, "unaligned": 0, "div": 0}} for s in (0x1000, 0x2000, 0x3000, 0x4000)}
        ctx = {"funcs": funcs, "family_of": {0x3000: [0x3000, 0x4000], 0x4000: [0x3000, 0x4000]}, "have": set(),
               "reserved": {0x2000, 0x4000}, "attempted": set(), "gen": {}, "snc": set(), "lib": {},
               "wip": {("EBOOT.BIN", s): ["wip/x.c"] for s in funcs}, "names": {}}
        self.assertEqual(Q.rank(ctx, "EBOOT.BIN", "near", Args()), [0x1000])
        self.assertEqual(Q.rank(ctx, "EBOOT.BIN", "families", Args()), [])

    def test_the_check_flags_a_starter_that_a_source_now_covers(self) -> None:
        (self.root / "src" / "eboot").mkdir(parents=True)
        (self.root / "wip").mkdir()
        (self.root / "src" / "eboot" / "a_08840f44.c").write_text(self.HEADER.format(0x08840F44, 0x08840F64), encoding="utf-8")
        (self.root / "wip" / "b_0880b604.c").write_text(self.HEADER.format(0x0880B604, 0x0880B634), encoding="utf-8")
        self.write([{"module": "EBOOT.BIN", "start": "0x0880b604", "issue": 2}])
        self.assertEqual(Q.cmd_starters(self.root), 0)
        self.write([{"module": "EBOOT.BIN", "start": "0x0880b604", "issue": 2},
                    {"module": "EBOOT.BIN", "start": "0x08840f44", "issue": 2}])
        self.assertEqual(Q.cmd_starters(self.root), 1)

    def test_the_check_flags_a_starter_that_a_source_places_as_a_helper(self) -> None:
        (self.root / "src" / "eboot").mkdir(parents=True)
        (self.root / "src" / "eboot" / "a_08840f44.c").write_text(
            "/* LBPMATCH module=EBOOT.BIN start=0x08840f44 end=0x08840f64 symbol=FUN_08840f44 lang=c99 "
            "helpers=FUN_0880b604@0x0880b604-0x0880b634 */\nint x;\n", encoding="utf-8")
        self.write([{"module": "EBOOT.BIN", "start": "0x08875f1c", "issue": 2}])
        self.assertEqual(Q.cmd_starters(self.root), 0)
        self.write([{"module": "EBOOT.BIN", "start": "0x0880b604", "issue": 2}])
        self.assertEqual(Q.cmd_starters(self.root), 1)

    def test_the_committed_list_is_well_formed_and_names_only_open_functions(self) -> None:
        starters = Q.load_starters()
        self.assertEqual(Q.cmd_starters(), 0, "a starter function is matched: remove its entry and update the issue")
        wip = Q.wip_leads(Q.M.ROOT)
        for key, ent in starters.items():
            self.assertTrue(wip.get(key), f"#{ent['issue']} 0x{key[1]:08x} has no wip/ file for the issue to point at")

    def test_nearperm_reserves_starters_and_clone_siblings_at_seed_selection(self) -> None:
        import lbpnearperm as nearperm

        class BeforeMatching(Exception):
            pass

        starter, sibling, unrelated = 0x1000, 0x1020, 0x1040
        addresses = (starter, sibling, unrelated)
        entries = [{"module": "EBOOT.BIN", "start": f"0x{starter:08x}", "issue": 2}]
        wip = self.root / "wip"
        wip.mkdir()
        files = {start: wip / f"synthetic_{start:08x}.c" for start in addresses}
        report = self.root / "near-report.json"
        report.write_text(json.dumps({"files": [
            {"wip": path.relative_to(self.root).as_posix(), "class": "unknown",
             "yield": 1, "lever": "permute", "words_left": 1, "words": 8}
            for path in files.values()
        ]}), encoding="utf-8")
        for start, path in files.items():
            path.write_text(self.HEADER.format(start, start + 32), encoding="utf-8")
        ledger = self.root / "ledger"
        ledger.mkdir()
        (ledger / "campaign.json").write_text(json.dumps({"modules": [{
            "name": "EBOOT.BIN",
            "functions": [{"start": s, "end": s + 32, "boundary_status": "auto"} for s in addresses],
        }]}), encoding="utf-8")
        # Two identical synthetic instruction fixtures and one unrelated shape.
        data = bytes(64) + b"\x01\x00\x02\x24" * 8
        lane = SimpleNamespace(covered=lambda _root: set(), wip_text=lambda p: p.read_text(encoding="utf-8"))

        def seeds(*chosen: int, module_reads: int = 1) -> list[int]:
            seen = []

            def before_matching(_root, _parallel, ends):
                seen.extend(addr for _module, addr in ends)
                raise BeforeMatching

            args = ["lbpnearperm.py", "--tag", "synthetic", "--report", str(report), *(str(files[s]) for s in chosen)]
            with patch.object(sys, "argv", args), patch.dict(sys.modules, {"lbplane": lane}), \
                    patch.object(nearperm, "ROOT", self.root), patch.object(nearperm.N, "ROOT", self.root), \
                    patch.object(Q, "QUEUE", self.root / "queue"), patch.object(Q, "lane_dirs", return_value=[]), \
                    patch.object(Q.M, "campaign_root", return_value=self.root), \
                    patch.object(Q.M, "module_bytes", return_value=(data, {"start": starter, "file_offset": 0})) as read_module, \
                    patch.object(nearperm.P, "make_runner", side_effect=before_matching), \
                    redirect_stdout(io.StringIO()), self.assertRaises(BeforeMatching):
                nearperm.main()
            self.assertEqual(read_module.call_count, module_reads)
            return sorted(seen)

        self.write(entries)
        for chosen, expected in (((), [unrelated]), ((starter,), []), ((sibling,), []), ((unrelated,), [unrelated])):
            with self.subTest(chosen=chosen):
                self.assertEqual(seeds(*chosen, module_reads=0 if chosen == (starter,) else 1), expected)
        self.write([])
        self.assertEqual(seeds(module_reads=0), list(addresses))

    def test_clone_propagation_never_targets_a_starter(self) -> None:
        import lbpclone as C

        exemplar, starter, other = 0x1000, 0x1020, 0x1040
        (self.root / "src" / "eboot").mkdir(parents=True)
        (self.root / "src" / "eboot" / "a_00001000.c").write_text(self.HEADER.format(exemplar, exemplar + 32),
                                                                   encoding="utf-8")
        ledger = self.root / "ledger"
        ledger.mkdir()
        (ledger / "campaign.json").write_text(json.dumps({"modules": [{"name": "EBOOT.BIN", "functions": [
            {"start": s, "end": s + 32, "boundary_status": "auto"} for s in (exemplar, starter, other)]}]}),
            encoding="utf-8")
        data = b"\x01\x00\x02\x24" * 24       # three functions of the same synthetic shape

        def targets() -> list[int]:
            seen: list[int] = []

            def retarget(_text, _e_start, t_start, *_rest):
                seen.append(t_start)
                return None

            args = ["lbpclone.py", "--tag", "synthetic", "--module", "EBOOT.BIN", "--dry-run"]
            with patch.object(sys, "argv", args), patch.object(C.M, "ROOT", self.root), \
                    patch.dict(C.M.__dict__, {"CAMPAIGN": self.root}), \
                    patch.object(C.M, "module_bytes", return_value=(data, {"start": exemplar, "file_offset": 0})), \
                    patch.object(C.M, "load_symbols", return_value={}), patch.object(C, "retarget", side_effect=retarget), \
                    patch.object(Q, "starter_starts", partial(Q.starter_starts, root=self.root)), \
                    redirect_stdout(io.StringIO()):
                self.assertEqual(C.main(), 0)
            return sorted(seen)

        self.write([{"module": "EBOOT.BIN", "start": f"0x{starter:08x}", "issue": 2}])
        self.assertEqual(targets(), [other])
        self.write([])
        self.assertEqual(targets(), [starter, other])

    def test_readability_keeps_held_placements_and_clone_sources_out_of_batches(self) -> None:
        import lbpreadable

        (self.root / "src" / "eboot").mkdir(parents=True)
        paths = []
        for label, start, helper, note in (
            ("held", 0x1000, "", ""),
            ("has_helper", 0x2000, " helpers=helper@0x00003000-0x00003020", ""),
            ("canonical", 0x4000, "", ""),
            ("clone", 0x4020, "", "/* Clone of src/eboot/canonical.c (lbpclone.py tag synthetic) */\n"),
            ("unrelated", 0x5000, "", ""),
        ):
            rel = f"src/eboot/{label}.c"
            text = f"/* LBPMATCH module=EBOOT.BIN start=0x{start:08x} end=0x{start + 32:08x} "
            (self.root / rel).write_text(text + f"symbol=FUN_{start:08x} lang=c99{helper} */\n{note}int param_1;\n",
                                         encoding="utf-8")
            paths.append(rel)

        def queued(tag: str) -> set[str]:
            output = self.root / tag
            args = ["lbpqueue.py", "--kind", "readability", "--name", tag]
            with patch.object(sys, "argv", args), patch.object(Q.M, "ROOT", self.root), \
                    patch.object(Q, "QUEUE", output), patch.object(Q, "lane_dirs", return_value=[]), \
                    patch.object(Q, "starter_starts", partial(Q.starter_starts, root=self.root)), \
                    patch.object(lbpreadable, "publishable", return_value=paths), redirect_stdout(io.StringIO()):
                self.assertEqual(Q.main(), 0)
            batch, = output.glob("*.json")
            return {row["source"] for row in json.loads(batch.read_text(encoding="utf-8"))["items"]}

        self.write([{"module": "EBOOT.BIN", "start": f"0x{s:08x}", "issue": 2} for s in (0x1000, 0x3000, 0x4020)])
        self.assertEqual(queued("held"), {"src/eboot/unrelated.c"})
        self.write([])
        self.assertEqual(queued("unheld"), set(paths) - {"src/eboot/clone.c"})


if __name__ == "__main__":
    unittest.main()
