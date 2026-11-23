#!/usr/bin/env python3
"""Exercise the match gate in disposable Git checkouts with synthetic tools."""

from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import unittest

GATE = Path(__file__).with_name("lbpmatchgate.py")


class MatchGateTests(unittest.TestCase):
    def setUp(self):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.root = Path(workspace.name)
        self.tools = self.root / "tools"
        self.tools.mkdir()
        shutil.copyfile(GATE, self.tools / GATE.name)
        (self.tools / "lbpmatch.py").write_text("print('synthetic match')\n", encoding="utf-8")
        (self.tools / "lbpreport.py").write_text("print('synthetic report')\n", encoding="utf-8")
        (self.root / "data").mkdir()
        (self.root / "data/public-figures.json").write_text("{}\n", encoding="utf-8")
        self.git("init", "-q")
        self.commit()
        self.head = self.git("rev-parse", "HEAD")

    def git(self, *args):
        return subprocess.check_output(["git", *args], cwd=self.root, text=True).strip()

    def commit(self):
        self.git("add", ".")
        self.git("-c", "user.name=Match Gate Fixture", "-c", "user.email=fixture@example.test",
                 "commit", "-qm", "Synthetic fixture")

    def gate(self, head):
        return subprocess.run([sys.executable, str(self.tools / GATE.name), head],
                              cwd=self.root, text=True, capture_output=True, timeout=10)

    def test_refuses_a_different_head(self):
        result = self.gate("0" * len(self.head))
        self.assertEqual(result.returncode, 1)
        self.assertIn("does not match the requested commit", result.stderr)
        self.assertFalse(list((self.root / ".git").glob("lbpmatchgate-*.log")))

    def test_refuses_dirty_checkout(self):
        for kind in ("untracked", "unstaged", "staged"):
            with self.subTest(kind=kind):
                path = self.root / "scratch" if kind == "untracked" else self.tools / "lbpmatch.py"
                original = path.read_bytes() if path.exists() else None
                path.write_text("print('dirty')\n", encoding="utf-8")
                if kind == "staged":
                    self.git("add", str(path))
                result = self.gate(self.head)
                self.assertEqual(result.returncode, 1)
                self.assertIn("checkout has staged, unstaged, or untracked changes", result.stderr)
                if original is None:
                    path.unlink()
                else:
                    path.write_bytes(original)
                    self.git("add", str(path))

    def finished_log(self, head):
        result = self.gate(head)
        self.assertEqual(result.returncode, 0, result.stderr)
        log = Path(result.stdout.split("Log: ", 1)[1].strip())
        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            text = log.read_text(encoding="utf-8")
            if text.endswith(f" {head}\n") and text.splitlines()[-1].startswith("DONE "):
                return text
            time.sleep(0.02)
        self.fail("detached worker did not finish")

    def test_clean_head_produces_a_completion_log(self):
        text = self.finished_log(self.head)
        self.assertIn(f"COMMIT {self.head}", text)
        self.assertIn("PASS lbpmatch.py (exit 0)", text)
        self.assertIn("PASS lbpreport.py (exit 0)", text)
        self.assertTrue(text.endswith(f"DONE PASS {self.head}\n"))
        self.assertEqual(self.git("status", "--porcelain"), "")

    def test_failed_match_still_reports_progress_and_finishes(self):
        (self.tools / "lbpmatch.py").write_text("raise SystemExit(3)\n", encoding="utf-8")
        self.commit()
        head = self.git("rev-parse", "HEAD")
        text = self.finished_log(head)
        self.assertIn("FAIL lbpmatch.py (exit 3)", text)
        self.assertIn("PASS lbpreport.py (exit 0)", text)
        self.assertTrue(text.endswith(f"DONE FAIL {head}\n"))


    def test_private_tree_checks_proofs_and_coverage(self):
        (self.root / "data/public-figures.json").unlink()
        (self.tools / "lbpproof.py").write_text(
            "import sys; assert sys.argv[1:] == ['check']\n", encoding="utf-8",
        )
        (self.tools / "lbpcov.py").write_text(
            "import sys; assert sys.argv[1:] == ['--proofs']\n", encoding="utf-8",
        )
        self.commit()
        head = self.git("rev-parse", "HEAD")
        text = self.finished_log(head)
        self.assertIn("PASS lbpproof.py (exit 0)", text)
        self.assertIn("PASS lbpcov.py (exit 0)", text)
        self.assertTrue(text.endswith(f"DONE PASS {head}\n"))

    def test_a_checkout_modified_during_checks_cannot_pass(self):
        (self.tools / "lbpmatch.py").write_text("open('scratch', 'w').close()\n", encoding="utf-8")
        self.commit()
        head = self.git("rev-parse", "HEAD")
        text = self.finished_log(head)
        self.assertIn("FAIL checkout: checkout has staged, unstaged, or untracked changes", text)
        self.assertTrue(text.endswith(f"DONE FAIL {head}\n"))


if __name__ == "__main__":
    unittest.main()
