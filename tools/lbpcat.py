#!/usr/bin/env python3
"""Classify why generated candidates mismatched (after lbpgen.py), to choose the next translation fix.

    python tools/lbpcat.py try/gen-<tag> [--variant v0] [--examples 3]

Reads each MISMATCH candidate's linked bytes (build/<module>/<lbpmatch.build_key>/text.bin) and the retail
bytes (target.bin, written by lbpmatch.py on a mismatch), and assigns the first matching class:

  frame        the stack adjustment (first instruction) differs
  length       different instruction counts (ours shorter/longer)
  load_sign    only lb<->lbu / lh<->lhu differ
  regalloc     same operations everywhere (sub-opcode fields included), only register fields differ
  likely       only branch vs branch-likely (and the delay slots they change), REGIMM and bc1 forms included
  reorder      the same multiset of words in a different order
  other        anything else
"""
from __future__ import annotations

import argparse
import collections
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpdiff as D  # noqa: E402
import lbpmatch as M  # noqa: E402
import lbpproject as P  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
LOAD_SIGN = {(0x20, 0x24), (0x24, 0x20), (0x21, 0x25), (0x25, 0x21)}  # lb/lbu, lh/lhu
LIKELY = {0x04: 0x14, 0x05: 0x15, 0x06: 0x16, 0x07: 0x17}  # beq/bne/blez/bgtz -> likely forms


def words(data: bytes) -> list[int]:
    return [int.from_bytes(data[i:i + 4], "little") for i in range(0, len(data) - len(data) % 4, 4)]


def opcode_key(word: int) -> tuple[int, ...]:
    """The instruction's operation with its register fields left out: two words with equal keys differ only in
    registers or immediates. Allegrex selects many operations below the major opcode, so those fields are kept."""
    op, rs, rt, sa, fn = word >> 26, (word >> 21) & 0x1F, (word >> 16) & 0x1F, (word >> 6) & 0x1F, word & 0x3F
    if op == 0:
        if fn == 0x02:
            return (0, fn, rs & 1)       # srl / rotr
        if fn == 0x06:
            return (0, fn, sa & 1)       # srlv / rotrv
        return (0, fn)                   # SPECIAL: function field
    if op == 1:
        return (1, rt)                   # REGIMM: rt selects the branch
    if op in (0x10, 0x11, 0x12):         # COP0, COP1 (FPU), COP2 (VFPU moves and branches)
        if rs == 8:
            return (op, rs, rt & 3)      # bc1f/bc1t/bc1fl/bc1tl, bvf/bvt/bvfl/bvtl
        if rs >= 0x10:
            return (op, rs, fn)          # add.s/mul.s/c.cond.s/cvt.s.w ..., eret
        return (op, rs, (word >> 7) & 1)  # mfc1/mtc1/cfc1/ctc1, mfv/mfvc/mtv/mtvc
    if op == 0x1C:
        return (op, fn)                  # SPECIAL2: halt, mfic, mtic
    if op == 0x1F:
        return (op, fn, sa) if fn == 0x20 else (op, fn)   # SPECIAL3: ext, ins; bshfl: wsbh, wsbw, seb, bitrev, seh
    if op == 0x2F:
        return (op, rt)                  # cache: rt is the cache operation
    if op in (0x18, 0x19, 0x1B):         # VFPU0/1/3: vadd/vsub/vdiv, vmul/vdot/vscl..., vcmp/vmin/vmax...
        sub = (word >> 23) & 7
        return (op, sub, word & (0x808F if op == 0x1B and sub == 0 else 0x8080))   # vcmp: bits 0-3 are the condition
    if op in (0x35, 0x36, 0x3D, 0x3E):
        return (op, (word >> 1) & 1)     # lvl.q/lvr.q, lv.q, svl.q/svr.q, sv.q and its write-back form
    if op == 0x34:                       # VFPU4: vmov/vabs/vneg..., vcst, vf2in, vi2f ... (only vd and vs are registers)
        return (op, (word >> 16) & 0x3FF, word & 0x8080)
    if op == 0x37:                       # VFPU5: vpfxs/vpfxt/vpfxd (the whole prefix), viim/vfim
        return (op, word & 0x03FFFFFF) if (word >> 24) & 3 != 3 else (op, (word >> 23) & 7)
    if op == 0x3C:                       # VFPU6: vmmul, vtfm/vhtfm, vmscl, vcrsp/vqmul; vmmov/vmidt/vmzero/vmone, vrot
        sub = (word >> 23) & 7
        return (op, sub, (word >> 16) & 0x7F if sub == 7 else 0, word & 0x8080)
    if op == 0x3F:
        return (op, word & 0x03FFFFFF)   # VFPU7: vnop, vsync, vflush
    return (op, 0)


