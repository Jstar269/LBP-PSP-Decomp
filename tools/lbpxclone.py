#!/usr/bin/env python3
"""Cross-project clones: reuse hst-decomp's exact sources for code LittleBigPlanet shares with Hot Shots Tennis.

    python tools/lbpxclone.py --tag x1 [--module EBOOT.BIN] [--cc gcc] [--parallel 4] [--dry-run]

Both games link Sony SDK code (C runtime, newlib libc/libm, SDK libraries) and, when they share a compiler, its
runtime support. A function that is byte-identical apart from call targets and absolute (hi/lo) addresses can take
the HST source with the addresses substituted, exactly as tools/lbpclone.py does inside one project.

hst-decomp is only READ: its sources (`hst_root`/src), its symbol maps (`hst_root`/symbols) and its retail modules
through its campaign ledger (`hst_campaign_root`). For each HST source whose header is a single function (no
`helpers=`), the HST retail bytes of [start, end) give a masked signature (lbpclone.analyse); every unmatched function
of the LBP module with the same signature and a consistent address mapping gets a candidate in
try/xclone-<tag>/, with the header's module, start and end retargeted and the HST symbol-map entries the mapping
reveals passed to lbpmatch.py as --symbols-extra. Only candidates lbpmatch.py reports EXACT are installed in
src/<module dir>/, with the note `Clone of hst-decomp src/... (lbpxclone.py tag X)`, and the symbol entries they
linked against are appended to symbols/<module>.txt. A function a source in src/ already places (its target or a
`helpers=` entry, checked again before installing) or a starter function an open good first issue holds
(data/starter-issues.json) is never a clone target.

--cc filters exemplars by compiler profile prefix (`gcc` = the Sony psp-gcc kits, `mwcc` = mwccpsp).
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C  # noqa: E402
import lbpconfig as CFG  # noqa: E402
import lbpmatch as M  # noqa: E402
import lbpproject as P  # noqa: E402
import lbpqueue as Q  # noqa: E402

HST_HEADER = re.compile(r"/\*\s*HSTMATCH\s+(.*?)\*/", re.S)
NOTE = re.compile(r"^/\*.*?\*/\n", re.S)


def hst_header(text: str) -> dict | None:
    m = HST_HEADER.search(text[:2000])
    if not m:
        return None
    fields = {k: v.strip('"') for k, v in re.findall(r'(\w+)=("[^"]*"|\S+)', m.group(1))}
    if not all(k in fields for k in ("module", "start", "end", "symbol")):
        return None
    fields["start"] = int(fields["start"], 16)
    fields["end"] = int(fields["end"], 16)
    return fields


def hst_modules(campaign: Path) -> dict[str, tuple[bytes, dict]]:
    ledger = json.loads((campaign / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    selection = json.loads((campaign / "inputs" / "revision-selection.json").read_text(encoding="utf-8-sig"))
    out = {}
    for m in ledger["modules"]:
        root = selection["roots"].get(m["input"]["root"])
        if not root:
            continue
        path = Path(root) / m["input"]["path"]
        if not path.exists():
            continue
        seg = next(s for s in m["load_segments"] if s["flags"] & 1)
        out[m["name"]] = (path.read_bytes(), seg)
    return out


def load_hst_symbols(hst_root: Path, module: str) -> dict[str, int]:
    out: dict[str, int] = {}
    path = hst_root / "symbols" / f"{module}.txt"
    if path.exists():
        for line in path.read_text(encoding="utf-8").splitlines():
            line = line.split("#", 1)[0].strip()
            m = re.fullmatch(r"(\S+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;?", line)
            if m:
                out.setdefault(m.group(1), int(m.group(2), 16))
    return out


def covered_starts(module: str) -> set[int]:
    """Starts of the module's functions that a source in src/ places: each header's target and its helpers."""
    out: set[int] = set()
    for q in (M.ROOT / "src").rglob("*"):
        if q.suffix in (".c", ".cpp"):
            h = M.parse_header(q)
            if h["module"] == module:
                out.update(start for _name, start, _end in M.placed_functions(h))
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tag", required=True)
    ap.add_argument("--module", default=P.default_module(), help="LBP module whose functions are the clone targets")
    ap.add_argument("--cc", default="", help="only exemplars whose cc profile starts with this (gcc, mwcc)")
    ap.add_argument("--parallel", type=int, default=4)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--max-per-target", type=int, default=3, help="distinct exemplar texts tried per clone")
    args = ap.parse_args()
    hst_root = CFG.host_path("hst_root")
    hst_campaign = CFG.host_path("hst_campaign_root")
    gen = M.ROOT / "try" / f"xclone-{args.tag}"
    if gen.exists():
        raise SystemExit(f"{gen} exists; use a new tag")

    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    targets = C.load_functions(ledger, args.module)
    covered = covered_starts(args.module)
    held = Q.starter_starts(args.module, M.ROOT)   # an open good first issue holds these for new contributors
    by_sig = defaultdict(list)
    for start, (end, (sig, refs)) in targets.items():
        if start not in covered and start not in held:
            by_sig[sig].append(start)

    modules = hst_modules(hst_campaign)
    hst_syms: dict[str, dict[str, int]] = {}
    target_symbols = M.load_symbols(args.module)
    candidates: dict[int, list[tuple[Path, Path]]] = defaultdict(list)
    seen_text: dict[int, set[str]] = defaultdict(set)
    extra: dict[str, int] = {}
    conflicting: set[str] = set()
    owned: dict[Path, dict[str, int]] = {}
    scanned = 0
    for path in sorted((hst_root / "src").rglob("*")):
        if path.suffix not in (".c", ".cpp"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        h = hst_header(text)
        if h is None or "helpers" in h or h["module"] not in modules:
            continue
        if args.cc and not h.get("cc", M.PRIMARY_PROFILE).startswith(args.cc):
            continue
        data, seg = modules[h["module"]]
        off = seg["file_offset"] + h["start"] - seg["start"]
        sig, e_refs = C.analyse(C.words(data[off:off + h["end"] - h["start"]]))
        scanned += 1
        for t_start in by_sig.get(sig, []):
            t_end, (_sig, t_refs) = targets[t_start]
            maps = C.mapping(e_refs, t_refs)
            if maps is None or len(candidates[t_start]) >= args.max_per_target:
                continue
            body = text.replace("HSTMATCH", P.HEADER_TAG, 1)
            body = body.replace(f"module={h['module']}", f"module={args.module}", 1)
            out = C.retarget(body, h["start"], t_start, t_end, *maps)
            if out is None or out in seen_text[t_start]:
                continue
            seen_text[t_start].add(out)
            name = path.name.replace(f"{h['start']:08x}", f"{t_start:08x}")
            if name == path.name:
                name = f"{path.stem}_{t_start:08x}{path.suffix}"
            dest = gen / f"h{h['start']:08x}" / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_text(out, encoding="utf-8", newline="\n")
            candidates[t_start].append((path, dest))
            base = hst_syms.setdefault(h["module"], load_hst_symbols(hst_root, h["module"]))
            entries = C.symbol_entries(base, h["start"], t_start, *maps, keep_names=True)
            owned[dest] = entries
            for symbol, address in entries.items():
                if target_symbols.get(symbol, address) != address or extra.setdefault(symbol, address) != address:
                    conflicting.add(symbol)
    for symbol in conflicting:
        extra.pop(symbol, None)
    files = [dest for rows in candidates.values() for _src, dest in rows]
    print(f"hst_exemplars_scanned={scanned} clone_targets={len(candidates)} candidates={len(files)} "
          f"symbol_entries={len(extra)} conflicting={len(conflicting)}")
    if args.dry_run or not files:
        return 0
    extra_path = gen / "symbols-extra.txt"
    extra_path.write_text("".join(f"{name} = 0x{address:08x};\n" for name, address in sorted(extra.items())),
                          encoding="utf-8", newline="\n")
    status = {}
    cmd = [sys.executable, "-B", str(Path(M.__file__)), "--quiet", "--parallel", str(args.parallel),
           "--symbols-extra", str(extra_path), "--json"]
    for k in range(0, len(files), 200):
        part = gen / f"summary-{k // 200}.json"
        subprocess.run(cmd + [str(part)] + [str(p) for p in files[k:k + 200]], stdout=subprocess.DEVNULL)
        for row in json.loads(part.read_text(encoding="utf-8")):
            status[str((M.ROOT / row["source"]).resolve())] = row["status"]
    dest_dir = M.ROOT / "src" / P.src_dirs()[args.module]
    dest_dir.mkdir(parents=True, exist_ok=True)
    accepted, accepted_bytes = 0, 0
    committed: dict[str, int] = {}
    covered = covered_starts(args.module)   # again: a source may have been added during the match run
    for t_start, rows in sorted(candidates.items()):
        if t_start in covered:
            continue
        for exemplar, cand in rows:
            if status.get(str(cand.resolve())) != "EXACT":
                continue
            text = cand.read_text(encoding="utf-8")
            head, rest = text.split("\n", 1)
            rest = NOTE.sub("", rest, count=1) if rest.lstrip().startswith("/*") else rest
            note = (f"/* Clone of hst-decomp src/{exemplar.relative_to(hst_root / 'src').as_posix()} "
                    f"(lbpxclone.py tag {args.tag}): addresses substituted; exact match verified; "
                    "names and types await human review. */\n")
            out = dest_dir / cand.name
            if out.exists():
                print(f"skip {out.name}: exists")
                continue
            out.write_text(head + "\n" + note + rest, encoding="utf-8", newline="\n")
            accepted += 1
            accepted_bytes += targets[t_start][0] - t_start
            linked = M.ROOT / "build" / args.module / M.build_key(cand) / "syms.ld"
            used = set(re.findall(r"^(\S+) = ", linked.read_text(encoding="utf-8"), re.M)) if linked.exists() else set()
            for symbol, address in owned.get(cand, {}).items():
                if symbol in used and symbol in extra and symbol not in committed and target_symbols.get(symbol) != address:
                    committed[symbol] = address
            break
    if committed:
        with (M.ROOT / "symbols" / f"{args.module}.txt").open("a", encoding="utf-8", newline="\n") as stream:
            for symbol, address in sorted(committed.items()):
                stream.write(f"{symbol} = 0x{address:08x};   # lbpxclone {args.tag}\n")
    counts: dict[str, int] = {}
    for cand in files:
        key = status.get(str(cand.resolve()), "NOT_RUN")
        counts[key] = counts.get(key, 0) + 1
    report = {"tag": args.tag, "module": args.module, "hst_exemplars_scanned": scanned,
              "clone_targets": len(candidates), "candidates": len(files), "accepted": accepted,
              "accepted_bytes": accepted_bytes, "status_counts": counts}
    (gen / "report.json").write_text(json.dumps(report, indent=1) + "\n", encoding="utf-8")
    print(json.dumps(report))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
