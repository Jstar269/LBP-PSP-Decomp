#!/usr/bin/env python3
"""Compiler-identification benchmark: one fixed sample of functions, measured under candidate compilers.

The sample (data/cceval-sample.json, addresses only) has three parts, drawn once with a fixed seed so results stay
comparable between candidates and sessions:
  gcc171  every source under src/ that matches only with the GCC 3.3.3 kit (lbpgen tag g3, cc=gcc171-Os);
  gcc410  a random sample of sources that match with gcc410-* (lbpgen tags g1/g2/g4);
  miss    a random sample of functions with a Ghidra translation that linked but did not match (lbpgen g2/g4
          pass-1 MISMATCH, no source yet); their v0 translation is regenerated under try/cceval/sample/miss/.

`run` compiles every sample source with one candidate compiler (any psp-gcc driver in WSL, any flags), links and
compares it exactly as lbpmatch.py does (same code, imported), and reports EXACT counts and words left (differing
words; a source that does not compile or link counts all its words) per part. Results are experiments, never
matches: nothing is written to src/.

  python tools/lbpcceval.py sample [--seed 20261027] [--n410 150] [--nmiss 150]
  python tools/lbpcceval.py run --name gcc410-Os --driver ~/lbp-gcc4/psp-gcc-4.1.0s/bin/psp-gcc \\
         --flags "-Os -G0 -fno-merge-constants -mpreferred-stack-boundary=4" [--parallel 6]
  python tools/lbpcceval.py table          # all results under try/cceval/results/, ranked
"""
from __future__ import annotations

import argparse
import json
import os
import random
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402

ROOT = M.ROOT
SAMPLE = ROOT / "data" / "cceval-sample.json"
WORK = ROOT / "try" / "cceval"
CC_RE = re.compile(r"\bcc=\S+")


def src_rows():
    rows = []
    for p in sorted((ROOT / "src").rglob("*")):
        if p.suffix not in (".c", ".cpp"):
            continue
        h = M.parse_header(p)
        text = p.read_text(encoding="utf-8")
        tag = re.search(r"lbpgen\.py tag (\w+)", text)
        rows.append((p, h, tag.group(1) if tag else None))
    return rows


def cmd_sample(args) -> int:
    import lbpgen as G
    import lbpproject as P
    rng = random.Random(args.seed)
    rows = src_rows()
    g171 = sorted((p, h) for p, h, t in rows if t == "g3" and h.get("cc") == "gcc171-Os")
    g410_all = sorted(((p, h) for p, h, t in rows if t in ("g1", "g2", "g4") and h.get("cc", "").startswith("gcc410-")),
                      key=lambda r: r[1]["start"])
    g410 = sorted(rng.sample(g410_all, min(args.n410, len(g410_all))), key=lambda r: r[1]["start"])
    have = {h["start"] for _, h, _ in rows}
    miss_rows = {}
    for tag in ("g2", "g4"):
        for part in sorted((ROOT / "try" / f"gen-{tag}").glob("summary-pass1-*.json")):
            for r in json.loads(part.read_text(encoding="utf-8")):
                start = int(r["start"], 16)
                if r["status"] == "MISMATCH" and start not in have and "/v0/" in r["source"]:
                    miss_rows[start] = r
    miss_starts = sorted(rng.sample(sorted(miss_rows), min(args.nmiss, len(miss_rows))))
    # regenerate the v0 translations of the sampled misses (lbpgen's own translator, variant 0)
    module = P.default_module()
    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    mod = next(m for m in ledger["modules"] if m["name"] == module)
    G.ADDR_RANGE = (0x10000, max(s["memory_end"] for s in mod["load_segments"]))
    G.RETAIL["module"] = module
    G.RETAIL["ends"] = {f["start"]: f["end"] for f in mod["functions"]}
    G.BARE_MIN = 0x08800000
    pseudo, _ = G.load_pseudocode(M.CAMPAIGN, module.replace(".", "_"), ROOT / "ghidra" / "out" / "EBOOT_BIN-p1")
    protos = {}
    for addr, (name, path) in pseudo.items():
        sig = G.signature(path.read_text(encoding="utf-8"))
        if sig:
            protos[name] = sig
    ends = G.RETAIL["ends"]
    out = WORK / "sample" / "miss"
    out.mkdir(parents=True, exist_ok=True)
    misses = []
    for s in miss_starts:
        name, path = pseudo[s]
        src = G.translate(name, path.read_text(encoding="utf-8"), protos, G.VARIANTS[0])
        if src is None:
            continue
        lang = G.VARIANTS[0].get("lang", "c99")
        q = out / (name + (".cpp" if lang == "c++" else ".c"))
        q.write_text(f"/* LBPMATCH module={module} start=0x{s:08x} end=0x{ends[s]:08x} symbol={name} lang={lang} "
                     f"cc=gcc410-Os */\n" + src, encoding="utf-8", newline="\n")
        misses.append({"start": f"0x{s:08x}", "end": f"0x{ends[s]:08x}", "source": q.relative_to(ROOT).as_posix()})
    sample = {"seed": args.seed, "note": "fixed compiler-identification sample; see docs/TOOLCHAIN.md",
              "gcc171": [{"start": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}",
                          "source": p.relative_to(ROOT).as_posix()} for p, h in g171],
              "gcc410": [{"start": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}",
                          "source": p.relative_to(ROOT).as_posix()} for p, h in g410],
              "miss": misses}
    SAMPLE.write_text(json.dumps(sample, indent=1) + "\n", encoding="utf-8")
    print({k: len(v) for k, v in sample.items() if isinstance(v, list)}, f"(pool: gcc410 {len(g410_all)}, miss "
          f"{len(miss_rows)})")
    return 0


