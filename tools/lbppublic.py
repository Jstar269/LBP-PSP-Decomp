#!/usr/bin/env python3
"""Maintainer tool: keep the public repository in step with this private one (the hst-decomp model, hstpublic.py).

    python tools/lbppublic.py sync --repo <public repository> -m <commit message>
    python tools/lbppublic.py check --repo <public repository> [--commit REV]

The public repository is a SEPARATE git repository (its own history, no link to this one; this repository has no
remote and its pre-push hook refuses every push). It only ever receives what tools/lbppublish.py copies.

`sync` builds the filtered tree with lbppublish.py into a temporary directory, audits it (lbppublish.py --audit),
mirrors it into the public repository's work tree (adding, replacing and deleting tracked files; `.git` and ignored
files are left alone), runs `check`, and commits there when anything changed. It never pushes: pushing is a separate,
deliberate step of the owner's publishing procedure (AGENTS.md).

`check` refuses a tree whose sources (src/, wip/) are not all listed in its own ORIGINS.json with a publishable class,
or whose ORIGINS.json lists an excluded class, a tree that holds a file the policy keeps out (lbppublish.py
PRIVATE_PATHS and DISC_DERIVED, except the public README.md and AGENTS.md: `sync` stages the whole work tree, untracked
files included), and a commit message with a private path or key-like text (lbppublish.py PRIVATE_TEXT,
PERSONAL_TEXT, KEY_TEXT), as hst-decomp's tools/hstpublic.py does; with --commit it checks that commit of the public repository. A pre-push hook
in the public repository can run it on every pushed commit.
"""
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbppublish

ROOT = lbppublish.ROOT


def git(cwd: Path, *args: str, text: bool = True, check: bool = True) -> str:
    return subprocess.run(["git", *args], cwd=cwd, check=check, capture_output=True, text=text).stdout


def mirror(src: Path, dst: Path) -> None:
    """Make dst's tracked files equal to src's files; untracked and ignored files in dst are left alone."""
    want = {p.relative_to(src).as_posix() for p in src.rglob("*") if p.is_file()}
    for rel in git(dst, "ls-files").splitlines():
        if rel not in want and (dst / rel).is_file():
            (dst / rel).unlink()
            parent = (dst / rel).parent
            while parent != dst and parent.is_dir() and not any(parent.iterdir()):
                parent.rmdir()
                parent = parent.parent
    for rel in sorted(want):
        (dst / rel).parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(src / rel, dst / rel)


def check_tree(files: list[str], origins: dict) -> list[str]:
    bad = [f"{f}: excluded by the policy (lbppublish.py PRIVATE_PATHS or DISC_DERIVED)" for f in files
           if (lbppublish.matches(f, lbppublish.PRIVATE_PATHS) or lbppublish.matches(f, lbppublish.DISC_DERIVED))
           and f not in lbppublish.PUBLIC_FILES.values()]
    for f in files:
        if f.split("/")[0] in ("src", "wip") and f.endswith((".c", ".cpp")):
            cls = origins.get(f)
            if cls is None or cls in lbppublish.EXCLUDED_CLASSES:
                bad.append(f"{f}: {cls or 'not in ORIGINS.json'}")
    bad += [f"ORIGINS.json lists {p} as {c}" for p, c in origins.items() if c in lbppublish.EXCLUDED_CLASSES]
    return bad


def message_problems(message: str) -> list[str]:
    """The lines of a commit message with a private path or key-like text (as lbppublish.py --audit judges a line)."""
    bad = []
    for n, line in enumerate(message.splitlines(), 1):
        key = lbppublish.KEY_TEXT.search(line) and not lbppublish.DIGEST_CONTEXT.search(line)
        if lbppublish.PRIVATE_TEXT.search(line) or lbppublish.PERSONAL_TEXT.search(line) or key:
            bad.append(f"commit message line {n}: private path or key-like text")
    return bad


def check(repo: Path, commit: str | None, message: str | None = None) -> int:
    if commit is None:
        files = git(repo, "ls-files").splitlines()
        origins = json.loads((repo / "ORIGINS.json").read_text(encoding="utf-8"))
    else:
        files = git(repo, "ls-tree", "-r", "--name-only", commit).splitlines()
        origins = json.loads(git(repo, "show", f"{commit}:ORIGINS.json"))
        message = git(repo, "log", "-1", "--format=%B", commit)
    bad = check_tree(files, origins) + message_problems(message or "")
    for b in bad[:20]:
        print(f"REFUSED {b}", file=sys.stderr)
    if bad:
        print(f"REFUSED: {len(bad)} non-publishable entr(y/ies)", file=sys.stderr)
        return 1
    print(f"check: {len(files)} files, every source publishable")
    return 0


def sync(repo: Path, message: str) -> int:
    if not (repo / ".git").exists():
        raise SystemExit(f"{repo} is not a git repository")
    if repo.resolve() == ROOT.resolve() or ROOT.resolve() in repo.resolve().parents:
        raise SystemExit("the public repository must be outside this one")
    with tempfile.TemporaryDirectory() as tmp:
        out = Path(tmp) / "tree"
        subprocess.run([sys.executable, str(ROOT / "tools" / "lbppublish.py"), "--out", str(out)], check=True,
                       stdout=subprocess.DEVNULL)
        if subprocess.run([sys.executable, str(ROOT / "tools" / "lbppublish.py"), "--audit", str(out)]).returncode:
            raise SystemExit("the filtered tree fails the audit: nothing synced")
        mirror(out, repo)
    git(repo, "add", "-A")
    if not git(repo, "status", "--porcelain").strip():
        print("public repository already up to date")
        return 0
    if check(repo, None, message):
        return 1
    subprocess.run(["git", "commit", "-q", "-F", "-"], cwd=repo, input=message, text=True, check=True)
    print(git(repo, "log", "-1", "--oneline").strip())
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("sync")
    s.add_argument("--repo", type=Path, required=True)
    s.add_argument("-m", "--message", required=True)
    c = sub.add_parser("check")
    c.add_argument("--repo", type=Path, required=True)
    c.add_argument("--commit")
    args = ap.parse_args()
    if args.cmd == "sync":
        return sync(args.repo.resolve(), args.message)
    return check(args.repo.resolve(), args.commit)


if __name__ == "__main__":
    raise SystemExit(main())
