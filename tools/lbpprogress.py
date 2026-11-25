#!/usr/bin/env python3
"""The tier ladder of docs/COMPLETION.md (T1 to T4 over the strict scope) and the two files that publish it.

    python tools/lbpprogress.py [--tree DIR]           print the ladder (nothing is written)
    python tools/lbpprogress.py --write [--tree DIR]   write DIR/progress.json and the ladder block of
                                                       DIR/docs/COMPLETION.md
    python tools/lbpprogress.py --check [--tree DIR]   exit 1 when a committed figure disagrees with the ledger rows,
                                                       the published figures, progress.json or the ladder block

DIR is a public tree: the public repository's checkout (the default, the tree this file sits in) or one that
tools/lbppublish.py built, which calls write_tree() and, in --audit, tree_problems(). The ladder is a pure function of
two files the public tree already carries, so it needs neither the game nor the private ledger:

  data/modules.json          the strict scope: every byte of every module in scope (`size`, the decrypted module), the
                             denominator of docs/COMPLETION.md, which never shrinks
  data/public-figures.json   the published figures (tools/lbppublish.py figures(), from the accepted ledger)

TIERS, each a share of the strict scope (hst-decomp's tools/hstprogress.py reports the same ladder):

  T1  code bytes of the public scope owned by accepted proofs of published sources (`public_t1_bytes`). Sony SDK code
      matched in the maintainer's tree is not published, so it is not counted here; T1 is a floor
  T2  bytes the published sources produce in a byte-identical whole-module build (`t2.image_t2_bytes`, code and data;
      tools/lbpbuild.py). 0 while a module's figures say `not measured`; scaffolding never counts
  T3  T2 and the shifted build passes (gate 5): `t3.image_t3_bytes` when the figures carry a T3 measurement, else 0
  T4  = STRICT: T3 and the source is reviewed (gate 7). 0: no review is recorded yet. Nothing is estimated

progress.json is the strict figure as a shields.io endpoint badge, in hst-decomp's format. The ladder block is
`<!-- lbpprogress:begin tiers -->` ... `<!-- lbpprogress:end -->` in docs/COMPLETION.md; do not edit it by hand.

--check also checks the published figures against the ledger they come from: in a public tree that is
data/public-functions.txt (one row per function of the public scope, from the accepted ledger; tools/lbpreport.py
refuses rows that do not add up to the figures) and the published boundary lists; where the accepted ledger itself
is present (data/accepted-ledger.jsonl, the maintainer's tree), every row's matched bytes are recounted from its
accepted proof. tools/lbpxverify.py (`figures`) recounts the same figures with code of its own.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpreport

ROOT = Path(__file__).resolve().parent.parent
MODULES_REL = "data/modules.json"
BADGE_REL = "progress.json"
COMPLETION_REL = "docs/COMPLETION.md"
BLOCK = re.compile(r"<!-- lbpprogress:begin ([\w-]+) -->\n\n(.*?)\n\n<!-- lbpprogress:end -->", re.S)
TIERS = ((1, "function matches (published sources)"), (2, "whole-module build"), (3, "shifted build"),
         (4, "strict (reviewed)"))


def pct(part: int, whole: int) -> float:
    return round(100 * part / whole, 3) if whole else 0.0


def scope_bytes(modules_doc: dict) -> int:
    """The strict scope: the size of every decrypted module data/modules.json lists."""
    sizes = [m["size"] for m in modules_doc["modules"]]
    if not sizes or any(not isinstance(s, int) or s <= 0 for s in sizes):
        raise ValueError(f"{MODULES_REL}: every module needs a positive size")
    return sum(sizes)


def ladder(figures: dict, modules_doc: dict) -> dict:
    """The tiers over the strict scope, from the published figures. Refuses figures that break the ladder's order."""
    scope = scope_bytes(modules_doc)
    known = {m["name"] for m in modules_doc["modules"]}
    t1 = t2 = t3 = 0
    measured = {"t2": [], "t3": []}
    for name, fig in figures.items():
        if name not in known:
            raise ValueError(f"the figures name {name}, which {MODULES_REL} does not list")
        t1 += fig["public_t1_bytes"]
        if isinstance(fig.get("t2"), dict):
            t2 += fig["t2"]["image_t2_bytes"]
            measured["t2"].append(name)
        if isinstance(fig.get("t3"), dict):
            if not isinstance(fig.get("t2"), dict):
                raise ValueError(f"{name}: a T3 figure without a T2 figure")
            t3 += fig["t3"]["image_t3_bytes"]
            measured["t3"].append(name)
    t4 = 0
    if not (0 <= t4 <= t3 <= t2 <= scope and 0 <= t1 <= scope):
        raise ValueError(f"the tiers are out of order: T1 {t1}, T2 {t2}, T3 {t3}, T4 {t4}, scope {scope}")
    dates = sorted(f["ledger_date"] for f in figures.values() if f.get("ledger_date"))
    return {"scope_bytes": scope, "t1_bytes": t1, "t2_bytes": t2, "t3_bytes": t3, "t4_bytes": t4,
            "t1_percent": pct(t1, scope), "t2_percent": pct(t2, scope), "t3_percent": pct(t3, scope),
            "t4_percent": pct(t4, scope), "strict_percent": pct(t4, scope),
            "t2_measured": measured["t2"], "t3_measured": measured["t3"], "ledger_date": dates[-1] if dates else None}


