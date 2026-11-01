#!/usr/bin/env python3
"""Checks for a contribution: what a pull request adds must be source, never game data or private material.

    python tools/lbpprcheck.py [--base origin/main] [--repo .]

Compares the commits of the branch (`git diff <base>...HEAD`) with the base and refuses, for each file the branch
adds or changes:

  binary          a binary file (git sees it as binary, or it holds a NUL byte), or a name the ignore rules keep
                  out for that reason (retail modules, objects, images of discs, archives, compilers, key files)
  place           a path that is never versioned: build/, try/, keys/, inputs/, campaign/, Ghidra projects and caches, lbp-config.json,
                  the disc-derived files tools/lbppublish.py keeps private (data/functions-*.txt and the rest)
  size            a changed file over 1 MiB (sources are a few kilobytes; nothing a contribution needs is that big)
  private         an added line with a personal or secret value: a home directory path (C:\\Users\\..., /home/...,
                  /mnt/c/Users/...), a private key block, a GitHub, OpenAI or AWS token
  dump            an added file with more than 16 lines that look like a disassembly listing (an address, an
                  instruction word, a mnemonic): disassembly of the game is never published
  symbols         a deleted or changed line in symbols/ (the symbol maps are append-only)
  header          a source in src/ or wip/ whose first line is not an LBPMATCH header with module, start, end and
                  symbol, or whose second line is not one of the provenance notes CONTRIBUTING.md lists
  gate            a source the fake-match gate refuses (tools/lbpgate.py: asm that emits instructions, data
                  directives, code-emitting pragmas, extra= under src/)

It needs no game, compiler or configuration, so it runs in CI on every pull request and before you open one. It is a
first filter: the maintainer still reviews every change and re-verifies every source with tools/lbpmatch.py before
it is accepted. Exit status 1 when anything is refused.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpgate
import lbplint
import lbppublish

MATCH = "LBPMATCH"
CONFIG = "lbp-config.json"
MAX_BYTES = 1 << 20
BINARY_SUFFIXES = {".o", ".obj", ".elf", ".bin", ".prx", ".pbp", ".iso", ".cso", ".exe", ".dll", ".so", ".a", ".zip",
                   ".7z", ".gz", ".tar", ".gzf", ".rep", ".img", ".dat", ".sfo", ".at3", ".pmf"}
NEVER = (re.compile(r"^(build|try|keys|inputs|campaign|analysis)/"), re.compile(r"^ghidra/(projects|out|cache|settings)/"),
         re.compile(r"(^|/)[^/]*keyfile[^/]*$", re.I), re.compile(rf"^{re.escape(CONFIG)}$"))
PRIVATE = re.compile(r"[A-Za-z]:[\\/]+Users[\\/]+(?!<)\w|/home/(?!<|user\b)\w|/mnt/[a-z]/Users/(?!<)\w|"
                     r"-----BEGIN [A-Z ]*PRIVATE KEY-----|\bghp_[A-Za-z0-9]{20}|\bgithub_pat_\w{20}|"
                     r"\bsk-[A-Za-z0-9]{20}|\bAKIA[0-9A-Z]{16}\b")
DUMP_LINE = re.compile(r"^\W*(0x)?[0-9a-fA-F]{6,8}:?\s+(0x)?[0-9a-fA-F]{8}\s+[a-z][a-z0-9.]{1,9}\b")
DUMP_LIMIT = 16
HEADER = re.compile(rf"^/\* {MATCH} (?=.*\bmodule=\S)(?=.*\bstart=0x[0-9a-fA-F]+)(?=.*\bend=0x[0-9a-fA-F]+)"
                    r"(?=.*\bsymbol=\S).*\*/\s*$")


def git(repo: Path, *args: str) -> str:
    return subprocess.run(["git", *args], cwd=repo, check=True, capture_output=True, text=True,
                          encoding="utf-8", errors="replace").stdout


def changed_files(repo: Path, base: str) -> list[tuple[str, str]]:
    """(status, path) of every file the branch adds or modifies relative to the merge base (deletions are fine)."""
    out = git(repo, "diff", "--name-status", "--no-renames", "--diff-filter=AM", f"{base}...HEAD")
    return [tuple(line.split("\t", 1)) for line in out.splitlines() if line.strip()]


def binary_paths(repo: Path, base: str) -> set[str]:
    out = git(repo, "diff", "--numstat", "--no-renames", "--diff-filter=AM", f"{base}...HEAD")
    return {line.split("\t", 2)[2] for line in out.splitlines() if line.startswith("-\t-\t")}


def added_lines(repo: Path, base: str, path: str) -> list[str]:
    out = git(repo, "diff", "--no-renames", "-U0", f"{base}...HEAD", "--", path)
    return [l[1:] for l in out.splitlines() if l.startswith("+") and not l.startswith("+++")]


def removed_lines(repo: Path, base: str, path: str) -> list[str]:
    out = git(repo, "diff", "--no-renames", "-U0", f"{base}...HEAD", "--", path)
    return [l[1:] for l in out.splitlines() if l.startswith("-") and not l.startswith("---")]


def check(repo: Path, base: str) -> list[str]:
    refusals: list[str] = []
    binaries = binary_paths(repo, base)
    for status, path in changed_files(repo, base):
        file = repo / path
        if Path(path).suffix.lower() in BINARY_SUFFIXES or path in binaries:
            refusals.append(f"binary   {path}: binary files and game data are never added")
            continue
        if any(p.search(path) for p in NEVER) or path in lbppublish.DISC_DERIVED:
            refusals.append(f"place    {path}: this path is never versioned")
            continue
        data = file.read_bytes() if file.is_file() else b""
        if len(data) > MAX_BYTES:
            refusals.append(f"size     {path}: {len(data):,} bytes (limit {MAX_BYTES:,})")
            continue
        if b"\0" in data:
            refusals.append(f"binary   {path}: holds NUL bytes")
            continue
        added = added_lines(repo, base, path)
        for line in added:
            if PRIVATE.search(line):
                refusals.append(f"private  {path}: an added line holds a personal path or a secret: "
                                f"{line.strip()[:80]}")
                break
        dump = sum(1 for line in added if DUMP_LINE.match(line))
        if dump > DUMP_LIMIT:
            refusals.append(f"dump     {path}: {dump} added lines look like a disassembly listing")
        if path.startswith("symbols/") and status == "M" and removed_lines(repo, base, path):
            refusals.append(f"symbols  {path}: existing lines changed or deleted (append only)")
        if re.match(r"^(src|wip)/.*\.(c|cpp)$", path):
            lines = data.decode("utf-8", "replace").split("\n", 2)
            if not HEADER.match(lines[0]):
                refusals.append(f"header   {path}: line 1 is not a {MATCH} header with module, start, end, symbol")
            elif len(lines) < 2 or not any(rx.search(lines[1]) for _kind, rx in lbplint.NOTE_KINDS):
                refusals.append(f"header   {path}: line 2 is not a provenance note CONTRIBUTING.md lists")
            found = lbpgate.refusals(lbpgate.check_file(file, repo))
            if found:
                refusals.append(f"gate     {path}: " + "; ".join(str(f) for f in found)[:200])
    return refusals


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", default="origin/main", help="the branch the pull request goes into (default origin/main)")
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[1],
                    help="the checkout to check (default: this one)")
    args = ap.parse_args()
    repo = args.repo.resolve()
    try:
        git(repo, "rev-parse", "--verify", "--quiet", f"{args.base}^{{commit}}")
    except subprocess.CalledProcessError:
        raise SystemExit(f"no commit {args.base!r} to compare with: fetch it first (git fetch origin main), or pass "
                         "--base with the branch your pull request goes into")
    files = changed_files(repo, args.base)
    refusals = check(repo, args.base)
    for r in refusals:
        print("REFUSED " + r)
    print(f"{len(files)} added or changed file(s) against {args.base}: "
          + ("OK" if not refusals else f"{len(refusals)} refusal(s)"))
    return 1 if refusals else 0


if __name__ == "__main__":
    raise SystemExit(main())
