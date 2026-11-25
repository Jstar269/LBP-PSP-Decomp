#!/usr/bin/env python3
"""Checks of tools/lbpcceval.py on a synthetic tree (no toolchain, no game data): lbpmatch's main() is replaced by a
stub that writes the --json rows, so only what `run` does around it is tested.

    python tools/test_lbpcceval.py [-v]
"""
from __future__ import annotations

import argparse
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpcceval as C  # noqa: E402

SOURCE = """/* LBPMATCH module=EBOOT.BIN start=0x{start:08x} end=0x{end:08x} symbol=FUN_{start:08x} lang=c99 cc={cc} */
int FUN_{start:08x}(int x)
{{
  return x + 1;
}}
"""


class RunTests(unittest.TestCase):
    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        src = self.root / "src" / "eboot"
        src.mkdir(parents=True)
        for start, cc in ((0x08804000, "gcc404x-Os"), (0x08804100, "gcc171-Os"), (0x08804200, "mwcc219")):
            (src / f"FUN_{start:08x}.c").write_text(SOURCE.format(start=start, end=start + 0x10, cc=cc),
                                                    encoding="utf-8")
        self.compiled: list[str] = []

    def fake_main(self) -> int:
        argv = sys.argv
        out = Path(argv[argv.index("--json") + 1])
        files = [Path(a) for a in argv[argv.index("--json") + 2:]]
        self.compiled = [p.read_text(encoding="utf-8").split("\n", 1)[0] for p in files]
        rows = [{"source": p.relative_to(self.root).as_posix(), "start": f"0x{C.M.parse_header(p)['start']:08x}",
                 "status": "EXACT", "words_differ": 0, "detail": ""} for p in files]
        out.write_text(json.dumps(rows), encoding="utf-8")
        return 0

    def run_cmd(self, src_set: bool, sample: Path) -> int:
        args = argparse.Namespace(name="cand", driver="/opt/psp-gcc", B=None, flags="-Os -G0", parallel=1,
                                  src_set=src_set)
        profiles: dict = {}
        with mock.patch.object(C, "ROOT", self.root), mock.patch.object(C, "SAMPLE", sample), \
                mock.patch.object(C, "WORK", self.root / "try" / "cceval"), \
                mock.patch.object(C.M, "gcc_profiles", return_value=profiles), \
                mock.patch.object(C.M, "main", self.fake_main), mock.patch.dict("os.environ"), \
                mock.patch.object(sys, "argv", list(sys.argv)), mock.patch("sys.stdout"):
            return C.cmd_run(args)

    def test_src_set_runs_without_the_fixed_sample(self) -> None:
        # data/cceval-sample.json is disc-derived and not in the public repository (lbppublish.DISC_DERIVED);
        # `run --src-set` does not use it
        missing = self.root / "data" / "cceval-sample.json"
        self.assertEqual(self.run_cmd(True, missing), 0)
        result = json.loads((self.root / "try" / "cceval" / "results-src" / "cand.json").read_text(encoding="utf-8"))
        self.assertEqual({k: v["n"] for k, v in result["parts"].items()}, {"gcc171": 1, "gcc410": 1, "miss": 0})
        self.assertEqual(result["total_exact"], 2)
        self.assertTrue(all("cc=gcc410-Os" in head for head in self.compiled))

    def test_fixed_sample_run_without_the_sample_says_how_to_get_it(self) -> None:
        missing = self.root / "data" / "cceval-sample.json"
        with self.assertRaises(SystemExit) as cm:
            self.run_cmd(False, missing)
        self.assertIn("cceval-sample.json", str(cm.exception.code))
        self.assertIn("--src-set", str(cm.exception.code))

    def test_fixed_sample_is_read_when_given(self) -> None:
        sample = self.root / "data" / "cceval-sample.json"
        sample.parent.mkdir(parents=True)
        row = {"start": "0x08804000", "end": "0x08804010", "source": "src/eboot/FUN_08804000.c"}
        sample.write_text(json.dumps({"gcc171": [], "gcc410": [row], "miss": []}), encoding="utf-8")
        self.assertEqual(self.run_cmd(False, sample), 0)
        result = json.loads((self.root / "try" / "cceval" / "results" / "cand.json").read_text(encoding="utf-8"))
        self.assertEqual(result["parts"]["gcc410"]["exact"], 1)


if __name__ == "__main__":
    unittest.main()
