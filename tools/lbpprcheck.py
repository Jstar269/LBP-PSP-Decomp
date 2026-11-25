#!/usr/bin/env python3
"""Checks for a contribution: what a pull request adds must be source, never game data or private material.

    python tools/lbpprcheck.py [--base origin/main] [--repo .]

Compares the commits of the branch (`git diff <base>...HEAD`) with the base and refuses, for each file the branch
adds or changes:

  binary          a binary file (git sees it as binary, or it holds a NUL byte), or a name the ignore rules keep
                  out for that reason (retail modules, objects, images of discs, archives, compilers, key files)
  place           a path that is never versioned: build/, try/, keys/, inputs/, campaign/, Ghidra projects and caches, lbp-config.json,
                  the disc-derived files tools/lbppublish.py keeps private (DISC_DERIVED); also a change to the
                  published function boundary list (FUNCTION_LISTS: the maintainers decide boundaries) or the
                  translation-unit map (UNIT_MAPS, tools/lbptu.py); and any path the base branch's .gitignore
                  rules keep out (added by force: maintainer-only notes, other modules' function lists, ...)
  size            a changed file over 1 MiB (sources are a few kilobytes; nothing a contribution needs is that big); the
                  generated objdiff report (docs/objdiff-report.json, one unit per function) has a 32 MiB limit
  report          that report when its units do not add up to its module figures (tools/lbpreport.py unit_problems)
                  or it is not, byte for byte, the report tools/lbpreport.py makes from the tree's
                  data/public-figures.json, data/public-functions.txt and data/translation-units.json
  private         an added line with a personal or secret value: a home directory path (C:\\Users\\..., /home/...,
                  /mnt/c/Users/...), a private key block, a GitHub, OpenAI or AWS token
  dump            an added file with more than 16 lines that look like a disassembly listing (an address, an
                  instruction word, a mnemonic): disassembly of the game is never published
  symbols         a deleted or changed line in symbols/ (the symbol maps are append-only), or a map that no longer
                  loads: a line that is not `NAME = 0xADDRESS;` or a name bound to two addresses (tools/lbpmatch.py
                  load_symbols), or an address name (FUN_/DAT_/LAB_/PTR_/jtbl_xxxxxxxx) bound to another address
  header          a source in src/ or wip/ whose first line is not an LBPMATCH header with module, start, end and
                  symbol, or that repeats the header further down; a source in src/ without one of the provenance
                  notes CONTRIBUTING.md lists right after the header (tools/lbplint.py provenance_note: one-line
                  annotations may come first). A near miss in wip/ needs the header only (CONTRIBUTING.md, "Near
                  misses"), as hst-decomp's tools/hstprcheck.py
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
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpgate
import lbplint
import lbppublish

MATCH = "LBPMATCH"
CONFIG = "lbp-config.json"
MAX_BYTES = 1 << 20
# The objdiff progress report (tools/lbpreport.py) lists every function of the public scope, one unit per line, so it is
# several MiB. It has its own limit, and it must still add up: its units, to the module figures.
OBJDIFF_REPORT = "docs/objdiff-report.json"
REPORT_MAX_BYTES = 32 << 20
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


SYMBOL_LINE = re.compile(r"(\S+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;?")
ADDRESS_NAME = re.compile(r"(?:FUN|DAT|LAB|PTR|jtbl)_([0-9a-fA-F]{8})")


def symbol_problems(text: str) -> list[str]:
    """Why a symbol map would not load in tools/lbpmatch.py load_symbols, or binds an address name to an address other
    than its own ([] when neither)."""
    seen: dict[str, int] = {}
    out = []
    for n, raw in enumerate(text.splitlines(), 1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        m = SYMBOL_LINE.fullmatch(line)
        if not m:
            out.append(f"line {n} is not `NAME = 0xADDRESS;`: {line[:60]}")
            continue
        name, value = m.group(1), int(m.group(2), 16)
        named = ADDRESS_NAME.fullmatch(name)
        if named and int(named.group(1), 16) != value:
            out.append(f"line {n}: {name} = 0x{value:08x} contradicts the address its name carries")
        if seen.setdefault(name, value) != value:
            out.append(f"line {n}: {name} is already bound to 0x{seen[name]:08x}")
    return out


def git(repo: Path, *args: str) -> str:
    # --literal-pathspecs: a path given after `--` names that file only (a name such as `:x` or `[ab].c` is no
    # pathspec magic or glob that matches another file or none). Decoded here, not by text=True: its universal
    # newlines would turn a carriage return inside a line into a newline
    return subprocess.run(["git", "--literal-pathspecs", *args], cwd=repo, check=True,
                          capture_output=True).stdout.decode("utf-8", "replace")


def git_z(repo: Path, *args: str) -> list[str]:
    """The NUL-separated fields of a `-z` git command. Paths come raw (not C-quoted as git quotes names with non-ASCII
    bytes, quotes, backslashes or control characters), decoded so that `repo / path` is the file itself."""
    out = subprocess.run(["git", "--literal-pathspecs", *args], cwd=repo, check=True, capture_output=True).stdout
    return [f.decode("utf-8", "surrogateescape") for f in out.split(b"\0")][:-1] if out else []


def changed_files(repo: Path, base: str, kinds: str = "AMT") -> list[tuple[str, str]]:
    """(status, path) of every file the branch adds, modifies or changes the type of (a file made a symbolic link or
    a submodule: T) relative to the merge base; with kinds="D", the files it deletes."""
    fields = git_z(repo, "diff", "-z", "--name-status", "--no-renames", f"--diff-filter={kinds}", f"{base}...HEAD")
    return list(zip(fields[0::2], fields[1::2]))


def binary_paths(repo: Path, base: str) -> set[str]:
    fields = git_z(repo, "diff", "-z", "--numstat", "--no-renames", "--diff-filter=AMT", f"{base}...HEAD")
    return {f.split("\t", 2)[2] for f in fields if f.startswith("-\t-\t")}


HUNK = re.compile(r"^@@ -\d+(?:,(\d+))? \+\d+(?:,(\d+))? @@")


def diff_lines(repo: Path, base: str, path: str) -> tuple[list[str], list[str]]:
    """(added, removed) lines of one file. Read by the counts of each hunk header, so a line whose text starts with
    `++` or `--` is a line like any other (not a `+++`/`---` file header), and split at newlines only (a carriage
    return, form feed or U+2028 inside a line does not end it)."""
    out = git(repo, "diff", "--no-color", "--no-ext-diff", "--no-renames", "-U0", f"{base}...HEAD", "--", path)
    added: list[str] = []
    removed: list[str] = []
    lines = out.split("\n")
    i = 0
    while i < len(lines):
        m = HUNK.match(lines[i])
        i += 1
        if not m:
            continue
        old, new = int(m.group(1) or 1), int(m.group(2) or 1)
        while (old or new) and i < len(lines):
            line = lines[i]
            i += 1
            if line.startswith("-") and old:
                removed.append(line[1:])
                old -= 1
            elif line.startswith("+") and new:
                added.append(line[1:])
                new -= 1
            elif not line.startswith("\\"):        # "\ No newline at end of file" is no line of the file
                break
    return added, removed


def ignored_paths(repo: Path, base: str, paths: list[str]) -> set[str]:
    """The paths that the base branch's ignore rules (its .gitignore files) keep out: they were added by force. The
    base's rules, not the branch's, so a pull request cannot drop a rule for the files it adds; checked in an empty
    repository, so no local or global exclude file of this machine counts."""
    tree = subprocess.run(["git", "ls-tree", "-r", "-z", "--name-only", base], cwd=repo, check=True,
                          capture_output=True).stdout.split(b"\0")
    rules = [n for n in tree if n == b".gitignore" or n.endswith(b"/.gitignore")]
    if not rules or not paths:
        return set()
    with tempfile.TemporaryDirectory() as tmp:
        subprocess.run(["git", "init", "-q", tmp], check=True, capture_output=True)
        for name in rules:
            blob = subprocess.run(["git", "cat-file", "blob", f"{base}:{name.decode('utf-8', 'surrogateescape')}"],
                                  cwd=repo, check=True, capture_output=True).stdout
            dest = Path(tmp, name.decode("utf-8", "surrogateescape"))
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(blob)
        (Path(tmp) / ".git" / "info" / "exclude").write_bytes(b"")
        out = subprocess.run(["git", "-c", f"core.excludesFile={Path(tmp, '.git', 'info', 'exclude')}",
                              "check-ignore", "--no-index", "-z", "--stdin"], cwd=tmp, capture_output=True,
                             input="\0".join(paths).encode("utf-8", "surrogateescape")).stdout
    return {f.decode("utf-8", "surrogateescape") for f in out.split(b"\0") if f}


def added_lines(repo: Path, base: str, path: str) -> list[str]:
    return diff_lines(repo, base, path)[0]


def removed_lines(repo: Path, base: str, path: str) -> list[str]:
    return diff_lines(repo, base, path)[1]


def report_problems(data: bytes) -> list[str]:
    """Why an objdiff report is not one tools/lbpreport.py writes ([] when its units add up to its modules)."""
    import json

    import lbpreport
    try:
        return lbpreport.unit_problems(json.loads(data.decode("utf-8")))
    except (ValueError, KeyError, TypeError, AttributeError) as exc:
        return [f"not an objdiff report tools/lbpreport.py writes ({type(exc).__name__})"]


def report_differences(repo: Path) -> list[str]:
    """Why the tree's objdiff report is not the one tools/lbpreport.py makes from the tree's figures, rows and units
    ([] when it is byte for byte): the report is a pure function of them, so it carries nothing else."""
    import lbpreport
    figures = repo / lbpreport.FIGURES_REL
    if not figures.is_file():
        return [f"no {lbpreport.FIGURES_REL} to make it from"]
    try:
        return lbpreport.differences(lbpreport.load_figures(figures), repo / OBJDIFF_REPORT,
                                     lbpreport.load_rows(repo / lbpreport.ROWS_REL),
                                     lbpreport.read_units(repo / lbpreport.TU_REL))
    except (ValueError, KeyError, TypeError, AttributeError) as exc:
        return [f"the figures do not make a report ({type(exc).__name__}: {str(exc)[:100]})"]


def check(repo: Path, base: str) -> list[str]:
    refusals: list[str] = []
    binaries = binary_paths(repo, base)
    for _, path in changed_files(repo, base, "D"):
        # deleting a whole file deletes its lines: append-only and the maintainers' lists hold for that too
        if path.startswith("symbols/"):
            refusals.append(f"symbols  {path}: deleted (the symbol maps are append only)")
        elif path in lbppublish.FUNCTION_LISTS or path in lbppublish.UNIT_MAPS:
            refusals.append(f"place    {path}: deleted (the maintainers' list)")
    changed = changed_files(repo, base)
    ignored = ignored_paths(repo, base, [path for _, path in changed])
    for status, path in changed:
        file = repo / path
        if Path(path).suffix.lower() in BINARY_SUFFIXES or path in binaries:
            refusals.append(f"binary   {path}: binary files and game data are never added")
            continue
        if any(p.search(path) for p in NEVER) or path in lbppublish.DISC_DERIVED:
            refusals.append(f"place    {path}: this path is never versioned")
            continue
        if path in ignored:
            refusals.append(f"place    {path}: the ignore rules (.gitignore) keep this path out; it was added by force")
            continue
        if path in lbppublish.FUNCTION_LISTS:
            refusals.append(f"place    {path}: the function boundary list is the maintainers' (a list you make "
                            "from your own disc stays local)")
            continue
        if path in lbppublish.UNIT_MAPS:
            refusals.append(f"place    {path}: the translation-unit map is the maintainers' (tools/lbptu.py makes it "
                            "from the disc)")
            continue
        data = file.read_bytes() if file.is_file() else b""
        if path == OBJDIFF_REPORT:
            if len(data) > REPORT_MAX_BYTES:
                refusals.append(f"size     {path}: {len(data):,} bytes (limit {REPORT_MAX_BYTES:,})")
                continue
            problems = report_problems(data) or report_differences(repo)
            if problems:
                refusals.append(f"report   {path}: {problems[0]}")
        elif len(data) > MAX_BYTES:
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
        if path.startswith("symbols/") and status == "T":
            refusals.append(f"symbols  {path}: no longer a regular file (append only)")
        elif path.startswith("symbols/") and status == "M" and removed_lines(repo, base, path):
            refusals.append(f"symbols  {path}: existing lines changed or deleted (append only)")
        elif path.startswith("symbols/") and path.endswith(".txt"):
            problems = symbol_problems(data.decode("utf-8", "replace"))
            if problems:
                refusals.append(f"symbols  {path}: the map no longer loads: {problems[0]}")
        if re.match(r"(?s)^(src|wip)/.*\.(c|cpp)$", path):
            text = data.decode("utf-8", "replace")
            lines = text.split("\n")
            if not HEADER.match(lines[0]):
                refusals.append(f"header   {path}: line 1 is not a {MATCH} header with module, start, end, symbol")
            elif any(ln.lstrip().startswith(f"/* {MATCH} ") for ln in lines[1:]):
                refusals.append(f"header   {path}: a second {MATCH} header after line 1")
            elif path.startswith("src/") and lbplint.provenance_note(text)[0] is None:
                refusals.append(f"header   {path}: no provenance note CONTRIBUTING.md lists right after the header")
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
    sys.stdout.reconfigure(errors="backslashreplace")      # a path that is not UTF-8 is still printed
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