def likely_pair(a: int, b: int) -> bool:
    """`a` and `b` are the same branch, one of them in its likely form."""
    def form(w: int) -> tuple[int, ...] | None:
        op, rs, rt = w >> 26, (w >> 21) & 0x1F, (w >> 16) & 0x1F
        if op in LIKELY or op in LIKELY.values():
            return (op - 0x10 if op >= 0x14 else op, int(op >= 0x14))
        if op == 1 and rt in (0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13):
            return (1, rt & ~2, (rt >> 1) & 1)          # bltz/bgez/bltzal/bgezal and their likely forms
        if op in (0x11, 0x12) and rs == 8:
            return (op, rt & 1, (rt >> 1) & 1)          # bc1f/bc1t, bvf/bvt: the nd bit is the likely form
        return None
    fa, fb = form(a), form(b)
    return fa is not None and fb is not None and fa[:-1] == fb[:-1] and fa[-1] != fb[-1]


def classify(ours: list[int], theirs: list[int]) -> str:
    if ours and theirs and ours[0] != theirs[0] and ours[0] >> 16 == 0x27BD and theirs[0] >> 16 == 0x27BD:
        return "frame"
    if len(ours) != len(theirs):
        return "length"
    pairs = [(a, b) for a, b in zip(ours, theirs) if a != b]
    if all(((a >> 26), (b >> 26)) in LOAD_SIGN and (a & 0x03FFFFFF) == (b & 0x03FFFFFF) for a, b in pairs):
        return "load_sign"
    if all(opcode_key(a) == opcode_key(b) for a, b in pairs):
        return "regalloc"
    likely_pairs = [(a, b) for a, b in pairs if likely_pair(a, b)]
    if likely_pairs and len(likely_pairs) * 3 >= len(pairs):
        return "likely"
    if sorted(ours) == sorted(theirs):
        return "reorder"
    return "other"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("gen", type=Path)
    ap.add_argument("--variant", default="v0")
    ap.add_argument("--module", default=P.default_module())
    ap.add_argument("--examples", type=int, default=3)
    args = ap.parse_args()
    gen = args.gen.resolve()
    rows = []
    for summary in sorted(gen.glob("summary-*.json")):
        rows += json.loads(summary.read_text(encoding="utf-8"))
    exact = {row["start"] for row in rows if row["status"] == "EXACT"}
    counts: collections.Counter = collections.Counter()
    sizes: collections.Counter = collections.Counter()
    examples: dict[str, list[str]] = collections.defaultdict(list)
    for row in rows:
        source = ROOT / row["source"]
        if row["status"] != "MISMATCH" or source.parent.name != args.variant or row["start"] in exact:
            continue
        out = ROOT / "build" / args.module / M.build_key(source)
        if not (out / "text.bin").exists() or not (out / "target.bin").exists():
            continue
        theirs = words((out / "target.bin").read_bytes())
        start = row["start"] if isinstance(row["start"], int) else int(row["start"], 16)
        ours = words(D.function_bytes(out / "text.bin", start, 4 * len(theirs)))[:max(len(theirs), 1) + 64]
        # text.bin may run past the function; compare only our function's words when lengths agree
        ours_fn = ours[:len(theirs)] if len(ours) >= len(theirs) and ours[len(theirs) - 1:len(theirs)] else ours
        kind = classify(ours_fn, theirs)
        counts[kind] += 1
        sizes[kind] += 4 * len(theirs)
        if len(examples[kind]) < args.examples:
            examples[kind].append(row["source"])
    total = sum(counts.values())
    print(f"{args.variant} mismatches classified: {total}")
    for kind, n in counts.most_common():
        print(f"  {kind:10s} {n:5d}  {sizes[kind]:7d} bytes   e.g. {', '.join(examples[kind])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
