#!/usr/bin/env python3
"""Checks of tools/lbpmatch.py's behaviour when measurements overlap or the machine under it fails (no toolchain
needed). They pin down the flake in which one full measurement pass demoted a random source: two runs on the same
source shared build/<module>/<key>/, so the second run's start-up wipe deleted the first run's compile.log/obj.o and the
first run died with no status line.

    python tools/test_lbpmatch_concurrency.py [-v]
"""
from __future__ import annotations

import json
import os
import subprocess
import sys
import tempfile
import threading
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402

HEADER = "/* LBPMATCH module=EBOOT.BIN start=0x08100000 end=0x08100010 symbol=FUN_08100000 lang=c99 */\nint x;\n"


class BuildDirLocks(unittest.TestCase):
    def test_a_second_process_cannot_take_a_held_key_and_gets_it_after_the_release(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            lock_dir = Path(d) / ".locks"
            probe = ("import sys; sys.path.insert(0, sys.argv[1]); import lbpmatch as M; from pathlib import Path\n"
                     "fh = open(Path(sys.argv[2]) / 'EBOOT.BIN' / 'k.lock', 'a+b')\n"
                     "sys.exit(0 if M._try_lock(fh) else 7)\n")
            tools = str(Path(__file__).resolve().parent)

            def try_from_another_process() -> int:
                return subprocess.run([sys.executable, "-c", probe, tools, str(lock_dir)]).returncode

            with M.build_dir_locks(lock_dir, ["EBOOT.BIN/k"]):
                self.assertEqual(try_from_another_process(), 7, "a held key must be refused to another process")
            self.assertEqual(try_from_another_process(), 0, "a released key must be free (no stale lock)")

    def test_the_same_key_twice_in_one_run_does_not_deadlock(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            with M.build_dir_locks(Path(d), ["EBOOT.BIN/k", "EBOOT.BIN/k", "EBOOT.BIN/j"]):
                pass


class OverlappingRuns(unittest.TestCase):
    def test_overlapping_runs_on_one_source_keep_their_own_build_files(self) -> None:
        """The reproducer of the flake: run B starts while run A is between its compile and its comparison."""
        with tempfile.TemporaryDirectory() as d:
            tmp = Path(d)
            src = tmp / "src" / "t.c"
            src.parent.mkdir(parents=True)
            src.write_text(HEADER, encoding="utf-8")
            build_root = tmp / "build"

            def out_dir() -> Path:      # the key depends on ROOT, which is patched while the runs are live
                return build_root / "EBOOT.BIN" / M.build_key(src)

            a_compiled, release_a = threading.Event(), threading.Event()
            calls: list[str] = []

            def fake_run_wsl(lines, name, native=None):
                if name != "compile":
                    return
                who = "A" if not calls else "B"
                calls.append(who)
                out_dir().mkdir(parents=True, exist_ok=True)
                (out_dir() / "compile.log").write_text(f"error from {who}\n", encoding="utf-8")
                if who == "A":
                    a_compiled.set()
                    self.assertTrue(release_a.wait(30), "A was never released")

            def argv(tag: str) -> list[str]:
                return ["lbpmatch.py", "--quiet", str(src), "--json", str(tmp / f"{tag}.json")]

            def run():
                M.main()        # reads sys.argv once, at its start: the test sets it before starting each thread

            with patch.dict(os.environ, {"LBPMATCH_BUILD_ROOT": str(build_root), "LBP_UNUSED": "1"}), \
                 patch.object(M, "ROOT", tmp), patch.object(M, "native_dir", return_value="/fake/native"), patch.object(M, "run_wsl", side_effect=fake_run_wsl):
                saved_argv = sys.argv
                ta, tb = threading.Thread(target=run), threading.Thread(target=run)
                sys.argv = argv("a")
                ta.start()
                self.assertTrue(a_compiled.wait(30))
                sys.argv = argv("b")
                tb.start()
                tb.join(1.5)
                self.assertTrue(tb.is_alive(), "B must wait for A's build directory, not run over it")
                self.assertEqual((out_dir() / "compile.log").read_text(encoding="utf-8"), "error from A\n",
                                 "B wiped or rewrote A's build files while A was still running")
                release_a.set()
                ta.join(30)
                tb.join(30)
                sys.argv = saved_argv
            for tag in "ab":
                row = json.loads((tmp / f"{tag}.json").read_text(encoding="utf-8"))[0]
                self.assertEqual(row["status"], "COMPILE_FAILED")
                self.assertIn(f"error from {tag.upper()}", row["detail"])


class MachineFailures(unittest.TestCase):
    def run_main(self, fake_run_wsl) -> list[dict]:
        with tempfile.TemporaryDirectory() as d:
            tmp = Path(d)
            src = tmp / "t.c"
            src.write_text(HEADER, encoding="utf-8")
            with patch.dict(os.environ, {"LBPMATCH_BUILD_ROOT": str(tmp / "build"), "LBP_UNUSED": "1"}), \
                 patch.object(M, "ROOT", tmp), patch.object(M, "native_dir", return_value="/fake/native"), patch.object(M, "run_wsl", side_effect=fake_run_wsl), \
                 patch.object(sys, "argv", ["lbpmatch.py", "--quiet", str(src), "--json", str(tmp / "r.json")]):
                M.main()
            return json.loads((tmp / "r.json").read_text(encoding="utf-8"))

    def test_a_step_that_left_no_log_is_a_status_not_a_traceback(self) -> None:
        rows = self.run_main(lambda lines, name, native=None: None)
        self.assertEqual(rows[0]["status"], "COMPILE_FAILED")
        self.assertIn("no compile.log", rows[0]["detail"])

    def test_a_compiler_that_died_is_not_reported_as_a_compile_error(self) -> None:
        def fake(lines, name, native=None):
            if name == "compile":
                d = next(Path(os.environ["LBPMATCH_BUILD_ROOT"]).glob("EBOOT.BIN/*"))
                (d / "compile.log").write_text("terminate called after throwing an instance of 'std::filesystem_error'\n"
                                               "  what():  directory iterator cannot advance: Out of memory\n"
                                               "Aborted (core dumped)\n", encoding="utf-8")
        self.assertEqual(self.run_main(fake)[0]["status"], "COMPILE_CRASHED")

    def test_a_wsl_service_failure_stops_the_run(self) -> None:
        text = ("A connection attempt failed.\r\nError code: Wsl/Service/0x8007274c\r\n").encode("utf-16-le")
        done = subprocess.CompletedProcess([], 4294967295, stdout=text)
        with tempfile.TemporaryDirectory() as d, patch.object(M, "ROOT", Path(d)), \
                patch("subprocess.run", return_value=done) as run, patch.object(M.time, "sleep") as sleep:
            with self.assertRaises(M.ToolFailure) as caught:
                M.run_wsl(["true"], "compile")
            self.assertIn("0x8007274c", str(caught.exception))
            self.assertEqual(run.call_count, M.WSL_CONNECT_ATTEMPTS)     # waited out aloud, then stopped
            self.assertEqual(sleep.call_count, M.WSL_CONNECT_ATTEMPTS - 1)
            self.assertEqual(list((Path(d) / "build").glob("*.sh")), [], "the script must be removed")

    def test_an_instance_that_terminated_is_not_retried(self) -> None:
        text = "The Windows Subsystem for Linux instance has terminated.\r\n".encode("utf-16-le")
        done = subprocess.CompletedProcess([], 1, stdout=text)
        with tempfile.TemporaryDirectory() as d, patch.object(M, "ROOT", Path(d)),                 patch("subprocess.run", return_value=done) as run, patch.object(M.time, "sleep"):
            with self.assertRaises(M.ToolFailure):
                M.run_wsl(["true"], "compile")
            self.assertEqual(run.call_count, 1)

    def test_a_service_that_answers_on_the_second_attempt_runs_the_step_once(self) -> None:
        down = subprocess.CompletedProcess([], 4294967295, stdout="Error code: Wsl/Service/0x8007274c".encode("utf-16-le"))
        up = subprocess.CompletedProcess([], 0, stdout=b"")
        with tempfile.TemporaryDirectory() as d, patch.object(M, "ROOT", Path(d)),                 patch("subprocess.run", side_effect=[down, up]) as run, patch.object(M.time, "sleep"):
            M.run_wsl(["true"], "compile")
            self.assertEqual(run.call_count, 2)

    def test_a_failing_compiler_is_not_a_wsl_failure(self) -> None:
        done = subprocess.CompletedProcess([], 123, stdout=b"")
        with tempfile.TemporaryDirectory() as d, patch.object(M, "ROOT", Path(d)), patch("subprocess.run", return_value=done):
            M.run_wsl(["false"], "compile")      # xargs exits 123 when a compile fails: that is a result, not an outage


if __name__ == "__main__":
    unittest.main()