def badge(lad: dict) -> dict:
    """progress.json: the strict figure as a shields.io endpoint badge (hst-decomp's progress.json format)."""
    s = lad["strict_percent"]
    color = "red" if s < 10 else "orange" if s < 25 else "yellow" if s < 50 else "yellowgreen" if s < 75 \
        else "brightgreen"
    return {"schemaVersion": 1, "label": "strict match", "message": f"{s:.1f}% (tier 1: {lad['t1_percent']:.1f}%)",
            "color": color}


def render_badge(lad: dict) -> str:
    return json.dumps(badge(lad), indent=1) + "\n"


def render_blocks(lad: dict) -> dict[str, str]:
    """{block name: text} for docs/COMPLETION.md, a pure function of the ladder."""
    def cell(n: int) -> str:
        if n in (2, 3) and not lad[f"t{n}_measured"]:
            return "not measured"
        return f"{lad[f't{n}_bytes']:,} B"
    rows = [f"| T{n}: {label} | {cell(n)} | {lad[f't{n}_percent']:.3f}% |" for n, label in TIERS]
    when = f"the accepted proofs of {lad['ledger_date']}" if lad["ledger_date"] else "the accepted proofs"
    tiers = "\n".join([
        f"| Tier | Bytes | Share of the strict scope ({lad['scope_bytes']:,} B) |",
        "|---|---|---|",
        *rows,
        "",
        f"Measured from `data/public-figures.json` ({when}) by `tools/lbpprogress.py`; the strict scope is every byte "
        "of every module `data/modules.json` lists. T1 counts published sources only, so Sony SDK code matched in the "
        "maintainer's tree is not in it. A tier that is not measured counts 0. "
        f"[`progress.json`](../progress.json) is the strict figure as a badge."])
    return {"tiers": tiers}


def fill_blocks(text: str, blocks: dict[str, str]) -> str:
    return BLOCK.sub(lambda m: f"<!-- lbpprogress:begin {m.group(1)} -->\n\n{blocks.get(m.group(1), m.group(2))}"
                               "\n\n<!-- lbpprogress:end -->", text)


def block_differences(text: str, lad: dict, rel: str = COMPLETION_REL) -> list[str]:
    want = render_blocks(lad)
    have = {m.group(1): m.group(2) for m in BLOCK.finditer(text)}
    return [f"{rel}: block {name} " + ("is missing" if name not in have else "differs from the figures")
            for name in want if have.get(name) != want[name]]


# --------------------------------------------------------------------------------------------------------------
# the figures against the ledger they come from

def boundary_list(tree: Path, module: str) -> dict[int, int] | None:
    """{start: end} of a module's published boundary list (data/functions-<module>.txt), or None without one."""
    path = tree / "data" / f"functions-{module}.txt"
    if not path.is_file():
        return None
    out = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if line:
            s, e = line.split()[:2]
            out[int(s, 16)] = int(e, 16)
    return out


def ledger_problems(tree: Path, figures: dict, rows: dict[str, list[tuple]] | None,
                    proofs: list[dict] | None = None) -> list[str]:
    """Why the published figures do not follow from the ledger rows (data/public-functions.txt) and, when given, from
    the accepted proofs behind them ([] when they do)."""
    if rows is None:
        return [f"{lbpreport.ROWS_REL} is missing: the figures cannot be checked against the ledger"]
    out = []
    for name, fig in figures.items():
        try:
            want = lbpreport.module_figure(name, fig)
        except (KeyError, ValueError) as exc:
            out.append(f"{lbpreport.FIGURES_REL}: {exc}")
            continue
        mine = rows.get(name, [])
        got = (sum(r[1] for r in mine), sum(r[2] for r in mine), len(mine), sum(1 for r in mine if r[3]))
        if got != want:
            out.append(f"{name}: the ledger rows give {got} (scope bytes, matched bytes, functions, matched functions), "
                       f"the figures {want}")
        bounds = boundary_list(tree, name)
        for start, size, matched, source, _sym in mine:
            if not 0 <= matched <= size or bool(source) != bool(matched):
                out.append(f"{name} 0x{start:08x}: {matched} matched bytes of {size}, source {source or '-'}")
            elif bounds is not None and bounds.get(start) != start + size:
                out.append(f"{name} 0x{start:08x}: not a function of data/functions-{name}.txt")
    for name in rows:
        if name not in figures:
            out.append(f"{lbpreport.ROWS_REL} lists {name}, which the figures do not")
    if proofs is not None:
        owners = {(p["module"], int(p["start"], 16)): p for p in proofs if p.get("status") == "EXACT"}
        for name, mine in rows.items():
            for start, size, matched, source, _sym in mine:
                if not source:
                    continue
                p = owners.get((name, start))
                if p is None or p.get("source") != source:
                    out.append(f"{name} 0x{start:08x}: {source} has no accepted proof at this address")
                elif min(int(p["end"], 16) - start, size) != matched:
                    out.append(f"{name} 0x{start:08x}: the proof gives {min(int(p['end'], 16) - start, size)} matched "
                               f"bytes, the rows {matched}")
    return out


