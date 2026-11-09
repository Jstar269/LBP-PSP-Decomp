#!/usr/bin/env python3
"""Re-test parked near misses under every locked compiler profile (a compiler-profile sweep).

    python tools/lbpsweep.py [--parallel 3] [--unlocked] [--lane NAME]

Seeds: every .c/.cpp under the tree's wip/ (recursive) and the top-level wip/ of every lane under ext_root (a
directory with LANE.json), read with lbplane.wip_text, for functions src/ does not cover; a seed whose header has
`extra=` or `helpers=`, an unknown cc= or a header that is not one closed first line is skipped. Seeds are deduped by
the SHA-1 of the text with the header's cc= removed and the trailing `//` lines dropped (the first one found wins:
the tree's wip/ before the lanes, each in path order).

Measure: each (seed, profile) is written to try/sweep/src/<profile>/<key>/<original name> with only cc= changed and
compiled by tools/lbpmatch.py --quiet --json in chunks of 40, building into try/sweep/build (LBPMATCH_BUILD_ROOT).
The seed's own cc= (the primary profile without one) is measured first; the other locked GCC profiles of
data/toolchain-manifest.json only for seeds that linked (a status that is not a compile, link or placement failure).
With --unlocked the unlocked GCC profiles are measured too: an EXACT there is a lead, never installed.

Cache: try/sweep/cache.json, keyed by (text key, profile, core_digest of the lock); a re-run compiles only new or
changed pairs. A result that is about the machine, not the source (COMPILE_CRASHED, no result row), is not cached.

--lane NAME: for every function with an EXACT under a locked profile one file is picked (the seed's own cc= first,
then the primary profile, then the manifest's lock order; the tree's seeds before the lanes') and written to
<ext_root>/NAME/src/eboot/<original name> with the trailing `//` lines removed, cc= set and a true provenance note
on line 2; plus LANE.json (kind sweep), the batch try/agent-queue/NAME-01.json (in the main tree and in the lane) and
PROGRESS.md EXACT lines. The harvest is not run: the command is printed.

Prints one JSON summary on stdout: seeds, skipped, pairs compiled, cache hits, EXACT per profile, picks, leads.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbplane as L
import lbpmatch as M
from lbplint import NOTE_KINDS, provenance_note

ROOT = M.ROOT
SWEEP = ROOT / "try" / "sweep"
MANIFEST = ROOT / "data" / "toolchain-manifest.json"
CHUNK = 40
CACHE_SCHEMA = "lbp-decomp.sweep-cache.v1"
KNOWN_PROFILES = M.GCC_PROFILE_NAMES | M.SNC_PROFILE_NAMES | {"mwcc219"}
# lbpmatch.py statuses of a source that did not compile, link or get placed: no comparison happened
NOT_LINKED = frozenset({"COMPILE_FAILED", "COMPILE_CRASHED", "LINK_FAILED", "MISSING_SYMBOLS", "UNPLACED_FUNCTION",
                        "UNPLACED_SECTION", "NO_SYMBOL", "NO_RESULT"})
# results about the machine rather than the source: measured again next time
UNCACHED = frozenset({"COMPILE_CRASHED", "NO_RESULT"})
CC_KEY = re.compile(r'(?<![\w-])cc=("[^"]*"|\S+)')
HEADER_FIELDS = re.compile(r'(\w+)=("[^"]*"|\S+)')
ORIGIN_TRAILER = re.compile(r"^//\s*(?:near miss from|improved by) lane ([\w.-]+)")
ORIGIN_NOTE = re.compile(r"\((?:external matching )?lane ([\w.-]+)")
# a clause of a provenance note that described the near miss (false once a profile makes it EXACT)
STALE = re.compile(r"\bnear\b|near[- ]miss|\bnot\b.*\b(?:exact|verified)\b|mismatch|exactness|exact match verified"
                   r"|\bwords?\b.*\b(?:off|differ)|\bdiffer|trial candidate|exploratory|scratch|see trailing notes"
                   r"|preliminary|unreviewed|parked", re.I)
REVIEW = re.compile(r"names and types await human review\.?\s*")

# runner(files, build_root, parallel, json_out) -> {resolved path: lbpmatch.py --json row}; tests replace it
Runner = Callable[[list[Path], Path, int, Path], dict]


# ---------------------------------------------------------------- text

def strip_trailing_comments(text: str) -> str:
    """The text without its trailing blank and `//` lines (notes that describe a near miss), ending in one newline."""
    lines = text.replace("\r\n", "\n").rstrip("\n").split("\n")
    while lines and (not lines[-1].strip() or lines[-1].lstrip().startswith("//")):
        lines.pop()
    return "\n".join(lines) + "\n"


def trailing_comments(text: str) -> list[str]:
    lines = text.replace("\r\n", "\n").rstrip("\n").split("\n")
    out = []
    while lines and (not lines[-1].strip() or lines[-1].lstrip().startswith("//")):
        ln = lines.pop()
        if ln.strip():
            out.insert(0, ln.strip())
    return out


def header_of(text: str) -> dict | None:
    """The LBPMATCH fields of a text whose first line is the whole header (as lbpmatch.parse_header reads them), or
    None when the first line is not a closed header or lacks module/start/end/symbol."""
    first = text.split("\n", 1)[0]
    if not first.startswith("/* LBPMATCH "):
        return None
    m = M.HEADER.match(first)
    if not m:
        return None
    fields = {k: v.strip('"') for k, v in HEADER_FIELDS.findall(m.group(1))}
    try:
        fields["start"] = int(fields["start"], 16)
        fields["end"] = int(fields["end"], 16)
    except (KeyError, ValueError):
        return None
    if "module" not in fields or "symbol" not in fields:
        return None
    return fields


def text_key(text: str) -> str:
    """SHA-1 of the text with the header's cc= removed and the trailing `//` lines dropped: two seeds that differ
    only in their profile or their near-miss notes are one seed."""
    body = strip_trailing_comments(text)
    first, sep, rest = body.partition("\n")
    first = re.sub(r"\s+", " ", CC_KEY.sub("", first))
    return hashlib.sha1((first + sep + rest).encode("utf-8")).hexdigest()


def set_cc(text: str, profile: str) -> str:
    """The text with only the header's cc= set to `profile` (added before the header's `*/` when it has none)."""
    first, sep, rest = text.partition("\n")
    if CC_KEY.search(first):
        first = CC_KEY.sub(f"cc={profile}", first)
    else:
        i = first.rindex("*/")
        first = first[:i].rstrip() + f" cc={profile} " + first[i:]
    return first + sep + rest


def dedupe_header(first: str) -> str:
    """The header line without a repeated identical key=value token."""
    seen, out = set(), []
    for tok in first.split(" "):
        if "=" in tok and tok in seen:
            continue
        seen.add(tok)
        out.append(tok)
    return " ".join(out)


def origin_lane(text: str, found_in: str = "") -> str:
    """The lane that wrote a seed: its harvest trailer (`// near miss from lane X`), else a `(lane X` in its note,
    else the lane directory it was found in ("" when none is known)."""
    for ln in trailing_comments(text):
        m = ORIGIN_TRAILER.match(ln)
        if m:
            return m.group(1)
    lines = text.split("\n")
    if len(lines) > 1:
        m = ORIGIN_NOTE.search(lines[1])
        if m:
            return m.group(1)
    return found_in


def sweep_phrase(profile: str, lane: str) -> str:
    return f"exact match verified under cc={profile} after a compiler-profile sweep (lane {lane}, tools/lbpsweep.py)"


def standard_note(profile: str, lane: str, orig: str) -> str:
    where = f" (external matching lane {orig})" if orig else ""
    return f"/* Hand-written reconstruction{where}; {sweep_phrase(profile, lane)}; names and types await human review. */"


def rewrite_note(text: str, profile: str, lane: str, orig: str) -> str:
    """The text with a true provenance note on line 2. A one-line note of a kind lbplint knows keeps its kind clause
    (and a description after its review clause); the clauses that described the near miss are dropped and the sweep's
    verdict added. A one-line comment of no known kind that describes the near miss is replaced by the standard
    hand-written note; otherwise the standard note is inserted after the header."""
    lines = text.split("\n")
    second = lines[1] if len(lines) > 1 else ""
    m = re.fullmatch(r"/\*(.*)\*/", second.strip())
    if m and "*/" not in m.group(1):
        body = m.group(1).strip()
        tail = ""
        r = REVIEW.search(body)
        if r:
            body, tail = body[:r.start()], body[r.end():].strip()
        clauses = [c.strip() for c in body.split(";") if c.strip()]
        head = clauses[0] if clauses else ""
        if ":" in head:
            kind_part, detail = head.split(":", 1)
            if STALE.search(detail) or not detail.strip():
                head = kind_part.strip()
        if any(rx.search(head) for _k, rx in NOTE_KINDS) and not STALE.search(head):
            kept = [c.rstrip(".") for c in clauses[1:] if not STALE.search(c)]
            parts = [head.rstrip(".")] + kept + [sweep_phrase(profile, lane), "names and types await human review."]
            note = "; ".join(parts) + (f" {tail}" if tail and not STALE.search(tail) else "")
            lines[1] = f"/* {note} */"
            return "\n".join(lines)
        if STALE.search(m.group(1)):
            lines[1] = standard_note(profile, lane, orig)
            return "\n".join(lines)
    elif second.lstrip().startswith("/*") and provenance_note(text)[0]:
        return text   # a multi-line note of a known kind (a library source's): left as it is
    lines.insert(1, standard_note(profile, lane, orig))
    return "\n".join(lines)


def installed_text(text: str, profile: str, lane: str, orig: str) -> str:
    """A seed as a lane source: trailing `//` lines removed, the header deduped with cc= set, a true note."""
    body = strip_trailing_comments(set_cc(text, profile))
    first, sep, rest = body.partition("\n")
    return rewrite_note(dedupe_header(first) + sep + rest, profile, lane, orig)


# ---------------------------------------------------------------- seeds

def seed_files(tree: Path, lanes: list[Path]) -> list[tuple[Path, str]]:
    """(path, lane it was found in) of every candidate file: the tree's wip/ recursively, then each lane's top-level
    wip/ ("" for the tree)."""
    out = [(p, "") for p in sorted((tree / "wip").rglob("*")) if p.is_file() and p.suffix in (".c", ".cpp")]
    for d in lanes:
        out += [(p, d.name) for p in sorted((d / "wip").glob("*")) if p.is_file() and p.suffix in (".c", ".cpp")]
    return out


def discover_seeds(tree: Path, lanes: list[Path], covered: set[tuple[str, int]], primary: str) -> tuple[list[dict], Counter]:
    """The deduped seeds and a count of the files skipped by reason."""
    seeds: list[dict] = []
    seen: set[str] = set()
    skipped: Counter = Counter()
    for p, lane in seed_files(tree, lanes):
        text = L.wip_text(p)
        h = header_of(text)
        if h is None:
            skipped["bad_header"] += 1
            continue
        if (h["module"], h["start"]) in covered:
            skipped["covered"] += 1
            continue
        if "extra" in h or "helpers" in h:
            skipped["extra_or_helpers"] += 1
            continue
        cc = h.get("cc", primary)
        if cc not in KNOWN_PROFILES:
            skipped["unknown_cc"] += 1
            continue
        key = text_key(text)
        if key in seen:
            skipped["duplicate"] += 1
            continue
        seen.add(key)
        seeds.append({"path": p, "name": p.name, "lane": lane, "text": text, "key": key, "cc": cc,
                      "module": h["module"], "start": h["start"], "end": h["end"], "symbol": h["symbol"],
                      "origin": origin_lane(text, lane)})
    return seeds, skipped


# ---------------------------------------------------------------- profiles, cache, picking

def lock_profiles(manifest: dict) -> tuple[list[str], str, str]:
    """(locked profile names in the manifest's order, primary profile, core_digest)."""
    core = manifest["core"]
    return list(core["profiles"]), core.get("primary", M.PRIMARY_PROFILE), manifest["core_digest"]


def sweep_profiles(locked: list[str], unlocked: bool) -> tuple[list[str], list[str]]:
    """(locked GCC profiles in lock order, unlocked GCC profiles sorted; empty unless asked for)."""
    gcc = [p for p in locked if p in M.GCC_PROFILE_NAMES]
    return gcc, (sorted(M.GCC_PROFILE_NAMES - set(locked)) if unlocked else [])


def cache_key(key: str, profile: str, digest: str) -> str:
    return f"{key}|{profile}|{digest}"


def cacheable(row: dict) -> bool:
    return row.get("status") not in UNCACHED


def linked(status: str) -> bool:
    return status not in NOT_LINKED


def chunks(seq: list, n: int = CHUNK) -> list[list]:
    return [seq[i:i + n] for i in range(0, len(seq), n)]


def try_path(sweep: Path, profile: str, seed: dict) -> Path:
    return sweep / "src" / profile / seed["key"][:12] / seed["name"]


def rank(own: str, profile: str, primary: str, order: list[str]) -> tuple:
    """Pick order of an EXACT profile: the seed's own cc=, then the primary profile, then the lock order."""
    if profile == own:
        return (0, 0)
    if profile == primary:
        return (1, 0)
    return (2, order.index(profile) if profile in order else len(order))


def pick(seeds: list[dict], results: dict, locked: list[str], primary: str) -> dict[tuple[str, int], tuple[dict, str]]:
    """Per (module, start) with an EXACT under a locked profile: (seed, profile), by rank, then seed order."""
    best: dict[tuple[str, int], tuple[tuple, dict, str]] = {}
    for i, s in enumerate(seeds):
        for prof, row in results.get(s["key"], {}).items():
            if row.get("status") != "EXACT" or prof not in locked:
                continue
            r = rank(s["cc"], prof, primary, locked) + (i,)
            a = (s["module"], s["start"])
            if a not in best or r < best[a][0]:
                best[a] = (r, s, prof)
    return {a: (s, p) for a, (_r, s, p) in best.items()}


def leads(seeds: list[dict], results: dict, locked: list[str], picked: dict) -> list[dict]:
    """EXACT results under unlocked profiles for functions with no locked EXACT."""
    out = []
    for s in seeds:
        if (s["module"], s["start"]) in picked:
            continue
        for prof, row in sorted(results.get(s["key"], {}).items()):
            if row.get("status") == "EXACT" and prof not in locked:
                out.append({"module": s["module"], "address": f"0x{s['start']:08x}", "profile": prof,
                            "seed": seed_rel(s)})
    return out


def seed_rel(s: dict) -> str:
    p = s["path"]
    try:
        return p.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return p.as_posix()


def load_cache(path: Path) -> dict:
    if path.exists():
        try:
            doc = json.loads(path.read_text(encoding="utf-8"))
            if doc.get("schema") == CACHE_SCHEMA:
                return doc.get("entries", {})
        except ValueError:
            pass
    return {}


def save_cache(path: Path, entries: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(".tmp")
    tmp.write_text(json.dumps({"schema": CACHE_SCHEMA, "entries": entries}, indent=0, sort_keys=True) + "\n",
                   encoding="utf-8")
    os.replace(tmp, path)


# ---------------------------------------------------------------- measuring

def run_lbpmatch(files: list[Path], build_root: Path, parallel: int, out: Path) -> dict:
    """tools/lbpmatch.py --quiet --json over `files` (one chunk), building into `build_root`; rows by resolved path."""
    env = dict(os.environ, LBPMATCH_BUILD_ROOT=str(build_root))
    out.unlink(missing_ok=True)
    cmd = [sys.executable, str(Path(M.__file__)), "--quiet", "--parallel", str(parallel), "--json", str(out)]
    subprocess.run(cmd + [str(f) for f in files], stdout=subprocess.DEVNULL, env=env)
    rows = {}
    if out.exists():
        for r in json.loads(out.read_text(encoding="utf-8")):
            rows[(ROOT / r["source"]).resolve()] = r
    return rows


def measure(pairs: list[tuple[dict, str]], sweep: Path, digest: str, cache: dict, results: dict, parallel: int,
            runner: Runner, cache_path: Path | None, stats: Counter) -> None:
    """Fill results[key][profile] for every (seed, profile) pair: from the cache, or compiled in chunks."""
    todo: list[tuple[dict, str, Path]] = []
    for s, prof in pairs:
        ck = cache_key(s["key"], prof, digest)
        if ck in cache:
            results.setdefault(s["key"], {})[prof] = cache[ck]
            stats["cache_hits"] += 1
            continue
        f = try_path(sweep, prof, s)
        f.parent.mkdir(parents=True, exist_ok=True)
        f.write_text(set_cc(s["text"], prof), encoding="utf-8", newline="\n")
        todo.append((s, prof, f))
    for n, part in enumerate(chunks(todo)):
        rows = runner([f for _s, _p, f in part], sweep / "build", parallel, sweep / f"chunk-{n}.json")
        for s, prof, f in part:
            row = rows.get(f.resolve()) or {"status": "NO_RESULT", "words_differ": None}
            res = {"status": row["status"], "words_differ": row.get("words_differ")}
            results.setdefault(s["key"], {})[prof] = res
            stats["pairs_compiled"] += 1
            if cacheable(res):
                cache[cache_key(s["key"], prof, digest)] = res
        if cache_path is not None:
            save_cache(cache_path, cache)


def sweep(seeds: list[dict], gcc_locked: list[str], unlocked: list[str], digest: str, cache: dict, parallel: int,
          runner: Runner = run_lbpmatch, sweep_dir: Path = SWEEP, cache_path: Path | None = None) -> tuple[dict, Counter]:
    """Baseline (each seed's own cc=), then the other locked GCC profiles (and the unlocked ones when given) for the
    seeds that linked. Returns (results[key][profile] -> row, stats)."""
    results: dict = {}
    stats: Counter = Counter()
    measure([(s, s["cc"]) for s in seeds], sweep_dir, digest, cache, results, parallel, runner, cache_path, stats)
    rest = [(s, p) for s in seeds if linked(results[s["key"]][s["cc"]]["status"])
            for p in gcc_locked + unlocked if p != s["cc"]]
    measure(rest, sweep_dir, digest, cache, results, parallel, runner, cache_path, stats)
    return results, stats


# ---------------------------------------------------------------- lane

def batch_items(picks: list[tuple[dict, str]], batches: list[Path]) -> list[dict]:
    """One batch item per pick: copied from the newest earlier batch that carried the address (`batches` oldest
    first), else made from the header."""
    want = {s["start"] for s, _p in picks}
    found: dict[int, dict] = {}
    for b in batches:
        try:
            doc = json.loads(b.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            continue
        for it in doc.get("items", []):
            try:
                a = int(it["address"], 16)
            except (KeyError, TypeError, ValueError):
                continue
            if a in want:
                found[a] = it
    out = []
    for s, prof in sorted(picks, key=lambda sp: sp[0]["start"]):
        it = found.get(s["start"]) or {"address": f"0x{s['start']:08x}", "end": f"0x{s['end']:08x}",
                                       "size": s["end"] - s["start"], "family": [], "ghidra_name": s["symbol"],
                                       "cc_hint": prof}
        if s["module"] != "EBOOT.BIN":
            it = dict(it, module=s["module"])
        out.append(it)
    return out


def install_lane(name: str, picks: list[tuple[dict, str]], lane_dir: Path, main: Path, batches: list[Path],
                 head: str, made: str) -> dict:
    """Write the lane: src/eboot/<name> per pick, LANE.json, the batch (main and lane try/agent-queue/) and
    PROGRESS.md. A pick whose file name is already used (in the lane or in the main src/eboot/) is left out."""
    if not re.fullmatch(r"[A-Za-z0-9][\w.-]*", name):
        raise SystemExit(f"bad lane name {name!r} (letters, digits, . _ -)")
    if lane_dir.exists():
        raise SystemExit(f"{lane_dir} exists")
    out_dir = lane_dir / "src" / "eboot"
    out_dir.mkdir(parents=True)
    used: set[str] = set()
    written, taken, progress = [], [], []
    for s, prof in sorted(picks, key=lambda sp: (sp[0]["module"], sp[0]["start"])):
        if s["name"] in used or (main / "src" / "eboot" / s["name"]).exists():
            taken.append(seed_rel(s))
            continue
        used.add(s["name"])
        (out_dir / s["name"]).write_text(installed_text(s["text"], prof, name, s["origin"]), encoding="utf-8",
                                         newline="\n")
        written.append((s, prof))
        progress.append(f"0x{s['start']:08x} EXACT src/eboot/{s['name']}")
    items = batch_items(written, batches)
    batch_name = f"{name}-01.json"
    doc = {"module": "EBOOT.BIN", "kind": "sweep", "name": f"{name}-01", "made": made, "items": items}
    for q in (main / "try" / "agent-queue", lane_dir / "try" / "agent-queue"):
        q.mkdir(parents=True, exist_ok=True)
        (q / batch_name).write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    (lane_dir / "LANE.json").write_text(json.dumps({
        "lane": name, "batch": batch_name, "kind": "sweep", "items": len(items), "main": str(main),
        "main_head": head, "made": made}, indent=1) + "\n", encoding="utf-8")
    (lane_dir / "PROGRESS.md").write_text("\n".join(progress) + ("\n" if progress else ""), encoding="utf-8",
                                          newline="\n")
    return {"lane": str(lane_dir), "installed": len(written), "name_taken": taken,
            "harvest": f"python tools/lbplane.py harvest {name}"}


def known_batches(ext: Path | None, main: Path) -> list[Path]:
    paths = list((main / "try" / "agent-queue").glob("*.json"))
    if ext is not None and ext.exists():
        paths += list(ext.glob("*/try/agent-queue/*.json"))
    return sorted(paths, key=lambda p: p.stat().st_mtime)


# ---------------------------------------------------------------- main

def summary(seeds: list[dict], skipped: Counter, stats: Counter, results: dict, picked: dict, lead_rows: list) -> dict:
    per_profile: Counter = Counter()
    for s in seeds:
        for prof, row in results.get(s["key"], {}).items():
            if row.get("status") == "EXACT":
                per_profile[prof] += 1
    return {"seeds": len(seeds), "skipped": dict(sorted(skipped.items())),
            "pairs_compiled": stats["pairs_compiled"], "cache_hits": stats["cache_hits"],
            "exact_per_profile": dict(sorted(per_profile.items())), "exact_functions": len(picked),
            "picks": [{"module": a[0], "address": f"0x{a[1]:08x}", "profile": p, "seed": seed_rel(s)}
                      for a, (s, p) in sorted(picked.items())],
            "leads": lead_rows}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--parallel", type=int, default=3, help="lbpmatch.py --parallel (AGENTS.md: 4 at most)")
    ap.add_argument("--unlocked", action="store_true", help="also measure the unlocked GCC profiles (leads only)")
    ap.add_argument("--lane", metavar="NAME", help="write the EXACT picks as a sweep lane under ext_root")
    args = ap.parse_args()
    ext = L.ext_root() if L.CFG.get("ext_root") else None
    if args.lane and ext is None:
        raise SystemExit("--lane needs ext_root (lbp-config.json)")
    if args.lane and (ext / args.lane).exists():
        raise SystemExit(f"{ext / args.lane} exists")
    locked, primary, digest = lock_profiles(json.loads(MANIFEST.read_text(encoding="utf-8")))
    gcc_locked, unlocked = sweep_profiles(locked, args.unlocked)
    lanes = sorted(p for p in ext.glob("*") if (p / "LANE.json").exists()) if ext is not None and ext.exists() else []
    seeds, skipped = discover_seeds(ROOT, lanes, L.covered(ROOT), primary)
    cache_path = SWEEP / "cache.json"
    cache = load_cache(cache_path)
    results, stats = sweep(seeds, gcc_locked, unlocked, digest, cache, args.parallel, cache_path=cache_path)
    picked = pick(seeds, results, locked, primary)
    out = summary(seeds, skipped, stats, results, picked, leads(seeds, results, locked, picked))
    if args.lane:
        made = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
        out["lane"] = install_lane(args.lane, list(picked.values()), ext / args.lane, ROOT, known_batches(ext, ROOT),
                                   L.git_head(ROOT), made)
        print(f"not harvested; run: {out['lane']['harvest']}", file=sys.stderr)
    print(json.dumps(out, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
