#!/usr/bin/env python3
"""Cross-title SDK fingerprints: a catalogue of the accepted matches' masked fingerprints, and a lookup that proposes
clones in this title from another PSP decomp's catalogue (Hot Shots Tennis and LittleBigPlanet link the same Sony SDK
libraries, so a function matched in one is often byte-identical apart from relocations in the other).

    python tools/lbpfingerprint.py export [--out FILE] [--module MODULE ...]
    python tools/lbpfingerprint.py lookup CATALOGUE [CATALOGUE ...] [--module MODULE ...] [--min-instructions 4]
                                          [--all] [--json FILE]

`export` reads the accepted ledger (tools/lbpproof.py; maintainers' tree) and, for each proof, the retail bytes of its
claimed range from your campaign, and writes one entry per proof: instruction count, relocation-masked SHA-256, the
source's path, its licence (the effective REUSE.toml annotation) and, for orientation, its module, start and compiler
profile. The masking is tools/lbpclone.py's `analyse` (call targets, `lui` immediates and the low halves that use them
masked), the same signature `lbpclone.py` and `lbpfamilies.py` use inside this title, so in-title and cross-title
fingerprints agree. The format is documented in docs/GUIDE.md ("SDK fingerprint catalogue") and is shared with
hst-decomp's tools/hstfingerprint.py: either project's export can be read by the other's lookup. (When hst-decomp's
tree and campaign are on this machine, tools/lbpxclone.py ports and verifies its sources directly; a catalogue needs
neither the other project's tree nor its game.)

`lookup` computes the same fingerprint for every function of this title with a usable boundary (as lbpclone.py) that
no source covers yet (`--all`: every such function) and prints each one whose instruction count and masked SHA-256
equal a catalogue entry.

The catalogue lists identities only and authorises nothing. A proposal is a lead: the source it names still has to
be ported with this title's addresses and pass `python tools/lbpmatch.py` (EXACT) and the fake-match gate
(tools/lbpgate.py) before `tools/lbppromote.py` accepts it; the exemplar's licence travels with any code reused.

Fingerprints derive from retail code, so a catalogue never enters the public tree: the default output is under the
gitignored try/, `--out`/`--json` refuse a path inside the repository that git does not ignore, `.gitignore` ignores
`sdk-fingerprints-*.json`, and tools/lbpprcheck.py refuses a catalogue in a pull request.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
import tomllib
from collections import defaultdict
from pathlib import Path
from typing import Callable, Iterable

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C  # noqa: E402
import lbpmatch as M  # noqa: E402

ROOT = M.ROOT
PROJECT = "lbp-psp-decomp"
# Shared with hst-decomp (tools/hstfingerprint.py): change both together, and the version with any change.
FORMAT = "psp-decomp.sdk-fingerprints.v1"
MASKING = "clone-analyse.v1"  # lbpclone.analyse == lbpclone.analyse
DEFAULT_OUT = ROOT / "try" / f"sdk-fingerprints-{PROJECT}.json"
MIN_INSTRUCTIONS = 4  # shorter functions (stubs, `jr ra`) collide by chance
ENTRY_KEYS = ("instructions", "masked_sha256", "source", "licence", "module", "start", "cc")
SHA256 = re.compile(r"[0-9a-f]{64}")


# --------------------------------------------------------------------------------------------------------------
# fingerprints

def signature_sha256(sig: Iterable[int]) -> str:
    """SHA-256 over the masked signature, each word as four little-endian bytes, in order."""
    return hashlib.sha256(b"".join((w & 0xFFFFFFFF).to_bytes(4, "little") for w in sig)).hexdigest()


def fingerprint(code: bytes) -> tuple[int, str]:
    """(instruction count, relocation-masked SHA-256) of one function's code."""
    sig, _refs = C.analyse(C.words(code))
    return len(sig), signature_sha256(sig)


# --------------------------------------------------------------------------------------------------------------
# licences (REUSE.toml: the last annotation whose path glob matches is the effective one, as tools/lbpreuse.py check)

