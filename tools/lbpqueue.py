#!/usr/bin/env python3
"""Build hand-matching batches for matching agents (lanes: tools/lbplane.py).

    python tools/lbpqueue.py --name lbp1 --kind families|near|singles|library [--module EBOOT.BIN]
                             [--batches 1] [--per-batch 25] [--min-size 32] [--max-size 1024]
                             [--include-snc] [--exclude 0xADDR,...] [--dry-run]

Writes try/agent-queue/<name>-NN.json (NN from 01), each {"module", "kind", "name", "made", "items": [...]}; an item
has address, end, size, family (the other members' starts), pseudocode (absolute path of the Ghidra export, advisory),
ghidra_name, symbol (a non-address name from symbols/ when one exists), best_generated (the lbpgen candidate with the
fewest differing words: source relative to the main tree, tag, differing_words, words), flags, cc_hint, library and
wip (existing near-miss files) when known.

Kinds (each ranks the functions that are not covered by src/, not reserved and not attempted, see below):

  families  one representative per unmatched clone family (lbpclone.py's masked code, >= 2 members, none matched),
            total family bytes first: one EXACT unlocks every member through lbpclone.py
  near      functions with an lbpgen candidate within 35% of its words (MISMATCH, linked), or a wip/ file; one
            per family (fewest words left); ranked by code bytes unlocked (family included) per differing word
  singles   functions in no family and with no near candidate, ranked by expected bytes per lane minute
            (single_weight: size / (1 + (size/192)^2), highest for 100-350 B), then those with an lbpgen start point
  library   functions identified as library code (try/lib-hits*.json: a newlib/zlib object of the same masked
            shape) or carrying a library name in symbols/, one per family, smallest first
  readability  MATCHED sources the public tree would carry (tools/lbppublish.py policy) that are not clones
            themselves, most readability debt first (tools/lbpreadable.py score x (1 + its clones); score 0 left out); an item names its
            `source` and the number of its `clones` (re-derived from it after a rewrite: lbpclone.py --refresh).
            For readability lanes (docs/READABILITY-BRIEF.md, harvested with lbplane.py rewrite-harvest)

Flags from the retail words (for the lane and the ranking): `ins_zero` counts `ins rt,$zero,pos,n` (bit-field stores
of zero and contiguous clear masks such as `x & ~7`; the primary compiler emits both since the ins-from-zero change of
2026-10-28, docs/TOOLCHAIN.md), `vfpu` counts VFPU instructions (inline VFPU asm, asm=vfpu rules), `unaligned`
counts lwl/lwr/swl/swr (assembled since the -mulsw change of 2026-10-29), `div` counts integer div/divu (GCC's
`bnez; div; break 7` guard; lbpgen.py drops Ghidra's `trap(7)` for it since 2026-11-09), `snc` marks the SNC-built library (cc_hint
snc127-O2; try/snc-functions.txt). VFPU functions sort last; SNC functions only with --include-snc. `--flag NAME`
keeps only functions with that flag, and offers attempted ones again (lanes skipped them before the change).

Reserved / attempted (so batches made at different times stay disjoint): every address of a batch under
try/agent-queue/ that no lane took yet, and of a lane under ext_root that is not harvested (no try/harvest-<lane>/),
is reserved, and so is every --exclude address (and its family). In a harvested lane, an address with a PROGRESS.md
line (EXACT, NEAR or SKIP) is attempted and not offered again (a NEAR comes back through its wip/ file in the near
kind); an address the lane never reached is free.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C
import lbpconfig as CFG
import lbpmatch as M
import lbpproject as P

QUEUE = M.ROOT / "try" / "agent-queue"
GHIDRA = M.ROOT / "ghidra" / "out"
NEAR_RATIO = 0.35
LIBRARY_NAME = re.compile(r"^(?:_Z|__|_[a-z]|inflate|deflate|adler32|crc32|zcalloc|zcfree|mem|str|mb|wc|_?[a-z]+_r$)")
VFPU_OPS = {0x18, 0x19, 0x1B, 0x32, 0x34, 0x35, 0x36, 0x37, 0x3A, 0x3C, 0x3D, 0x3E, 0x3F}
UNALIGNED_OPS = {0x22, 0x26, 0x2A, 0x2E}
PROGRESS = re.compile(r"^\s*(0x[0-9A-Fa-f]{6,8})\s+(EXACT|NEAR|SKIP)\b")


def flags(ws: list[int]) -> dict:
    """ins_zero: `ins rt,$zero,pos,n` (SPECIAL3 0x1f, function 0x04, rs = $zero); vfpu: VFPU opcodes and COP2 moves
    (mfv/mtv/bvf, opcode 0x12); unaligned: lwl/lwr/swl/swr (opcodes 0x22, 0x26, 0x2a, 0x2e); div: div/divu (SPECIAL
    functions 0x1a, 0x1b)."""
    ins0 = sum(1 for w in ws if w >> 26 == 0x1F and w & 0x3F == 0x04 and (w >> 21) & 31 == 0)
    vfpu = sum(1 for w in ws if w >> 26 in VFPU_OPS or w >> 26 == 0x12)
    unaligned = sum(1 for w in ws if w >> 26 in UNALIGNED_OPS)
    div = sum(1 for w in ws if w >> 26 == 0 and w & 0x3F in (0x1A, 0x1B))
    return {"ins_zero": ins0, "vfpu": vfpu, "unaligned": unaligned, "div": div}


def best_generated(root: Path) -> dict[int, dict]:
    """Per start, the lbpgen candidate (try/gen-<tag>/summary-*.json) that linked with the fewest differing words."""
    best: dict[int, dict] = {}
    for f in sorted(root.glob("try/gen-*/summary-*.json")):
        tag = f.parent.name[len("gen-"):]
        try:
            rows = json.loads(f.read_text(encoding="utf-8"))
        except ValueError:
            continue
        for r in rows:
            if r.get("status") != "MISMATCH" or r.get("words_differ") is None:
                continue
            m = re.match(r"(\d+)/(\d+) words", r.get("detail", ""))
            words = int(m.group(2)) if m else None
            start = int(r["start"], 16)
            cur = best.get(start)
            if cur is None or r["words_differ"] < cur["differing_words"]:
                best[start] = {"source": r["source"], "tag": tag, "differing_words": r["words_differ"], "words": words}
    return best


def pseudocode(root: Path, module_dir: str) -> dict[int, tuple[str, str]]:
    out: dict[int, tuple[str, str]] = {}
    for d in sorted(root.glob(f"{module_dir}*")):
        idx = d / "functions.jsonl"
        if not idx.exists():
            continue
        for line in idx.read_text(encoding="utf-8").splitlines():
            row = json.loads(line)
            if row.get("pseudocode_status") == "decompiled" and row.get("pseudocode_file"):
                out.setdefault(int(row["address"], 16), (row["name"], str((d / row["pseudocode_file"]).resolve())))
    return out


def library_hits(root: Path) -> dict[int, list[dict]]:
    out: dict[int, list[dict]] = {}
    for f in sorted(root.glob("try/lib-hits*.json")):
        if ".before-" in f.name:
            continue
        try:
            rows = json.loads(f.read_text(encoding="utf-8"))
        except ValueError:
            continue
        for r in rows:
            start = int(r["start"], 16)
            seen = {(c["function"], c["object"]) for c in out.get(start, [])}
            for c in r.get("candidates", [])[:6]:
                if (c["function"], c["object"]) not in seen:
                    out.setdefault(start, []).append({"function": c["function"], "object": c["object"],
                                                      "config": c["config"]})
                    seen.add((c["function"], c["object"]))
    return out


def wip_leads(root: Path) -> dict[tuple[str, int], list[str]]:
    out: dict[tuple[str, int], list[str]] = defaultdict(list)
    for p in sorted((root / "wip").rglob("*")):
        if p.suffix not in (".c", ".cpp"):
            continue
        try:
            h = M.parse_header(p)
        except ValueError:
            continue
        out[(h["module"], h["start"])].append(p.relative_to(root).as_posix())
    return out


def lane_dirs() -> list[Path]:
    if CFG.get("ext_root") is None:
        return []
    root = CFG.host_path("ext_root")
    return sorted(p for p in root.glob("*") if (p / "LANE.json").exists()) if root.exists() else []


def batch_addresses(path: Path) -> set[int]:
    try:
        doc = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return set()
    return {int(it["address"], 16) for it in doc.get("items", [])}


def reservations(root: Path, lanes: list[Path], queue: Path) -> tuple[set[int], set[int]]:
    """(reserved, attempted) addresses, as the module docstring defines them."""
    reserved, attempted, taken = set(), set(), set()
    for d in lanes:
        info = json.loads((d / "LANE.json").read_text(encoding="utf-8"))
        taken.add(info.get("batch"))
        addrs = set().union(*(batch_addresses(b) for b in (d / "try" / "agent-queue").glob("*.json")))
        if not (root / "try" / f"harvest-{d.name}").exists():
            reserved |= addrs
            continue
        prog = d / "PROGRESS.md"
        if prog.exists():
            for line in prog.read_text(encoding="utf-8", errors="replace").splitlines():
                m = PROGRESS.match(line)
                if m:
                    attempted.add(int(m.group(1), 16))
    for b in sorted(queue.glob("*.json")):
        if b.name not in taken:
            reserved |= batch_addresses(b)
    return reserved, attempted


def symbol_names(module: str) -> dict[int, str]:
    out: dict[int, str] = {}
    for name, addr in M.load_symbols(module).items():
        if M.default_address(name) is None:
            out.setdefault(addr, name)
    return out


def snc_starts(root: Path) -> set[int]:
    f = root / "try" / "snc-functions.txt"
    if not f.exists():
        return set()
    return {int(t, 16) for t in f.read_text(encoding="utf-8").split() if re.fullmatch(r"(?:0x)?[0-9a-fA-F]+", t)}


def collect(module: str) -> dict:
    """Every usable function of the module with its features, plus the family table and the exclusion sets."""
    ledger = json.loads((M.campaign_root() / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    mod = next(m for m in ledger["modules"] if m["name"] == module)
    data, seg = M.module_bytes({}, module)
    module_dir = re.sub(r"\W", "_", module)
    have = {M.parse_header(p)["start"] for p in (M.ROOT / "src").rglob("*")
            if p.suffix in (".c", ".cpp") and M.parse_header(p)["module"] == module}
    funcs: dict[int, dict] = {}
    families: dict[tuple, list[int]] = defaultdict(list)
    for f in mod["functions"]:
        if f.get("boundary_status") not in P.USABLE_BOUNDARY:
            continue
        off = seg["file_offset"] + f["start"] - seg["start"]
        ws = C.words(data[off:off + f["end"] - f["start"]])
        sig, _refs = C.analyse(ws)
        families[sig].append(f["start"])
        funcs[f["start"]] = {"start": f["start"], "end": f["end"], "size": f["end"] - f["start"], "sig": sig,
                             "flags": flags(ws)}
    family_of: dict[int, list[int]] = {}
    for sig, starts in families.items():
        if len(starts) >= 2:
            for s in starts:
                family_of[s] = sorted(starts)
    reserved, attempted = reservations(M.ROOT, lane_dirs(), QUEUE)
    return {"funcs": funcs, "family_of": family_of, "have": have, "reserved": reserved, "attempted": attempted,
            "gen": best_generated(M.ROOT), "pseudo": pseudocode(GHIDRA, module_dir), "lib": library_hits(M.ROOT),
            "wip": wip_leads(M.ROOT), "names": symbol_names(module), "snc": snc_starts(M.ROOT)}


def item(ctx: dict, module: str, start: int) -> dict:
    f = ctx["funcs"][start]
    it = {"address": f"0x{start:08x}", "end": f"0x{f['end']:08x}", "size": f["size"],
          "family": [f"0x{s:08x}" for s in ctx["family_of"].get(start, []) if s != start]}
    if start in ctx["pseudo"]:
        it["ghidra_name"], it["pseudocode"] = ctx["pseudo"][start]
    if start in ctx["names"]:
        it["symbol"] = ctx["names"][start]
    if start in ctx["gen"]:
        it["best_generated"] = ctx["gen"][start]
    it["flags"] = f["flags"]
    it["cc_hint"] = "snc127-O2" if start in ctx["snc"] else M.PRIMARY_PROFILE
    if start in ctx["lib"]:
        it["library"] = ctx["lib"][start]
    if (module, start) in ctx["wip"]:
        it["wip"] = ctx["wip"][(module, start)]
    return it


def is_near(ctx: dict, module: str, start: int) -> bool:
    g = ctx["gen"].get(start)
    words = ctx["funcs"][start]["size"] // 4
    return bool((g and g["differing_words"] <= NEAR_RATIO * max(words, 1)) or (module, start) in ctx["wip"])


def single_weight(size: int) -> float:
    """Expected code bytes per lane minute for a fresh single, up to a constant: bytes x a success rate falling
    with size, over an effort growing with size; peaks at 192 bytes (HST lanes: fresh singles of 100-350 B gave
    the best yield per hour)."""
    return size / (1 + (size / 192) ** 2)


def rank(ctx: dict, module: str, kind: str, args) -> list[int]:
    funcs, fam = ctx["funcs"], ctx["family_of"]
    covered_fam = {s for s in fam if any(m in ctx["have"] for m in fam[s])}

    # --flag: only functions whose retail words carry that flag; attempted ones are offered again, because a flag
    # filter follows a toolchain change for exactly those instructions (lanes skipped them before)
    blocked = ctx["reserved"] | (set() if args.flag else ctx["attempted"]) | set(args.exclude)

    def open_(s: int) -> bool:
        f = funcs[s]
        if s in ctx["have"] or any(m in blocked for m in fam.get(s, [s])):   # a family is offered once
            return False
        if args.flag and not f["flags"].get(args.flag):
            return False
        if not args.min_size <= f["size"] <= args.max_size:
            return False
        if s in ctx["snc"] and not args.include_snc:
            return False
        return True

    risk = lambda s: (1 if funcs[s]["flags"]["vfpu"] else 0)  # noqa: E731
    if kind == "families":
        reps, seen = [], set()
        for s in sorted(fam):
            members = tuple(fam[s])
            if members in seen or s in covered_fam:
                continue
            seen.add(members)
            cands = [m for m in members if open_(m)]
            if not cands:
                continue
            rep = min(cands, key=lambda m: (ctx["gen"].get(m, {}).get("differing_words", 1 << 30), m))
            reps.append(rep)
        return sorted(reps, key=lambda s: (risk(s), -funcs[s]["size"] * len(fam[s]), s))
    if kind == "near":
        def left(s: int) -> int:
            g = ctx["gen"].get(s)
            return g["differing_words"] if g else funcs[s]["size"] // 8

        best: dict[tuple, int] = {}   # one representative per family: the member with the fewest words left
        for s in funcs:
            if open_(s) and s not in covered_fam and is_near(ctx, module, s):
                k = tuple(fam.get(s, [s]))
                if k not in best or (left(s), s) < (left(best[k]), best[k]):
                    best[k] = s
        unlocked = lambda s: funcs[s]["size"] * max(1, len(fam.get(s, [s]))) / max(1, left(s))  # noqa: E731
        return sorted(best.values(), key=lambda s: (risk(s), -unlocked(s), s))
    if kind == "singles":
        out = [s for s in funcs if open_(s) and s not in fam and not is_near(ctx, module, s)
               and s not in ctx["lib"]]
        def ratio(s: int) -> float:   # words left of the best candidate (1.0 without one): a start point first
            g = ctx["gen"].get(s)
            return g["differing_words"] / max(1, funcs[s]["size"] // 4) if g else 1.0
        return sorted(out, key=lambda s: (risk(s), -round(single_weight(funcs[s]["size"]) / 8), ratio(s), s))
    if kind == "library":
        lib = set(ctx["lib"]) | {s for s, n in ctx["names"].items() if s in funcs and LIBRARY_NAME.match(n)}
        best: dict[tuple, int] = {}   # one per family: the smallest start
        for s in sorted(lib):
            if s in funcs and open_(s) and s not in covered_fam:
                best.setdefault(tuple(fam.get(s, [s])), s)
        return sorted(best.values(), key=lambda s: (risk(s), funcs[s]["size"], s))
    raise SystemExit(f"unknown kind {kind}")


def readability_batches(args) -> int:
    import lbpreadable as R
    clone_note = re.compile(r"^/\* Clone of (src/\S+) \(lbpclone\.py tag [\w./-]+\)")
    reserved, attempted = reservations(M.ROOT, lane_dirs(), QUEUE)
    blocked = reserved | attempted | set(args.exclude)
    rows, clones = [], defaultdict(int)
    for rel in R.publishable(M.ROOT):
        text = (M.ROOT / rel).read_text(encoding="utf-8", errors="replace")
        lines = text.split("\n", 2)
        m = clone_note.match(lines[1]) if len(lines) > 1 else None
        if m:
            clones[m.group(1)] += 1
            continue
        h = M.parse_header(M.ROOT / rel)
        if h["module"] != args.module or h["start"] in blocked:
            continue
        s = R.score(text)
        if s["score"] and args.min_size <= h["end"] - h["start"] <= args.max_size:
            rows.append((s, rel, h))
    # the published debt a rewrite removes: the source's own score and, through --refresh, its clones' copies
    rows.sort(key=lambda r: (-r[0]["score"] * (1 + clones.get(r[1], 0)), r[1]))
    made = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    QUEUE.mkdir(parents=True, exist_ok=True)
    for b in range(args.batches):
        part = rows[b * args.per_batch:(b + 1) * args.per_batch]
        if not part:
            break
        items = [{"address": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}", "size": h["end"] - h["start"],
                  "source": rel, "score": s, "clones": clones.get(rel, 0)} for s, rel, h in part]
        name = f"{args.name}-{b + 1:02d}"
        print(f"{name}: {len(items)} sources, score {sum(it['score']['score'] for it in items)}, "
              f"{sum(it['clones'] for it in items)} clones follow (of {len(rows)} ranked readability)")
        if args.dry_run:
            for it in items[:8]:
                print(f"  {it['score']['score']:4d} {it['clones']:4d} clones  {it['source']}")
            continue
        path = QUEUE / f"{name}.json"
        if path.exists():
            raise SystemExit(f"{path} exists")
        path.write_text(json.dumps({"module": args.module, "kind": "readability", "name": name, "made": made,
                                    "items": items}, indent=1) + "\n", encoding="utf-8")
        print(f"  -> {path}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--name", required=True)
    ap.add_argument("--kind", required=True, choices=("families", "near", "singles", "library", "readability"))
    ap.add_argument("--module", default=P.default_module())
    ap.add_argument("--batches", type=int, default=1)
    ap.add_argument("--per-batch", type=int, default=25)
    ap.add_argument("--min-size", type=int, default=32)
    ap.add_argument("--max-size", type=int, default=1024)
    ap.add_argument("--include-snc", action="store_true")
    ap.add_argument("--flag", choices=("ins_zero", "vfpu", "unaligned", "div"),
                    help="only functions with this retail flag, attempted ones included")
    ap.add_argument("--exclude", type=lambda v: [int(a, 16) for a in v.split(",") if a], default=[],
                    help="addresses never to offer (e.g. matched but not installed yet)")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9][\w.-]*", args.name):
        raise SystemExit(f"bad batch name {args.name!r}")
    if args.kind == "readability":
        return readability_batches(args)
    ctx = collect(args.module)
    order = rank(ctx, args.module, args.kind, args)
    total = len(order)
    QUEUE.mkdir(parents=True, exist_ok=True)
    made = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    for b in range(args.batches):
        part = order[b * args.per_batch:(b + 1) * args.per_batch]
        if not part:
            break
        items = [item(ctx, args.module, s) for s in part]
        name = f"{args.name}-{b + 1:02d}"
        unlock = sum(it["size"] * (1 + len(it["family"])) for it in items)
        print(f"{name}: {len(items)} items, {sum(it['size'] for it in items)} B, {unlock} B with family members"
              f" (of {total} ranked {args.kind})")
        if args.dry_run:
            for it in items[:8]:
                print(f"  {it['address']} {it['size']:5d} B fam {len(it['family']):3d} "
                      f"gen {it.get('best_generated', {}).get('differing_words', '-')} {it['flags']}")
            continue
        path = QUEUE / f"{name}.json"
        if path.exists():
            raise SystemExit(f"{path} exists")
        path.write_text(json.dumps({"module": args.module, "kind": args.kind, "name": name, "made": made,
                                    "items": items}, indent=1) + "\n", encoding="utf-8")
        print(f"  -> {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
