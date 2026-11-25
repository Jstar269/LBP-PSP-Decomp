#!/usr/bin/env python3
"""Run the full match and progress checks detached at a clean pull-request commit."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT, text=True).strip()


def verify_checkout(expected: str) -> None:
    head = git("rev-parse", "HEAD")
    if head != expected:
        raise ValueError(f"HEAD {head} does not match the requested commit {expected}")
    if git("status", "--porcelain=v1", "--untracked-files=all", "--ignore-submodules=none"):
        raise ValueError("checkout has staged, unstaged, or untracked changes")


def run_checks(expected: str) -> int:
    print(f"COMMIT {expected}", flush=True)
    checks = [("lbpmatch.py", "--all", "--parallel", "4")]
    if (ROOT / "data/public-figures.json").is_file():
        checks.append(("lbpreport.py", "--check"))
    else:
        checks.extend([("lbpproof.py", "check"), ("lbpcov.py", "--proofs")])

    status = "FAIL"
    try:
        verify_checkout(expected)
        passed = True
        for tool, *args in checks:
            try:
                code = subprocess.run([sys.executable, "-u", f"tools/{tool}", *args], cwd=ROOT).returncode
            except OSError as error:
                print(f"FAIL {tool}: {error}", flush=True)
                passed = False
            else:
                print(f"{'PASS' if code == 0 else 'FAIL'} {tool} (exit {code})", flush=True)
                passed &= code == 0
        verify_checkout(expected)
        status = "PASS" if passed else "FAIL"
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        print(f"FAIL checkout: {error}", flush=True)
    finally:
        print(f"DONE {status} {expected}", flush=True)
    return 0 if status == "PASS" else 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("expected_head", help="full SHA of the pushed pull-request head")
    parser.add_argument("--worker", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args()
    if not re.fullmatch(r"[0-9a-f]{40}|[0-9a-f]{64}", args.expected_head):
        parser.error("expected_head must be a full lowercase Git commit SHA")
    if args.worker:
        return run_checks(args.expected_head)

    try:
        verify_checkout(args.expected_head)
        log_dir = Path(git("rev-parse", "--absolute-git-dir"))
        options = {"creationflags": subprocess.DETACHED_PROCESS} if os.name == "nt" else {"start_new_session": True}
        with tempfile.NamedTemporaryFile(
            mode="w", prefix="lbpmatchgate-", suffix=".log", dir=log_dir, delete=False,
        ) as log:
            worker = subprocess.Popen(
                [sys.executable, "-u", str(Path(__file__).resolve()), args.expected_head, "--worker"],
                cwd=ROOT, stdin=subprocess.DEVNULL, stdout=log, stderr=subprocess.STDOUT, **options,
            )
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        print(f"REFUSED: {error}", file=sys.stderr)
        return 1

    print(f"STARTED {args.expected_head} (PID {worker.pid})")
    print(f"Log: {log.name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
