#!/usr/bin/env python3
"""Synthetic tests for tools/lbpconfig.py: the setup steps of docs/GUIDE.md that need no game or compiler."""
from __future__ import annotations

import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpconfig


class SetupTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.config_file = Path(self.tmp.name) / "lbp-config.json"
        self.patch = patch.object(lbpconfig, "CONFIG_FILE", self.config_file)
        self.patch.start()
        lbpconfig._file_cache = None
        self.env = patch.dict(os.environ, {k: v for k, v in os.environ.items()
                                           if not k.startswith(lbpconfig.ENV_PREFIX)}, clear=True)
        self.env.start()

    def tearDown(self) -> None:
        self.env.stop()
        self.patch.stop()
        lbpconfig._file_cache = None
        self.tmp.cleanup()

    def test_the_example_config_loads_as_documented(self) -> None:
        # docs/GUIDE.md, Setup step 2: copy lbp-config.example.json to lbp-config.json, then check
        example = Path(lbpconfig.__file__).resolve().parents[1] / "lbp-config.example.json"
        self.config_file.write_text(example.read_text(encoding="utf-8"), encoding="utf-8")
        data = json.loads(example.read_text(encoding="utf-8"))
        keys = [k for k in data if not k.startswith("_")]
        self.assertEqual(sorted(set(keys) - set(lbpconfig.KEYS)), [], "example keys lbpconfig.py does not know")
        for key in keys:
            self.assertEqual(lbpconfig.get(key), data[key].strip())
        # the placeholder campaign is LBP's own, not hst-decomp's (hst_campaign_root names that one)
        self.assertNotEqual(data["campaign_root"], data["hst_campaign_root"])

    @patch("subprocess.run", side_effect=FileNotFoundError(2, "No such file or directory", "wsl.exe"))
    def test_check_without_wsl_exe_is_a_config_error(self, _mock_run) -> None:
        # off Windows (or without WSL) `check` used to end in a FileNotFoundError traceback
        with self.assertRaises(lbpconfig.ConfigError) as ctx:
            lbpconfig._wsl_exists("/opt/pspdev/bin/psp-ld")
        self.assertIn("wsl.exe was not found", str(ctx.exception))


if __name__ == "__main__":
    unittest.main()