def private_proofs(root: Path = ROOT) -> list[dict] | None:
    """The accepted proofs when the accepted ledger is present (the maintainer's tree), else None."""
    import lbpproof
    if not lbpproof.ledger_present(root):
        return None
    return lbpproof.accepted_proofs(root=root)


# --------------------------------------------------------------------------------------------------------------
# a tree's files

def tree_ladder(tree: Path) -> dict:
    figures = lbpreport.load_figures(tree / lbpreport.FIGURES_REL)
    modules_doc = json.loads((tree / MODULES_REL).read_text(encoding="utf-8"))
    return ladder(figures, modules_doc)


def write_tree(tree: Path) -> list[str]:
    """Write progress.json and fill the ladder block of docs/COMPLETION.md (when it has one); the paths written."""
    lad = tree_ladder(tree)
    written = []
    (tree / BADGE_REL).write_text(render_badge(lad), encoding="utf-8", newline="\n")
    written.append(BADGE_REL)
    completion = tree / COMPLETION_REL
    if completion.is_file():
        text = completion.read_text(encoding="utf-8")
        new = fill_blocks(text, render_blocks(lad))
        if new != text:
            completion.write_text(new, encoding="utf-8", newline="\n")
            written.append(COMPLETION_REL)
    return written


def tree_problems(tree: Path, require: bool = False) -> list[str]:
    """Why progress.json or the ladder block of a tree disagrees with its figures ([] when they agree). Without
    `require`, a tree that has neither file is not checked (lbppublish.py --audit of a tree it did not build)."""
    have_badge = (tree / BADGE_REL).is_file()
    completion = tree / COMPLETION_REL
    have_block = completion.is_file() and bool(BLOCK.search(completion.read_text(encoding="utf-8")))
    if not require and not have_badge and not have_block:
        return []
    try:
        lad = tree_ladder(tree)
    except (OSError, ValueError, KeyError, TypeError) as exc:
        return [f"{BADGE_REL}: the ladder cannot be computed from the figures ({exc})"]
    out = []
    if not have_badge:
        out.append(f"{BADGE_REL} is missing")
    elif (tree / BADGE_REL).read_text(encoding="utf-8") != render_badge(lad):
        out.append(f"{BADGE_REL} differs from the figures: it says "
                   f"{json.loads((tree / BADGE_REL).read_text(encoding='utf-8')).get('message')!r}, the figures "
                   f"{badge(lad)['message']!r}")
    if completion.is_file():
        out += block_differences(completion.read_text(encoding="utf-8"), lad)
    elif require:
        out.append(f"{COMPLETION_REL} is missing")
    return out


def describe(lad: dict) -> list[str]:
    lines = [f"strict scope {lad['scope_bytes']:,} B" + (f", figures of {lad['ledger_date']}" if lad["ledger_date"]
                                                          else "")]
    for n, label in TIERS:
        lines.append(f"  T{n} {label:38} {lad[f't{n}_bytes']:>11,} B  {lad[f't{n}_percent']:7.3f}%")
    lines.append(f"badge: {badge(lad)['message']}")
    return lines


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tree", type=Path, default=ROOT, help="the public tree (default: the tree this file is in)")
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--write", action="store_true", help="write progress.json and the ladder block")
    mode.add_argument("--check", action="store_true", help="compare every committed figure; write nothing")
    args = ap.parse_args()
    tree = args.tree.resolve()
    if not (tree / lbpreport.FIGURES_REL).is_file():
        print(f"no {tree / lbpreport.FIGURES_REL}: in the private repository build a tree first "
              "(tools/lbppublish.py --out <dir>) and pass --tree <dir>")
        return 2
    if args.write:
        for rel in write_tree(tree):
            print(f"wrote {rel}")
    lad = tree_ladder(tree)
    for line in describe(lad):
        print(line)
    if not args.check:
        return 0
    figures = lbpreport.load_figures(tree / lbpreport.FIGURES_REL)
    rows = lbpreport.load_rows(tree / lbpreport.ROWS_REL)
    proofs = private_proofs()
    problems = ledger_problems(tree, figures, rows, proofs)
    if not problems:
        problems += lbpreport.differences(figures, tree / lbpreport.REPORT_REL, rows,
                                          lbpreport.read_units(tree / lbpreport.TU_REL))
    problems += tree_problems(tree, require=True)
    print("ledger: " + ("the accepted ledger" if proofs is not None else f"{lbpreport.ROWS_REL} (public rows)"))
    print("figures: " + ("agree" if not problems else "DIFFER"))
    for p in problems[:50]:
        print("  " + p)
    if len(problems) > 50:
        print(f"  ... and {len(problems) - 50} more")
    return 1 if problems else 0


if __name__ == "__main__":
    raise SystemExit(main())
