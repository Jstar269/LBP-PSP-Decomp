#!/usr/bin/env python3
"""Tests for tools/lbpmodinfo.py and the intake that uses it (tools/lbpintake.py), on synthetic files built here byte
by byte (no game data)."""
from __future__ import annotations

import contextlib
import io
import os
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpintake as I  # noqa: E402
import lbpmodinfo as MI  # noqa: E402


def small_elf() -> bytes:
    """A static ELF: one load segment at 0x08804000 holding two words (`jr ra; nop`), no section headers."""
    base, off = 0x08804000, 0x60
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9), 2, 8, 1, base, 52, 0, 0, 52, 32, 1,
                       40, 0, 0)
    phdr = struct.pack("<8I", 1, off, base, base, 8, 8, 5, 16)
    out = bytearray(off + 8)
    out[:52] = ehdr
    out[52:84] = phdr
    out[off:] = struct.pack("<II", 0x03E00008, 0)
    return bytes(out)


def run_modinfo(*argv: str) -> tuple[int, str]:
    sink = io.StringIO()
    with mock.patch.object(sys, "argv", ["lbpmodinfo.py", *argv]), contextlib.redirect_stdout(sink):
        try:
            rc = MI.main()
        except SystemExit as e:
            rc = e.code
    return rc, sink.getvalue()


class ModInfo(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def test_a_good_module_exits_0(self):
        path = self.dir / "good.elf"
        path.write_bytes(small_elf())
        rc, out = run_modinfo(str(path))
        self.assertEqual(rc, 0)
        self.assertIn("EXEC entry 0x08804000", out)

    def test_an_encrypted_module_exits_1(self):
        path = self.dir / "EBOOT.BIN"
        path.write_bytes(b"~PSP" + bytes(60))
        rc, out = run_modinfo(str(path))
        self.assertEqual(rc, 1)
        self.assertIn("not an ELF file", out)

    def test_a_truncated_module_is_reported_not_a_traceback(self):
        path = self.dir / "cut.elf"
        path.write_bytes(small_elf()[:60])
        with self.assertRaisesRegex(ValueError, "truncated"):
            MI.describe(path, nids=False)
        rc, out = run_modinfo(str(path))
        self.assertEqual(rc, 1)
        self.assertIn("cut.elf: truncated", out)

    def test_a_missing_file_exits_1(self):
        rc, out = run_modinfo(str(self.dir / "absent.prx"))
        self.assertEqual(rc, 1)
        self.assertIn("absent.prx", out)

    def test_a_short_modules_entry_spec_is_an_error_message(self):
        path = self.dir / "good.elf"
        path.write_bytes(small_elf())
        rc, _out = run_modinfo(str(path), "--modules-entry", "EBOOT.BIN:eboot")
        self.assertIsInstance(rc, str)
        self.assertIn("NAME:SRC_DIR:KIND:ORIGIN", rc)


class Intake(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.iso = self.dir / "image.iso"
        self.iso.write_bytes(b"not a real image")
        self.decryptor = self.dir / "decryptor.py"
        self.decryptor.write_text("", encoding="utf-8")

    def tearDown(self):
        self.tmp.cleanup()

    def run_intake(self, iso: Path) -> str:
        argv = ["lbpintake.py", "--iso", str(iso), "--decryptor", str(self.decryptor),
                "--inputs", str(self.dir / "inputs")]
        with mock.patch.object(sys, "argv", argv), contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaises(SystemExit) as caught:
                I.main()
        self.assertIsInstance(caught.exception.code, str)
        return caught.exception.code

    def test_a_missing_image_is_an_error_message(self):
        self.assertIn("no disc image", self.run_intake(self.dir / "absent.iso"))

    def test_without_7zip_the_intake_says_so(self):
        empty = self.dir / "empty-path"
        empty.mkdir()
        with mock.patch.dict(os.environ, {"PATH": str(empty)}), \
                mock.patch.object(I, "SEVEN_ZIP", str(self.dir / "no-7zip" / "7z.exe")):
            self.assertIn("7-Zip not found", self.run_intake(self.iso))

    def test_an_image_without_param_sfo_is_an_error_message(self):
        def no_output(*args: str, capture: bool = True) -> subprocess.CompletedProcess:
            return subprocess.CompletedProcess(["7z", *args], 2, b"", b"")
        with mock.patch.object(I, "seven_zip", no_output):
            self.assertIn("cannot read PSP_GAME/PARAM.SFO", self.run_intake(self.iso))
        self.assertFalse((self.dir / "inputs" / "disc.json").exists())


if __name__ == "__main__":
    unittest.main()
