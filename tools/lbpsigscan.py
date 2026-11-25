#!/usr/bin/env python3
"""Compare the functions of compiled objects with retail code: which compiler and flags reproduce a module's code?

    python tools/lbpsigscan.py OBJ.o [OBJ.o ...] [--module EBOOT.BIN] [--at NAME=0xADDR ...]

For every function symbol in the objects' .text, the masked signature (tools/lbpclone.py: call targets and %hi/%lo
address halves masked) is looked up among the module's functions; a hit means the retail function is that code up
to addresses. `--at NAME=0xADDR` also compares one object function with the retail function at ADDR word by word
(masked), printing how many words differ, which tells how close a compiler/flag choice comes on a known function.
Read-only; prints a summary.
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C  # noqa: E402
import lbpmatch as M  # noqa: E402
import lbpproject as P  # noqa: E402


def object_functions(path: Path):
    obj = path.read_bytes()
    sections = M.elf_sections(obj)
    for i, (name, s) in enumerate(sections):
        # every code input section ld places (tools/lbpmatch.py CODE_INPUT), including the `.gnu.linkonce.t.*` that
        # GCC 4.0 gives each inline function and implicit member, not only the `.text*` ones
        if s[1] != 1 or not M.CODE_INPUT.fullmatch(name) or not s[5]:
            continue
        data = obj[s[4]:s[4] + s[5]]
        syms = sorted((v, sz, n) for n, v, sz, info, sh in M.elf_symbols(obj) if sh == i and info & 15 == 2 and sz >= 4)
        for v, sz, n in syms:
            yield n, C.words(data[v:v + sz])


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("objects", nargs="+", type=Path)
    ap.add_argument("--module", default=P.default_module())
    ap.add_argument("--at", action="append", default=[], metavar="NAME=0xADDR")
    ap.add_argument("--min-words", type=int, default=4, help="ignore object functions shorter than this")
    args = ap.parse_args()
    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    mod = next(m for m in ledger["modules"] if m["name"] == args.module)
    data, seg = M.module_bytes({}, args.module)
    by_sig: dict[tuple, list[int]] = {}
    spans = {}
    for f in mod["functions"]:
        off = seg["file_offset"] + f["start"] - seg["start"]
        ws = C.words(data[off:off + f["end"] - f["start"]])
        spans[f["start"]] = ws
        by_sig.setdefault(C.analyse(ws)[0], []).append(f["start"])
    at = {k: int(v, 16) for k, v in (x.split("=", 1) for x in args.at)}
    total = hit = 0
    for path in args.objects:
        for name, ws in object_functions(path):
            if len(ws) < args.min_words:
                continue
            total += 1
            starts = by_sig.get(C.analyse(ws)[0], [])
            if starts:
                hit += 1
                print(f"HIT  {path.name}:{name} ({len(ws)} words) -> " + ", ".join(f"0x{a:08x}" for a in starts[:4]))
            if name in at:
                theirs = spans.get(at[name])
                if theirs is None:
                    print(f"AT   {name}: no function starts at 0x{at[name]:08x}")
                    continue
                s1, s2 = C.analyse(ws)[0], C.analyse(theirs)[0]
                diff = sum(1 for i in range(max(len(s1), len(s2))) if i >= len(s1) or i >= len(s2) or s1[i] != s2[i])
                print(f"AT   {name}: ours {len(ws)} words, retail {len(theirs)} words, {diff} masked words differ")
    print(f"{hit}/{total} object functions (>= {args.min_words} words) have a signature hit in {args.module}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