def cmd_run(args) -> int:
    if args.src_set:
        # every GCC-profile source under src/ (not the fixed sample): how a candidate does on the current matches
        sample = {"gcc171": [], "gcc410": [], "miss": []}
        for p, h, t in src_rows():
            cc = h.get("cc", "")
            if cc.startswith("gcc"):
                sample["gcc410" if cc.startswith("gcc4") else "gcc171"].append(
                    {"start": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}", "source": p.relative_to(ROOT).as_posix()})
    elif not SAMPLE.exists():
        # disc-derived (lbppublish.DISC_DERIVED), so a public checkout has none
        raise SystemExit(f"{SAMPLE.relative_to(ROOT).as_posix()} does not exist: draw it with `lbpcceval.py sample`, or "
                         "measure every GCC source under src/ with `run --src-set`")
    else:
        sample = json.loads(SAMPLE.read_text(encoding="utf-8"))
    profile = "gcc410-Os"   # a known profile name, whose driver and flags are replaced for this run only
    M.gcc_profiles()[profile] = {"driver": args.driver, "B": args.B, "flags": args.flags}
    run_dir = WORK / "runs" / args.name
    paths, part_of, words = [], {}, {}
    for part in ("gcc171", "gcc410", "miss"):
        d = run_dir / f"{args.name}-{part}"
        d.mkdir(parents=True, exist_ok=True)
        for row in sample[part]:
            src = ROOT / row["source"]
            text = src.read_text(encoding="utf-8")
            head, rest = text.split("\n", 1)
            head = CC_RE.sub(f"cc={profile}", head) if CC_RE.search(head) else head.replace(" */", f" cc={profile} */", 1)
            q = d / src.name
            q.write_text(head + "\n" + rest, encoding="utf-8", newline="\n")
            paths.append(q)
            part_of[q.relative_to(ROOT).as_posix()] = part
            words[q.relative_to(ROOT).as_posix()] = (int(row["end"], 16) - int(row["start"], 16)) // 4
    js = run_dir / "match.json"
    os.environ["LBPMATCH_BUILD_ROOT"] = str(ROOT / "build" / "cceval" / args.name)
    sys.argv = ["lbpmatch.py", "--quiet", "--parallel", str(args.parallel), "--json", str(js)] + [str(p) for p in paths]
    import contextlib
    import io
    with contextlib.redirect_stdout(io.StringIO()):
        M.main()
    res = {p: {"n": 0, "exact": 0, "words_left": 0, "words": 0, "failed": 0} for p in ("gcc171", "gcc410", "miss")}
    per_fn = {}
    for r in json.loads(js.read_text(encoding="utf-8")):
        part = part_of[r["source"]]
        n = words[r["source"]]
        exact = r["status"] == "EXACT"
        left = 0 if exact else (min(r["words_differ"], n) if r["words_differ"] is not None else n)
        t = res[part]
        t["n"] += 1
        t["exact"] += exact
        t["words_left"] += left
        t["words"] += n
        t["failed"] += r["status"] not in ("EXACT", "MISMATCH", "SECTION_MISMATCH")
        per_fn[r["start"]] = left
    summary = {"name": args.name, "driver": args.driver, "B": args.B, "flags": args.flags, "parts": res,
               "total_exact": sum(t["exact"] for t in res.values()),
               "total_words_left": sum(t["words_left"] for t in res.values()), "per_function_words_left": per_fn}
    out = WORK / ("results-src" if args.src_set else "results")
    out.mkdir(parents=True, exist_ok=True)
    (out / f"{args.name}.json").write_text(json.dumps(summary, indent=1) + "\n", encoding="utf-8")
    print(f"{args.name}: EXACT {summary['total_exact']} words_left {summary['total_words_left']} " +
          " ".join(f"{k}={t['exact']}/{t['n']}({t['words_left']}w,{t['failed']}f)" for k, t in res.items()))
    return 0


def cmd_table(args) -> int:
    rows = [json.loads(p.read_text(encoding="utf-8")) for p in sorted((WORK / "results").glob("*.json"))]
    rows.sort(key=lambda r: (-r["total_exact"], r["total_words_left"]))
    print("| Rank | Candidate | EXACT (all) | words left | gcc171-only | gcc410 sample | misses | flags |")
    print("| --- | --- | --- | --- | --- | --- | --- | --- |")
    for i, r in enumerate(rows, 1):
        p = r["parts"]
        cell = lambda t: f"{t['exact']}/{t['n']} ({t['words_left']} w)"  # noqa: E731
        print(f"| {i} | `{r['name']}` | {r['total_exact']} | {r['total_words_left']} | {cell(p['gcc171'])} | "
              f"{cell(p['gcc410'])} | {cell(p['miss'])} | `{r['flags']}` |")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("sample")
    s.add_argument("--seed", type=int, default=20261027)
    s.add_argument("--n410", type=int, default=150)
    s.add_argument("--nmiss", type=int, default=150)
    r = sub.add_parser("run")
    r.add_argument("--name", required=True)
    r.add_argument("--driver", required=True, help="WSL path of a psp-gcc driver")
    r.add_argument("--B", default=None, help="-B prefix for the driver, if any")
    r.add_argument("--flags", required=True)
    r.add_argument("--parallel", type=int, default=6)
    r.add_argument("--src-set", action="store_true", help="all GCC sources under src/ instead of the fixed sample "
                   "(parts: gcc171 = sources with a 3.3.3 profile, gcc410 = sources with a 4.x profile)")
    sub.add_parser("table")
    args = ap.parse_args()
    return {"sample": cmd_sample, "run": cmd_run, "table": cmd_table}[args.cmd](args)


if __name__ == "__main__":
    raise SystemExit(main())
