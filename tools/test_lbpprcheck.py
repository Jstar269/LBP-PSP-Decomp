#!/usr/bin/env python3
"""Tests for the contribution checks (lbpprcheck.py), on a throwaway git repository."""
from __future__ import annotations

import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpprcheck as P

HEADER = "/* LBPMATCH module=EBOOT.BIN start=0x00010000 end=0x00010008 symbol=FUN_00010000 lang=c99 */\n"
NOTE = "/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human review. */\n"
GOOD = HEADER + NOTE + "int FUN_00010000(void) { return 0; }\n"


class PrCheckTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmp.name)
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "test@example.invalid")
        self.git("config", "user.name", "test")
        self.git("config", "core.autocrlf", "false")
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\n")
        self.write("src/eboot/FUN_00010000.c", GOOD)
        self.commit("base")
        self.git("branch", "base")
        self.git("checkout", "-q", "-b", "topic")

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def git(self, *args: str) -> None:
        subprocess.run(["git", *args], cwd=self.repo, check=True, capture_output=True)

    def write(self, rel: str, data: str | bytes) -> None:
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        (path.write_bytes if isinstance(data, bytes) else lambda d: path.write_text(d, encoding="utf-8", newline="\n"))(data)

    def commit(self, message: str) -> None:
        self.git("add", "-A")
        self.git("commit", "-q", "-m", message)

    def refusals(self) -> list[str]:
        return P.check(self.repo, "base")

    def test_a_clean_source_passes(self) -> None:
        self.write("src/eboot/FUN_00010010.c", GOOD.replace("00010000", "00010010").replace("0x00010008", "0x00010018"))
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\nb = 0x00010010;   # appended\n")
        self.commit("match")
        self.assertEqual(self.refusals(), [])

    def test_binaries_and_never_versioned_places(self) -> None:
        self.write("inputs/EBOOT.BIN", b"\x7fELF\0\0")
        self.write("docs/notes.txt", b"text\0more")
        self.write("try/me/scratch.c", "int x;\n")
        self.write("campaign/ledger/campaign.json", "{}\n")
        self.write("lbp-config.json", "{}\n")
        self.write("data/functions-EBOOT.BIN.txt", "8804000 8804024 auto\n")
        self.write("data/translation-units.json", "{}\n")
        self.commit("bad")
        kinds = sorted(r.split()[0] for r in self.refusals())
        self.assertEqual(kinds, ["binary", "binary", "place", "place", "place", "place", "place"])

    def test_paths_the_ignore_rules_keep_out(self) -> None:
        self.write(".gitignore", "/docs/history/\n/HANDOFF-*.md\n/data/functions-*.txt\n!/data/functions-EBOOT.BIN.txt\n"
                                 "__pycache__/\n")
        self.commit("rules")
        self.git("branch", "-f", "base")
        self.write("docs/history/2026-11-01.md", "notes\n")
        self.write("HANDOFF-2026-11-01.md", "notes\n")
        self.write("data/functions-OTHER.PRX.txt", "8900000 8900040 auto\n")
        self.write("tools/__pycache__/x.txt", "x\n")
        self.write(".gitignore", "")                              # the base's rules count, not the branch's
        self.git("add", "-A", "-f")
        self.git("commit", "-q", "-m", "forced")
        got = sorted(r.split()[1] for r in self.refusals() if r.startswith("place"))
        self.assertEqual(got, ["HANDOFF-2026-11-01.md:", "data/functions-OTHER.PRX.txt:",
                               "docs/history/2026-11-01.md:", "tools/__pycache__/x.txt:"])
        self.assertEqual(len(self.refusals()), 4)

    def test_a_path_the_ignore_rules_allow_passes(self) -> None:
        self.write(".gitignore", "/data/functions-*.txt\n!/data/functions-EBOOT.BIN.txt\n/docs/history/\n")
        self.commit("rules")
        self.git("branch", "-f", "base")
        self.write("docs/notes/history.md", "x\n")
        self.write("docs/history.md", "x\n")
        self.commit("ok")
        self.assertEqual(self.refusals(), [])

    def test_private_paths_secrets_and_dumps(self) -> None:
        self.write("docs/a.md", "see C:" + "\\Users\\someone\\game\n")   # split: this file is checked too
        self.write("docs/b.md", "token ghp_" + "a" * 30 + "\n")
        self.write("docs/c.md", "placeholders are fine: C:\\Users\\<you>\\x and /home/<user>/x\n")
        self.write("wip/x.txt", "".join(f"{0x10000 + 4 * i:08x}:  27bdfff0  addiu sp,sp,-16\n" for i in range(20)))
        self.commit("bad")
        got = self.refusals()
        self.assertEqual(sorted(r.split()[0] + " " + r.split()[1] for r in got),
                         ["dump wip/x.txt:", "private docs/a.md:", "private docs/b.md:"])

    def test_unusual_names_are_checked_like_any_other(self) -> None:
        # git C-quotes names with non-ASCII bytes ("inputs/disc\303\251.txt"): each is still the file itself
        self.write("inputs/disc\u00e9.txt", "a table made from the disc\n")
        self.write("docs/caf\u00e9.bin.txt", b"A" * 9000 + b"\0" * 100)          # git sees text: NULs after 8000 bytes
        self.write("docs/r\u00e9sum\u00e9.md", "token ghp_" + "a" * 30 + "\n")
        self.write("src/eboot/FUN_\u00e9.c", "int f(void) { return 0; }\n")
        self.write("docs/big \u00e9.txt", "x" * (P.MAX_BYTES + 1))
        self.commit("bad")
        got = sorted(r.split()[0] for r in self.refusals())
        self.assertEqual(got, ["binary", "header", "place", "private", "size"])

    @unittest.skipIf(os.name == "nt", "names Windows cannot create")
    def test_names_with_pathspec_magic_or_control_characters(self) -> None:
        self.write(":x.md", "token ghp_" + "a" * 30 + "\n")                      # `:x.md` as a pathspec is x.md
        self.write("docs/[ab].md", "see /home/" + "someone/game\n")              # a glob too: still this file
        self.write("docs/a.md", "fine\n")
        self.write("wip/tab\tname.txt",
                   "".join(f"{0x10000 + 4 * i:08x}:  27bdfff0  addiu sp,sp,-16\n" for i in range(20)))
        self.write("src/eboot/new\nline.c", "int f(void) { return 0; }\n")
        self.commit("bad")
        got = sorted(r.split()[0] for r in self.refusals())
        self.assertEqual(got, ["dump", "header", "private", "private"])
    def test_the_report_is_the_one_its_figures_make(self) -> None:
        import json

        import lbpreport as R
        rows = {"M.BIN": [(0x100, 0x20, 0x20, "src/m/a.c", "do_a"), (0x120, 0x20, 0, None, None)]}
        figures = {"M.BIN": {"public_scope_bytes": 0x40, "public_t1_bytes": 0x20, "public_t1_percent": 50.0,
                             "public_scope_functions": 2, "public_t1_functions": 1, "t2": "x", "t3": "x"}}
        self.write(R.FIGURES_REL, json.dumps(figures))
        self.write(R.ROWS_REL, R.render_rows(rows))
        self.write(P.OBJDIFF_REPORT, R.render(R.build_report(figures, rows)))
        self.commit("report")
        self.git("branch", "-f", "base")
        report = R.build_report(figures, rows)
        report["blob"] = "QUJD" * 100000                              # a payload; the units still add up
        self.write(P.OBJDIFF_REPORT, json.dumps(report))
        self.commit("payload")
        self.assertEqual(P.report_problems((self.repo / P.OBJDIFF_REPORT).read_bytes()), [])
        self.assertEqual([r.split()[0] for r in self.refusals()], ["report"])
        done = {"M.BIN": {**figures["M.BIN"], "public_t1_bytes": 0x40, "public_t1_percent": 100.0,
                          "public_t1_functions": 2}}                  # self-consistent, but not the tree's figures
        self.write(P.OBJDIFF_REPORT, R.render(R.build_report(done)))
        self.commit("inflated")
        self.assertEqual([r.split()[0] for r in self.refusals()], ["report"])
        rows["M.BIN"][1] = (0x120, 0x20, 0x20, "src/m/b.c", "do_b")   # the figures and the report regenerated
        self.write(R.FIGURES_REL, json.dumps(done))
        self.write(R.ROWS_REL, R.render_rows(rows))
        self.write(P.OBJDIFF_REPORT, R.render(R.build_report(done, rows)))
        self.commit("regenerated")
        self.assertEqual(self.refusals(), [])

    def test_symbols_are_append_only(self) -> None:
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010004;\n")
        self.commit("edit")
        self.assertEqual([r.split()[0] for r in self.refusals()], ["symbols"])

    def test_lines_that_look_like_diff_headers_or_hold_line_breaks(self) -> None:
        # `++...` is a `+++` line in the diff; str.splitlines() splits at \f, U+2028 and other line breaks, and text=True
        # turns \r into a newline: the text after any of them lost its `+`
        self.write("docs/a.md", "".join(f"++ {0x10000 + 4 * i:08x}:  27bdfff0  addiu sp,sp,-16\n" for i in range(20)))
        self.write("docs/b.md", "note\rsee /home/" + "someone/game\n")
        self.write("docs/c.md", "x\u2028token ghp_" + "a" * 30 + "\n")
        self.write("docs/d.md", "".join(f"\f{0x10000 + 4 * i:08x}:  27bdfff0  addiu sp,sp,-16\n" for i in range(20)))
        self.write("docs/e.md", "++/home/" + "someone/game")                     # and no newline at the end
        self.commit("bad")
        got = sorted(r.split()[0] + " " + r.split()[1] for r in self.refusals())
        self.assertEqual(got, ["dump docs/a.md:", "dump docs/d.md:", "private docs/b.md:", "private docs/c.md:",
                               "private docs/e.md:"])

    def test_a_symbol_line_that_starts_with_two_dashes_is_append_only_too(self) -> None:
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\n--b = 0x00010004;\n")
        self.commit("base symbols")
        self.git("branch", "-f", "base")
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\n")
        self.commit("drop a line")
        self.assertEqual([r.split()[0] for r in self.refusals()], ["symbols"])
        self.assertEqual(P.diff_lines(self.repo, "base", "symbols/EBOOT.BIN.txt"), ([], ["--b = 0x00010004;"]))
    def test_symbol_maps_and_lists_are_not_deleted_or_replaced(self) -> None:
        self.write("data/functions-EBOOT.BIN.txt", "8804000 8804024 auto\n")
        self.write("symbols/OTHER.PRX.txt", "c = 0x00000100;\n")
        self.commit("base lists")
        self.git("branch", "-f", "base")
        self.git("rm", "-q", "symbols/OTHER.PRX.txt")
        (self.repo / "symbols/EBOOT.BIN.txt").unlink()        # a link to a new file that edits a line
        (self.repo / "data/functions-EBOOT.BIN.txt").unlink()
        self.write("symbols/EBOOT.BIN.new", "a = 0x00010004;\n")
        self.write("data/functions.new", "8804000 8804028 auto\n")
        try:
            (self.repo / "symbols/EBOOT.BIN.txt").symlink_to("EBOOT.BIN.new")
            (self.repo / "data/functions-EBOOT.BIN.txt").symlink_to("functions.new")
        except OSError:
            self.skipTest("no symbolic links here")
        self.commit("bad")
        got = sorted(r.split()[0] + " " + r.split()[1] for r in self.refusals())
        self.assertEqual(got, ["place data/functions-EBOOT.BIN.txt:", "symbols symbols/EBOOT.BIN.txt:",
                               "symbols symbols/OTHER.PRX.txt:"])

    def test_deleting_other_files_is_fine(self) -> None:
        self.git("rm", "-q", "src/eboot/FUN_00010000.c")
        self.commit("drop")
        self.assertEqual(self.refusals(), [])

    def test_appended_symbol_lines_must_keep_the_map_loadable(self) -> None:
        # lbpmatch.load_symbols refuses the whole map over one malformed line or a name bound twice, so such an
        # appended line breaks every match of the module; an address name (FUN_/DAT_...) bound elsewhere silently
        # sends its calls to another address
        for line in ("b = 0x0001001;;\n", "b 0x00010010\n", "a = 0x00010004;   # a second address\n",
                     "FUN_00010010 = 0x00010020;\n"):
            with self.subTest(line=line):
                self.git("checkout", "-q", "-B", "topic", "base")
                self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\n" + line)
                self.commit("append")
                self.assertEqual([r.split()[0] for r in self.refusals()], ["symbols"])
        self.git("checkout", "-q", "-B", "topic", "base")
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010000;\na = 0x00010000;   # repeated, same address\n"
                                            "DAT_00010020 = 0x00010020;\n# a comment\n\n")
        self.write("symbols/NEW.PRX.txt", "c = 0x0;\n")
        self.commit("append")
        self.assertEqual(self.refusals(), [])

    def test_header_note_and_gate(self) -> None:
        self.write("src/eboot/no_header.c", "int f(void) { return 0; }\n")
        self.write("src/eboot/no_note.c", HEADER + "/* written somehow */\nint FUN_00010000(void) { return 0; }\n")
        self.write("src/eboot/asm.c", HEADER + NOTE + 'void FUN_00010000(void) { asm("addiu $sp,$sp,-16"); }\n')
        self.commit("bad")
        got = sorted(r.split()[0] + " " + r.split()[1] for r in self.refusals())
        self.assertEqual(got, ["gate src/eboot/asm.c:", "header src/eboot/no_header.c:", "header src/eboot/no_note.c:"])

    def test_note_rules_agree_with_published_sources(self) -> None:
        body = "int FUN_00010000(void) { return 0; }\n"
        zlib = ("/* zlib 1.2.3 (zlib.net, zlib-1.2.3.tar.gz md5 debc62758716a169df9f62e6ab2bc634), inflate.c; the\n"
                "   function verbatim from inflate.c. */\n")                 # the published zlib sources' note
        self.write("src/eboot/zlib_x.c", HEADER + zlib + body)
        self.write("src/eboot/annotated.c", HEADER + "/* see docs/TOOLCHAIN.md */\n" + NOTE + body)
        self.write("wip/near_00010000.c", HEADER + body + "// near miss from lane x (unreviewed)\n")
        self.commit("ok")
        self.assertEqual(self.refusals(), [])
        self.write("wip/twice_00010000.c", HEADER + HEADER + body)
        self.write("src/eboot/late_note.c", HEADER + "/* a */\n/* b */\n/* c */\n" + NOTE + body)
        self.commit("bad")
        got = sorted(r.split()[0] + " " + r.split()[1] for r in self.refusals())
        self.assertEqual(got, ["header src/eboot/late_note.c:", "header wip/twice_00010000.c:"])
        import lbplint
        self.assertEqual(lbplint.provenance_note(HEADER + zlib + body)[0], "library")
        self.assertEqual(lbplint.provenance_note(HEADER + "int x;\n" + NOTE), (None, ""))


if __name__ == "__main__":
    unittest.main()
