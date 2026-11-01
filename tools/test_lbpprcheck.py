#!/usr/bin/env python3
"""Tests for the contribution checks (lbpprcheck.py), on a throwaway git repository."""
from __future__ import annotations

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
        self.commit("bad")
        kinds = sorted(r.split()[0] for r in self.refusals())
        self.assertEqual(kinds, ["binary", "binary", "place", "place", "place"])

    def test_private_paths_secrets_and_dumps(self) -> None:
        self.write("docs/a.md", "see C:" + "\\Users\\someone\\game\n")   # split: this file is checked too
        self.write("docs/b.md", "token ghp_" + "a" * 30 + "\n")
        self.write("docs/c.md", "placeholders are fine: C:\\Users\\<you>\\x and /home/<user>/x\n")
        self.write("wip/x.txt", "".join(f"{0x10000 + 4 * i:08x}:  27bdfff0  addiu sp,sp,-16\n" for i in range(20)))
        self.commit("bad")
        got = self.refusals()
        self.assertEqual(sorted(r.split()[0] + " " + r.split()[1] for r in got),
                         ["dump wip/x.txt:", "private docs/a.md:", "private docs/b.md:"])

    def test_symbols_are_append_only(self) -> None:
        self.write("symbols/EBOOT.BIN.txt", "a = 0x00010004;\n")
        self.commit("edit")
        self.assertEqual([r.split()[0] for r in self.refusals()], ["symbols"])

    def test_header_note_and_gate(self) -> None:
        self.write("src/eboot/no_header.c", "int f(void) { return 0; }\n")
        self.write("src/eboot/no_note.c", HEADER + "/* written somehow */\nint FUN_00010000(void) { return 0; }\n")
        self.write("src/eboot/asm.c", HEADER + NOTE + 'void FUN_00010000(void) { asm("addiu $sp,$sp,-16"); }\n')
        self.commit("bad")
        got = sorted(r.split()[0] + " " + r.split()[1] for r in self.refusals())
        self.assertEqual(got, ["gate src/eboot/asm.c:", "header src/eboot/no_header.c:", "header src/eboot/no_note.c:"])


if __name__ == "__main__":
    unittest.main()
