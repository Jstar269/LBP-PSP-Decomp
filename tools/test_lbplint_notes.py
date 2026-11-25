#!/usr/bin/env python3
"""lbplint.provenance_note accepts the note kinds CONTRIBUTING.md lists, and only those."""
from __future__ import annotations

import re
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbplint as L

ROOT = Path(__file__).resolve().parents[1]
HEADER = "/* LBPMATCH module=EBOOT.BIN start=0x08804000 end=0x08804008 symbol=FUN_08804000 lang=c99 */\n"


class NoteKindTests(unittest.TestCase):
    def test_the_listed_notes_keep_their_kinds(self) -> None:
        notes = {
            "hand": "/* Hand-written reconstruction from the disassembly; exact match verified. */",
            "corrected": "/* Ghidra pseudocode (lbpgen.py tag a1) corrected by hand: a loop; exact match verified. */",
            "generated": "/* Mechanical translation of Ghidra pseudocode (lbpgen.py tag a1); exact match verified. */",
            "m2c": "/* Mechanical translation of the disassembly (m2c 0123abc, lbpm2c.py tag m1); verified. */",
            "clone": "/* Clone of src/eboot/a.c (lbpclone.py tag t1): addresses substituted. */",
            "xclone": "/* Clone of hst-decomp src/eboot/a.c (lbpxclone.py tag x1): addresses substituted. */",
            "library": "/* Library source: newlib 1.14.0 libc_stdlib/abort.c function abort, preprocessed. */",
        }
        for kind, note in notes.items():
            self.assertEqual(L.provenance_note(HEADER + note + "\nint x;\n"), (kind, note))

    def test_a_template_note_names_a_tool_this_project_does_not_have(self) -> None:
        self.assertFalse((ROOT / "tools" / "lbptmpl.py").exists())
        note = "/* Container growth for an 8-byte element (template instance, lbptmpl.py tag t1) */"
        self.assertEqual(L.provenance_note(HEADER + note + "\nint x;\n")[0], None)

    def test_contributing_lists_the_tool_of_every_kind(self) -> None:
        listed = (ROOT / "CONTRIBUTING.md").read_text(encoding="utf-8")
        for kind, rx in L.NOTE_KINDS:
            tool = re.search(r"lbp\w+\\\.py", rx.pattern)
            if tool:
                self.assertIn(tool.group(0).replace("\\", ""), listed, kind)


if __name__ == "__main__":
    unittest.main()
