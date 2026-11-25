#!/usr/bin/env python3
"""Synthetic tests for tools/lbpbootstrap.py's failure paths (no game data: made-up module bytes only)."""
from __future__ import annotations

import hashlib
import io
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpbootstrap as B


class FailureTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.data = b"made-up module bytes"
        (self.root / "data").mkdir()
        (self.root / "data" / "functions-TEST.BIN.txt").write_text("00000000 00000008\n", encoding="utf-8")
        self.modules = self.root / "data" / "modules.json"
        self.modules.write_text(json.dumps({"title_id": "TEST00000", "modules": [{
            "name": "TEST.BIN", "origin": "disc/TEST.BIN", "sha256": hashlib.sha256(self.data).hexdigest(),
            "size": len(self.data), "load_segments": [{"flags": 7, "start": 0}],
            "functions_file": "data/functions-TEST.BIN.txt", "function_count": 1}]}), encoding="utf-8")
        self.out = self.root / "campaign"

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def run_main(self, *module: str):
        argv = ["lbpbootstrap.py", "--out", str(self.out)] + [a for m in module for a in ("--module", m)]
        with patch.object(B, "ROOT", self.root), patch.object(B, "MODULES", self.modules), \
                patch.object(sys, "argv", argv), patch("sys.stdout", new_callable=io.StringIO), \
                patch("sys.stderr", new_callable=io.StringIO) as err:
            try:
                code = B.main()
            except SystemExit as e:
                code = e.code
        return code, err.getvalue()

    def test_a_missing_file_is_a_usage_error_not_a_traceback(self) -> None:
        for path in (self.root / "no-such-EBOOT.BIN", self.root):   # a typo, or a directory
            code, err = self.run_main(f"TEST.BIN={path}")
            self.assertEqual(code, 2)
            self.assertIn("no such file", err)
        self.assertFalse(self.out.exists())

    def test_a_refused_module_leaves_no_campaign_directory(self) -> None:
        wrong = self.root / "wrong.bin"
        wrong.write_bytes(b"not the module")
        self.assertEqual(self.run_main(f"TEST.BIN={wrong}")[0], 1)
        self.assertFalse(self.out.exists())

    def test_the_right_module_writes_the_campaign(self) -> None:
        good = self.root / "good.bin"
        good.write_bytes(self.data)
        with patch.object(B, "stub_ranges", return_value=[]):
            self.assertEqual(self.run_main(f"TEST.BIN={good}")[0], 0)
        self.assertEqual((self.out / "inputs" / "TEST.BIN.decrypted").read_bytes(), self.data)
        ledger = json.loads((self.out / "ledger" / "campaign.json").read_text(encoding="utf-8"))
        self.assertEqual(ledger["modules"][0]["functions"], [{"start": 0, "end": 8, "boundary_status": "auto"}])


if __name__ == "__main__":
    unittest.main()
