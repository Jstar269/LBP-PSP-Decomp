#!/usr/bin/env python3
"""Synthetic tests for the scratch compiler tool (tools/lbpcc.py). The WSL step runs here under `sh` with stand-in
compiler and objdump scripts; no real compiler is needed."""
from __future__ import annotations

import io
import os
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stderr
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpcc  # noqa: E402
import lbpmatch as M  # noqa: E402

# A stand-in psp-gcc: it compiles only when an -I directory holds lbp/probe.h, as a source including it needs.
FAKE_GCC = """#!/bin/sh
out=; found=no; prev=
for a; do
  [ "$prev" = -o ] && out=$a
  [ "$prev" = -I ] && [ -f "$a/lbp/probe.h" ] && found=yes
  prev=$a
done
[ $found = yes ] || { echo "probe.c:1:21: lbp/probe.h: No such file or directory"; exit 1; }
printf 'ELF' > "$out"
"""
FAKE_OBJDUMP = "#!/bin/sh\nfor n in 1 2 3 4 5; do echo header; done\necho '   0:\tjr ra'\n"


@unittest.skipIf(os.name == "nt", "runs the WSL step with a local sh")
class LbpCcTests(unittest.TestCase):
    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        (self.root / "include" / "lbp").mkdir(parents=True)
        (self.root / "include" / "lbp" / "probe.h").write_text("typedef int s32;\n", encoding="utf-8")
        bin_dir = self.root / "bin"
        bin_dir.mkdir()
        for name, text in (("psp-gcc", FAKE_GCC), ("psp-objdump", FAKE_OBJDUMP)):
            (bin_dir / name).write_text(text, encoding="utf-8")
            (bin_dir / name).chmod(0o755)
        self.src = self.root / "probe.c"
        self.src.write_text('#include "lbp/probe.h"\ns32 f(void) { return 1; }\n', encoding="utf-8")
        self.scripts: list[str] = []
        profiles = {name: {"driver": str(bin_dir / "psp-gcc"), "B": None, "flags": "-Os -G0"}
                    for name in M.GCC_PROFILE_NAMES}
        self.patch(M, "ROOT", self.root)
        self.patch(M, "to_wsl", lambda p: str(Path(p).resolve()))
        self.patch(M, "gcc_profiles", lambda: profiles)
        self.patch(M, "pspdev_bin", lambda: str(bin_dir))
        self.patch(M, "native_dir", lambda: str(bin_dir))
        self.patch(M, "run_wsl", self.run_wsl)

    def patch(self, target, name: str, value) -> None:
        patcher = mock.patch.object(target, name, value)
        patcher.start()
        self.addCleanup(patcher.stop)

    def run_wsl(self, lines: list[str], name: str, native: str | None = None) -> int:
        script = "\n".join(lines)
        self.scripts.append(script)
        return subprocess.run(["sh", "-c", script], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode

    def main(self, *argv: str) -> int:
        with mock.patch.object(sys, "argv", ["lbpcc.py", str(self.src), *argv]), redirect_stderr(io.StringIO()):
            return lbpcc.main()

    def test_a_gcc_probe_finds_the_project_headers_as_lbpmatch_does(self) -> None:
        self.assertEqual(self.main(), 0)                  # the primary profile
        self.assertTrue((self.root / "build" / "lbpcc" / "probe.o").exists())
        self.assertIn("-I include", self.scripts[-1])

    def test_a_failed_compile_exits_1_and_an_earlier_object_does_not_count(self) -> None:
        self.assertEqual(self.main("--cc", "gcc171-O2"), 0)
        self.src.write_text('#include "lbp/missing.h"\n', encoding="utf-8")
        (self.root / "include" / "lbp" / "probe.h").unlink()
        self.assertEqual(self.main("--cc", "gcc171-O2"), 1)
        self.assertFalse((self.root / "build" / "lbpcc" / "probe.o").exists())

    def test_snc_and_unknown_profiles_are_refused_not_compiled_with_mwcc(self) -> None:
        for cc in ("snc127-O2", "gcc999-Os"):
            with self.subTest(cc=cc), self.assertRaises(SystemExit) as caught:
                self.main("--cc", cc)
            self.assertEqual(caught.exception.code, 2)
        self.assertEqual(self.scripts, [])


if __name__ == "__main__":
    unittest.main()
