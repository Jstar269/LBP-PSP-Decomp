#!/usr/bin/env python3
"""Synthetic tests for tools/lbpdiff.py's command line (no game, compiler or configuration)."""
from __future__ import annotations

import io
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpdiff as D


class CommandLineTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str, str]:
        with patch("sys.stdout", new_callable=io.StringIO) as out, patch("sys.stderr", new_callable=io.StringIO) as err:
            with self.assertRaises(SystemExit) as cm:
                D.main(argv)
        return cm.exception.code, out.getvalue(), err.getvalue()

    def test_help_prints_usage_instead_of_reading_a_file_named_help(self) -> None:
        code, out, _ = self.run_main(["--help"])
        self.assertEqual(code, 0)
        self.assertIn("usage:", out)
        self.assertIn("lbpmatch.py", out)

    def test_no_source_is_a_usage_error(self) -> None:
        code, _, err = self.run_main([])
        self.assertEqual(code, 2)
        self.assertIn("sources", err)

    def test_missing_source_is_named(self) -> None:
        code, _, err = self.run_main(["src/eboot/no_such_source_00000000.c"])
        self.assertEqual(code, 2)
        self.assertIn("no_such_source_00000000.c: no such file", err)


if __name__ == "__main__":
    unittest.main()