def licence_lookup(reuse_toml: Path = ROOT / "REUSE.toml") -> Callable[[str], str]:
    import lbpreuse as R
    config = tomllib.loads(reuse_toml.read_text(encoding="utf-8"))
    compiled = []
    for a in config.get("annotations", []):
        paths = [a["path"]] if isinstance(a["path"], str) else a["path"]
        compiled.append(([R.glob_regex(p) for p in paths], a.get("SPDX-License-Identifier", "NOASSERTION")))

    def licence(path: str) -> str:
        found = "NOASSERTION"
        for patterns, identifier in compiled:
            if any(p.match(path) for p in patterns):
                found = identifier
        return found
    return licence


# --------------------------------------------------------------------------------------------------------------
# the catalogue

def build_catalogue(proofs: Iterable[dict], code_of: Callable[[str, int, int], bytes],
                    licence_of: Callable[[str], str], project: str = PROJECT) -> dict:
    entries = []
    for p in proofs:
        if p.get("status", "EXACT") != "EXACT":
            continue
        start, end = int(p["start"], 16), int(p["end"], 16)
        n, sha = fingerprint(code_of(p["module"], start, end))
        if not n:
            continue
        entries.append({"instructions": n, "masked_sha256": sha, "source": p["source"],
                        "licence": licence_of(p["source"]), "module": p["module"], "start": f"0x{start:08x}",
                        "cc": p.get("cc", "")})
    entries.sort(key=lambda e: (e["masked_sha256"], e["instructions"], e["source"]))
    return {"format": FORMAT, "masking": MASKING, "project": project, "entries": entries}


def validate_catalogue(cat: object, where: str = "catalogue") -> dict:
    if not isinstance(cat, dict) or cat.get("format") != FORMAT:
        raise ValueError(f"{where}: not a {FORMAT} catalogue")
    if cat.get("masking") != MASKING:
        raise ValueError(f"{where}: masking {cat.get('masking')!r}, this tool computes {MASKING!r}")
    if not isinstance(cat.get("project"), str) or not isinstance(cat.get("entries"), list):
        raise ValueError(f"{where}: needs a project name and an entries list")
    for i, e in enumerate(cat["entries"]):
        if not isinstance(e, dict) or any(k not in e for k in ENTRY_KEYS):
            raise ValueError(f"{where}: entry {i} lacks one of {', '.join(ENTRY_KEYS)}")
        if not isinstance(e["instructions"], int) or e["instructions"] <= 0 or isinstance(e["instructions"], bool):
            raise ValueError(f"{where}: entry {i} has a bad instruction count")
        if not isinstance(e["masked_sha256"], str) or not SHA256.fullmatch(e["masked_sha256"]):
            raise ValueError(f"{where}: entry {i} has a bad masked_sha256")
    return cat


def read_catalogue(path: Path) -> dict:
    return validate_catalogue(json.loads(path.read_text(encoding="utf-8")), str(path))


def propose(targets: Iterable[tuple[str, int, int, tuple]], catalogues: Iterable[dict],
            min_instructions: int = MIN_INSTRUCTIONS, covered: set[tuple[str, int]] | None = None) -> list[dict]:
    """targets: (module, start, end, masked signature) of this title's functions. Every one whose fingerprint equals a
    catalogue entry gives one proposal per entry; `covered` marks the ones a source already covers."""
    index: dict[tuple[int, str], list[dict]] = defaultdict(list)
    for cat in catalogues:
        for e in cat["entries"]:
            index[(e["instructions"], e["masked_sha256"])].append({**e, "project": cat["project"]})
    out = []
    for module, start, end, sig in targets:
        if len(sig) < min_instructions:
            continue
        hits = index.get((len(sig), signature_sha256(sig)))
        for e in hits or ():
            out.append({"module": module, "start": f"0x{start:08x}", "end": f"0x{end:08x}",
                        "instructions": len(sig), "masked_sha256": e["masked_sha256"],
                        "covered": bool(covered and (module, start) in covered),
                        "exemplar": {k: e[k] for k in ("project", "source", "licence", "module", "start", "cc")}})
    out.sort(key=lambda r: (r["module"], r["start"], r["exemplar"]["project"], r["exemplar"]["source"]))
    return out


# --------------------------------------------------------------------------------------------------------------
# where a catalogue may be written

