#!/usr/bin/env python3
"""Game-free tests for the source quality report (tools/lbplint.py)."""
from __future__ import annotations

import io
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbplint as L  # noqa: E402

GOOD = ("/* LBPMATCH module=EBOOT.BIN start=0x08804000 end=0x08804008 symbol=FUN_08804000 */\n"
        "/* Hand-written reconstruction from the disassembly. */\n"
        "int FUN_08804000(void) { return 0; }\n")


class MalformedHeaderTests(unittest.TestCase):
    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        (self.root / "src" / "eboot").mkdir(parents=True)

    def run_lint(self) -> tuple[int, dict]:
        out = self.root / "try" / "lint.json"
        with patch.object(L.M, "ROOT", self.root), \
                patch.object(sys, "argv", ["lbplint.py", "--json", str(out)]), \
                patch("sys.stdout", io.StringIO()):
            code = L.main()
        return code, json.loads(out.read_text(encoding="utf-8"))

    def test_a_header_that_does_not_parse_is_a_finding_not_a_crash(self) -> None:
        # the header lacks end=, but the note is a known kind: the report must list the file under `header`
        (self.root / "src" / "eboot" / "good.c").write_text(GOOD, encoding="utf-8")
        (self.root / "src" / "eboot" / "bad.c").write_text(
            "/* LBPMATCH module=EBOOT.BIN start=0x08804010 symbol=FUN_08804010 */\n"
            "/* Hand-written reconstruction from the disassembly. */\n"
            "extern int FUN_08804000(int);\n"
            "int FUN_08804010(void) { return 0; }\n", encoding="utf-8")
        code, report = self.run_lint()
        self.assertEqual(code, 0)
        self.assertEqual(report["sources"], 2)
        self.assertEqual(report["files"]["header"], ["src/eboot/bad.c"])

    def test_a_file_with_no_header_at_all_is_a_finding(self) -> None:
        (self.root / "src" / "eboot" / "none.c").write_text(
            "/* Hand-written reconstruction from the disassembly. */\nint f(void) { return 0; }\n", encoding="utf-8")
        code, report = self.run_lint()
        self.assertEqual(code, 0)
        self.assertEqual(report["files"]["header"], ["src/eboot/none.c"])


if __name__ == "__main__":
    unittest.main()
