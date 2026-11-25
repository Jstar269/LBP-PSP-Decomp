#!/usr/bin/env python3
"""Game-free tests for the disc intake (tools/lbpintake.py): a stand-in decryptor script, no disc, no 7-Zip (the disc
description of step 2 is given as an existing inputs/disc.json, which the intake then reuses)."""
from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOL = Path(__file__).resolve().parent / "lbpintake.py"


class DecryptorStepTests(unittest.TestCase):
    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.tmp = Path(tmp.name)
        self.inputs = self.tmp / "inputs"
        self.inputs.mkdir()
        (self.inputs / "disc.json").write_text(json.dumps({"image": "disc.iso", "image_size": 0,
                                                           "param_sfo": {"DISC_ID": "TEST00000"},
                                                           "executables": []}), encoding="utf-8")
        self.iso = self.tmp / "disc.iso"
        self.iso.write_bytes(b"")
        (self.tmp / "decryptor").mkdir()

    def decryptor(self, body: str) -> Path:
        script = self.tmp / "decryptor" / "decrypt.py"
        script.write_text("import sys\nroot = sys.argv[sys.argv.index('--user-data-root') + 1]\n" + body,
                          encoding="utf-8")
        return script

    def intake(self, script: Path) -> subprocess.CompletedProcess:
        return subprocess.run([sys.executable, str(TOOL), "--iso", str(self.iso), "--decryptor", str(script),
                               "--inputs", str(self.inputs)], capture_output=True, text=True)

    def test_a_failing_decryptor_fails_the_intake_and_is_retried_next_time(self) -> None:
        script = self.decryptor("import os\nos.makedirs(root)\nopen(os.path.join(root, 'EBOOT.BIN'), 'wb')"
                                ".write(b'half')\nprint('bad key', file=sys.stderr)\nsys.exit(3)\n")
        r = self.intake(script)
        self.assertNotEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("decryptor failed (exit 3)", r.stderr)
        self.assertFalse((self.inputs / "decrypt").exists())      # not left where the next run would skip the step
        self.assertTrue((self.inputs / "decrypt.failed" / "EBOOT.BIN").is_file())
        self.assertIn("bad key", (self.inputs / "decrypt.log").read_text(encoding="utf-8"))
        # the next run runs the decryptor again (here it fails again, the same way)
        r = self.intake(script)
        self.assertIn("decryptor failed (exit 3)", r.stderr)

    def test_no_module_to_describe_is_a_failure(self) -> None:
        script = self.decryptor("import os\nos.makedirs(root)\nopen(os.path.join(root, 'EBOOT.BIN'), 'wb')"
                                ".write(b'not an elf')\n")
        r = self.intake(script)
        self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
        self.assertIn("NOT AN ELF", r.stdout)
        self.assertEqual(json.loads((self.inputs / "modinfo.json").read_text(encoding="utf-8")), [])


if __name__ == "__main__":
    unittest.main()
