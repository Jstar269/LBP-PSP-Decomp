#!/usr/bin/env python3
"""Tests for tools/lbpxclone.py: which LBP functions a cross-project clone may be installed at (synthetic trees only)."""
from __future__ import annotations

import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpxclone as X  # noqa: E402

MODULE = "EBOOT.BIN"
HST_START = 0x00001000
TARGETS = (0x08804000, 0x08804020, 0x08804040)
CODE = b"\x08\x00\xe0\x03\x00\x00\x00\x00" * 4   # jr ra; nop, repeated: one synthetic shape, no references
LBP_HEADER = "/* LBPMATCH module=EBOOT.BIN start=0x{0:08x} end=0x{1:08x} symbol={2} lang=c99{3} */\nint x;\n"


class TargetTests(unittest.TestCase):
    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name) / "lbp"
        self.hst = Path(tmp.name) / "hst"
        (self.root / "src" / "eboot").mkdir(parents=True)
        (self.root / "symbols").mkdir()
        (self.root / "data").mkdir()
        (self.root / "ledger").mkdir()
        (self.root / "ledger" / "campaign.json").write_text(json.dumps({"modules": [{"name": MODULE, "functions": [
            {"start": s, "end": s + 32, "boundary_status": "auto"} for s in TARGETS]}]}), encoding="utf-8")
        (self.hst / "src" / "eboot").mkdir(parents=True)
        (self.hst / "src" / "eboot" / f"FUN_{HST_START:08x}.c").write_text(
            f"/* HSTMATCH module=EBOOT.BIN start=0x{HST_START:08x} end=0x{HST_START + 32:08x} "
            f"symbol=FUN_{HST_START:08x} lang=c99 */\n/* Hand-written reconstruction. */\n"
            f"void FUN_{HST_START:08x}(void) {{}}\n", encoding="utf-8")
        self.starters([])

    def starters(self, starts: list[int]) -> None:
        (self.root / "data" / "starter-issues.json").write_text(json.dumps({
            "schema": "lbp-decomp.starter-issues.v1", "entries": [
                {"module": MODULE, "start": f"0x{s:08x}", "issue": 2} for s in starts]}), encoding="utf-8")

    def run_xclone(self, during_match=None) -> list[str]:
        """Run lbpxclone.main over the synthetic trees; every candidate matches. Returns the files left in src/."""
        def match(cmd, **_kw):
            if during_match:
                during_match()
            json_at = cmd.index("--json") + 1
            Path(cmd[json_at]).write_text(json.dumps([
                {"source": Path(p).resolve().relative_to(self.root.resolve()).as_posix(), "status": "EXACT"}
                for p in cmd[json_at + 1:]]), encoding="utf-8")

        hst_image = (CODE, {"start": HST_START, "file_offset": 0})
        lbp_image = (CODE * 3, {"start": TARGETS[0], "file_offset": 0})
        host = {"hst_root": self.hst, "hst_campaign_root": self.hst}
        argv = ["lbpxclone.py", "--tag", "t1", "--module", MODULE]
        with patch.object(sys, "argv", argv), patch.object(X.M, "ROOT", self.root), \
                patch.dict(X.M.__dict__, {"CAMPAIGN": self.root}), \
                patch.object(X.CFG, "host_path", side_effect=host.__getitem__), \
                patch.object(X, "hst_modules", return_value={MODULE: hst_image}), \
                patch.object(X.M, "module_bytes", return_value=lbp_image), \
                patch.object(X.M, "load_symbols", return_value={}), \
                patch.object(X.P, "src_dirs", return_value={MODULE: "eboot"}), \
                patch.object(X.subprocess, "run", side_effect=match), redirect_stdout(io.StringIO()):
            self.assertEqual(X.main(), 0)
        return sorted(p.name for p in (self.root / "src" / "eboot").iterdir())

    def test_every_uncovered_function_of_the_shape_gets_a_clone(self) -> None:
        self.assertEqual(self.run_xclone(), [f"FUN_{s:08x}.c" for s in TARGETS])

    def test_a_starter_function_is_never_a_clone_target(self) -> None:
        self.starters([TARGETS[1]])
        self.assertEqual(self.run_xclone(), [f"FUN_{s:08x}.c" for s in (TARGETS[0], TARGETS[2])])

    def test_a_function_placed_as_a_helper_is_covered(self) -> None:
        s, h = TARGETS[0], TARGETS[1]
        (self.root / "src" / "eboot" / "pair.c").write_text(
            LBP_HEADER.format(s, s + 32, f"FUN_{s:08x}", f" helpers=FUN_{h:08x}@0x{h:08x}-0x{h + 32:08x}"),
            encoding="utf-8")
        self.assertEqual(self.run_xclone(), [f"FUN_{TARGETS[2]:08x}.c", "pair.c"])

    def test_a_source_written_during_the_match_run_is_not_duplicated(self) -> None:
        s = TARGETS[2]

        def named_meanwhile() -> None:
            (self.root / "src" / "eboot" / "named.c").write_text(LBP_HEADER.format(s, s + 32, "named", ""),
                                                                 encoding="utf-8")
        self.assertEqual(self.run_xclone(named_meanwhile),
                         [f"FUN_{TARGETS[0]:08x}.c", f"FUN_{TARGETS[1]:08x}.c", "named.c"])


if __name__ == "__main__":
    unittest.main()
