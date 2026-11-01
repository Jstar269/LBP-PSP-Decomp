#!/usr/bin/env python3
"""The public objdiff progress report (docs/objdiff-report.json), a pure function of data/public-figures.json.

    python tools/lbpreport.py [--tree DIR]           write DIR/docs/objdiff-report.json from DIR/data/public-figures.json
    python tools/lbpreport.py --check [--tree DIR]   exit 1 when DIR's report differs from DIR's figures

DIR is a public tree: the public repository's checkout (the default: the tree this file sits in) or one that
tools/lbppublish.py built. In the private repository there is no public-figures.json: lbppublish.py computes the
figures from the private proofs and writes the report beside them in the same run (build_report below), and its
--audit runs the same check on the tree it audits, so the two files cannot disagree in a published tree. This command
is for looking at, or re-deriving, a tree's report.

WHY. Progress trackers (bytematch.dev, recomp.fyi) read an objdiff progress report that the default branch's
.github/workflows/progress-report.yml uploads as the artifact `UCUS98744_report`. Public CI cannot build this project
(no game, no keys, no compiler), and the accepted proofs behind the figures stay private. So the report is made from the
figures and committed; the workflow only uploads the committed file.

FORMAT. objdiff-core's report (REPORT_VERSION 2) in its JSON form (pbjson with preserve_proto_field_names): snake_case
names, 64-bit counters as decimal strings, 32-bit counters and percentages as numbers. One unit and one category per
module.

WHAT THE NUMBERS MEAN (honest by construction). The report is always tier T1, so bytes and functions share one basis
(the whole-module build, T2, has no function count; its figure is the README headline, not the trackers').
  total_code / matched_code      the module's public scope in bytes (the game's own code and the open-source libraries
                                 it links; Sony SDK code is outside it) / the bytes of it owned by accepted proofs of
                                 published sources (public_t1_bytes): functions that match exactly in isolation
  total_functions / matched_...  the public scope's functions / those of them with such a proof
                                 (public_scope_functions, public_t1_functions); counts only
  fuzzy_match_percent            equal to matched_code_percent: near misses are not counted
  total_data / matched_data      0 and 0: data is not a progress claim here
  complete_*                     0: nothing is linked from the decompiled sources as a finished unit
The report carries counts and percentages only: no game bytes, no function names, no addresses, no paths.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FIGURES_REL = "data/public-figures.json"
REPORT_REL = "docs/objdiff-report.json"
FIGURES_PATH = ROOT / FIGURES_REL
REPORT_PATH = ROOT / REPORT_REL
REPORT_VERSION = 2


def pct(part: int, whole: int) -> float:
    return round(100 * part / whole, 3) if whole else 0.0


def measures(total_code: int, matched_code: int, total_functions: int, matched_functions: int, units: int) -> dict:
    """One objdiff `Measures` message (JSON form): u64 as strings, u32 and floats as numbers."""
    matched_pct = pct(matched_code, total_code)
    return {"fuzzy_match_percent": matched_pct,
            "total_code": str(total_code), "matched_code": str(matched_code), "matched_code_percent": matched_pct,
            "total_data": "0", "matched_data": "0", "matched_data_percent": 0.0,
            "total_functions": total_functions, "matched_functions": matched_functions,
            "matched_functions_percent": pct(matched_functions, total_functions),
            "complete_code": "0", "complete_code_percent": 0.0, "complete_data": "0", "complete_data_percent": 0.0,
            "total_units": units, "complete_units": 0}


def module_figure(name: str, fig: dict) -> tuple[int, int, int, int]:
    """(scope bytes, matched bytes, scope functions, matched functions) of one module, tier T1. Refuses figures whose
    counts and published percentage disagree."""
    scope, done = fig["public_scope_bytes"], fig["public_t1_bytes"]
    n_scope, n_done = fig["public_scope_functions"], fig["public_t1_functions"]
    if not 0 <= done <= scope:
        raise ValueError(f"{name}: {done} matched bytes outside the scope of {scope}")
    if not 0 <= n_done <= n_scope:
        raise ValueError(f"{name}: {n_done} matched functions outside the scope of {n_scope}")
    if pct(done, scope) != fig["public_t1_percent"]:
        raise ValueError(f"{name}: {done}/{scope} B is {pct(done, scope)}%, but the figures publish "
                         f"{fig['public_t1_percent']}%")
    return scope, done, n_scope, n_done


def build_report(figures: dict) -> dict:
    """The report for the content of data/public-figures.json."""
    units, categories = [], []
    sums = [0, 0, 0, 0]
    for name, fig in figures.items():
        row = module_figure(name, fig)
        m = measures(*row, 1)
        units.append({"name": name, "measures": m, "metadata": {"module_name": name, "progress_categories": [name]}})
        categories.append({"id": name, "name": name, "measures": dict(m)})
        sums = [a + b for a, b in zip(sums, row)]
    return {"measures": measures(*sums, len(units)), "units": units, "version": REPORT_VERSION,
            "categories": categories}


def render(report: dict) -> str:
    return json.dumps(report, indent=1) + "\n"


def load_figures(path: Path = FIGURES_PATH) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def differences(figures: dict, path: Path = REPORT_PATH) -> list[str]:
    """Why the report at `path` does not match the figures (an empty list when it does)."""
    if not path.exists():
        return [f"{path.name} does not exist"]
    want = build_report(figures)
    text = path.read_text(encoding="utf-8")
    try:
        have = json.loads(text)
    except ValueError as exc:
        return [f"{path.name} is not valid JSON: {exc}"]
    if have == want and text == render(want):
        return []
    h, w = have.get("measures", {}), want["measures"]
    out = [f"{path.name} {k}: {h.get(k)}, figures say {w[k]}"
           for k in ("total_code", "matched_code", "matched_code_percent", "total_functions", "matched_functions")
           if h.get(k) != w[k]]
    return out or [f"{path.name} differs from the figures (run tools/lbpreport.py)"]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true", help="compare the report with the figures; write nothing")
    ap.add_argument("--tree", type=Path, default=ROOT, help="the public tree (default: the tree this file is in)")
    args = ap.parse_args()
    figures_path, report_path = args.tree / FIGURES_REL, args.tree / REPORT_REL
    if not figures_path.exists():
        print(f"no {figures_path}: in the private repository build a tree first (tools/lbppublish.py --out <dir>) "
              "and pass --tree <dir>")
        return 2
    figures = load_figures(figures_path)
    report = build_report(figures)
    total = report["measures"]
    diffs: list[str] = []
    if args.check:
        diffs = differences(figures, report_path)
        print("objdiff report: " + ("agrees with data/public-figures.json" if not diffs else "DIFFERS"))
        for d in diffs:
            print("  " + d)
    else:
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(render(report), encoding="utf-8", newline="\n")
        print(f"wrote {REPORT_REL}: {total['matched_code']}/{total['total_code']} code bytes "
              f"({total['matched_code_percent']:.3f} %), {total['matched_functions']}/{total['total_functions']} "
              "functions")
    return 1 if diffs else 0


if __name__ == "__main__":
    raise SystemExit(main())