def private_path(path: Path, root: Path = ROOT) -> bool:
    """True when `path` is outside the repository, or inside it and ignored by git (so it cannot be committed by
    accident). A tracked file is never private."""
    try:
        rel = path.resolve().relative_to(root.resolve())
    except ValueError:
        return True
    try:
        r = subprocess.run(["git", "check-ignore", "-q", rel.as_posix()], cwd=root, capture_output=True)
    except OSError:
        return False
    return r.returncode == 0


def require_private(path: Path, root: Path = ROOT) -> None:
    if not private_path(path, root):
        raise SystemExit(f"{path}: inside the repository and not ignored by git; fingerprints derive from retail code "
                         "and stay out of the tree (use try/ or a path outside the repository)")


def write_private(path: Path, obj: object, root: Path = ROOT) -> None:
    require_private(path, root)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(obj, indent=1) + "\n", encoding="utf-8", newline="\n")


# --------------------------------------------------------------------------------------------------------------
# commands

def export_command(args) -> int:
    import lbpproof
    require_private(args.out)
    if not lbpproof.ledger_present():
        raise SystemExit("no accepted ledger (data/accepted-ledger.jsonl): export runs in the maintainers' tree")
    proofs = [p for p in lbpproof.accepted_proofs() if not args.module or p["module"] in args.module]
    cache: dict = {}

    def code_of(module: str, start: int, end: int) -> bytes:
        data, seg = M.module_bytes(cache, module)
        off = seg["file_offset"] + start - seg["start"]
        return data[off:off + end - start]
    cat = build_catalogue(proofs, code_of, licence_lookup())
    write_private(args.out, cat)
    print(f"{FORMAT}: {len(cat['entries'])} fingerprints from {len(proofs)} accepted proofs -> {args.out}")
    return 0


def lookup_command(args) -> int:
    if args.json:
        require_private(args.json)
    catalogues = [read_catalogue(p) for p in args.catalogues]
    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    modules = args.module or [m["name"] for m in ledger["modules"]]
    covered = set()
    for p in sorted((ROOT / "src").rglob("*")):
        if p.suffix in (".c", ".cpp"):
            h = M.parse_header(p)
            covered.add((h["module"], h["start"]))
    targets = []
    for module in modules:
        for start, (end, (sig, _refs)) in C.load_functions(ledger, module).items():
            if args.all or (module, start) not in covered:
                targets.append((module, start, end, sig))
    rows = propose(targets, catalogues, args.min_instructions, covered)
    for r in rows:
        e = r["exemplar"]
        mark = " (covered)" if r["covered"] else ""
        print(f"PROPOSE {r['module']} {r['start']} {r['instructions']:5d} insns{mark}  <- {e['project']} {e['source']} "
              f"[{e['licence']}; cc={e['cc'] or '?'}]")
    entries = sum(len(c["entries"]) for c in catalogues)
    print(f"targets={len(targets)} catalogue_entries={entries} proposals={len(rows)} "
          f"functions={len({(r['module'], r['start']) for r in rows})}")
    print("Proposals only: port the named source and verify it with tools/lbpmatch.py (EXACT) and the fake-match gate "
          "before tools/lbppromote.py accepts it.")
    if args.json:
        write_private(args.json, {"format": FORMAT + ".proposals", "proposals": rows})
    return 0


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)
    ex = sub.add_parser("export", help="write this title's catalogue from the accepted ledger")
    ex.add_argument("--out", type=Path, default=DEFAULT_OUT, help=f"default {DEFAULT_OUT.relative_to(ROOT).as_posix()}")
    ex.add_argument("--module", action="append", help="only proofs of this module (repeatable)")
    lk = sub.add_parser("lookup", help="propose clones in this title from catalogues")
    lk.add_argument("catalogues", nargs="+", type=Path)
    lk.add_argument("--module", action="append", help="only this module's functions (repeatable)")
    lk.add_argument("--min-instructions", type=int, default=MIN_INSTRUCTIONS)
    lk.add_argument("--all", action="store_true", help="also functions a source already covers")
    lk.add_argument("--json", type=Path, help="also write the proposals here (outside the tree or gitignored)")
    args = ap.parse_args(argv)
    return export_command(args) if args.command == "export" else lookup_command(args)


if __name__ == "__main__":
    raise SystemExit(main())
