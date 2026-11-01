#!/usr/bin/env python3
"""Write the public objdiff progress report (docs/objdiff-report.json) from data/public-figures.json.

    python tools/lbpreport.py            write docs/objdiff-report.json
    python tools/lbpreport.py --check    exit 1 when the committed report differs from the published figures

WHY. Progress trackers (bytematch.dev, recomp.fyi) read an objdiff progress report that the default branch's
.github/workflows/progress-report.yml uploads as the artifact `UCUS98744_report`. Public CI cannot build this project
(no game, no keys, no compiler), and the accepted proofs behind the figures stay private. So the maintainers measure
the figures (`python tools/lbppublish.py` writes data/public-figures.json), run this tool and commit the report; the
workflow only uploads the committed file. Run it again after every refresh of data/public-figures.json.

FORMAT. objdiff-core's report (REPORT_VERSION 2) in its JSON form (pbjson with preserve_proto_field_names): snake_case
names, 64-bit counters as decimal strings, 32-bit counters and percentages as numbers. One unit and one category per
module.

WHAT THE NUMBERS MEAN (honest by construction).
  total_code / matched_code      the module's public scope in bytes (the game's own code and the open-source libraries
                                 it links; Sony SDK code is outside it) / the bytes of it that published, exactly
                                 matching sources produce. The percentage is the one in data/public-figures.json: the
                                 whole-module build's (T2) when that is measured, otherwise the isolated-match (T1).
  fuzzy_match_percent            equal to matched_code_percent: near misses are not counted
  total_data / matched_data      the data bytes the T2 figures count (matched_data equals total_data), or 0 and 0
                                 without a T2 figure; data is not a progress claim here
  total_functions / matched_...  0: the published figures count bytes, not functions
  complete_*                     0: nothing is linked from the decompiled sources as a finished unit
The report carries counts and percentages only: no game bytes, no function names, no addresses, no paths.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FIGURES_PATH = ROOT / "data" / "public-figures.json"
README_PATH = ROOT / "README.md"
REPORT_PATH = ROOT / "docs" / "objdiff-report.json"
REPORT_VERSION = 2


def pct(part: int, whole: int) -> float:
    return round(100 * part / whole, 3) if whole else 0.0


def measures(total_code: int, matched_code: int, data: int, units: int) -> dict:
    """One objdiff `Measures` message (JSON form): u64 as strings, u32 and floats as numbers."""
    matched_pct = pct(matched_code, total_code)
    return {"fuzzy_match_percent": matched_pct,
            "total_code": str(total_code), "matched_code": str(matched_code), "matched_code_percent": matched_pct,
            "total_data": str(data), "matched_data": str(data), "matched_data_percent": 100.0 if data else 0.0,
            "total_functions": 0, "matched_functions": 0, "matched_functions_percent": 0.0,
            "complete_code": "0", "complete_code_percent": 0.0, "complete_data": "0", "complete_data_percent": 0.0,
            "total_units": units, "complete_units": 0}


def module_figure(name: str, fig: dict) -> tuple[int, int, int]:
    """(scope bytes, matched code bytes, matched data bytes) of one module: the T2 figures when measured, else T1.
    Refuses figures whose bytes and published percentage disagree."""
    scope = fig["public_scope_bytes"]
    t2 = fig.get("t2")
    if isinstance(t2, dict):
        done, data, published = t2["public_t2_bytes"], t2["public_t2_data_bytes"], t2["public_t2_percent"]
    else:
        done, data, published = fig["public_t1_bytes"], 0, fig["public_t1_percent"]
    if not 0 <= done <= scope:
        raise ValueError(f"{name}: {done} matched bytes outside the scope of {scope}")
    if pct(done, scope) != published:
        raise ValueError(f"{name}: {done}/{scope} B is {pct(done, scope)}%, but the figures publish {published}%")
    return scope, done, data


def build_report(figures: dict) -> dict:
    """The report for the content of data/public-figures.json."""
    units, categories = [], []
    scope = done = data = 0
    for name, fig in figures.items():
        s, d, dat = module_figure(name, fig)
        m = measures(s, d, dat, 1)
        units.append({"name": name, "measures": m, "metadata": {"module_name": name, "progress_categories": [name]}})
        categories.append({"id": name, "name": name, "measures": dict(m)})
        scope, done, data = scope + s, done + d, data + dat
    return {"measures": measures(scope, done, data, len(units)), "units": units, "version": REPORT_VERSION,
            "categories": categories}


def render(report: dict) -> str:
    return json.dumps(report, indent=1) + "\n"


def load_figures(path: Path = FIGURES_PATH) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def readme_disagreement(readme: str, report: dict) -> str:
    """'' when the README's progress sentence ("N code bytes of it, P% of the public scope") states the report's
    figure or none is found; otherwise what differs, so a stale README is noticed before trackers show another number."""
    m = re.search(r"([\d,]+) code bytes of it, ([\d.]+)% of the public scope", readme)
    if not m:
        return ""
    total = report["measures"]
    if int(m.group(1).replace(",", "")) == int(total["matched_code"]) and float(m.group(2)) == total["matched_code_percent"]:
        return ""
    return (f"README.md says {m.group(1)} code bytes = {m.group(2)}%, the report says {total['matched_code']} = "
            f"{total['matched_code_percent']}%: refresh the README's progress paragraph from data/public-figures.json")


def differences(figures: dict, path: Path = REPORT_PATH) -> list[str]:
    """Why the committed report does not match the figures (an empty list when it does)."""
    if not path.exists():
        return [f"{path.name} does not exist"]
    want = build_report(figures)
    try:
        have = json.loads(path.read_text(encoding="utf-8"))
    except ValueError as exc:
        return [f"{path.name} is not valid JSON: {exc}"]
    if have == want:
        return []
    h, w = have.get("measures", {}), want["measures"]
    out = [f"{path.name} {k}: {h.get(k)}, figures say {w[k]}"
           for k in ("total_code", "matched_code", "matched_code_percent") if h.get(k) != w[k]]
    return out or [f"{path.name} differs from the figures (run tools/lbpreport.py)"]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true", help="compare the committed report with the figures; write nothing")
    args = ap.parse_args()
    figures = load_figures()
    report = build_report(figures)
    total = report["measures"]
    diffs: list[str] = []
    if args.check:
        diffs = differences(figures)
        print("objdiff report: " + ("agrees with data/public-figures.json" if not diffs else "DIFFERS"))
        for d in diffs:
            print("  " + d)
    else:
        REPORT_PATH.write_text(render(report), encoding="utf-8", newline="\n")
        print(f"wrote {REPORT_PATH.relative_to(ROOT).as_posix()}: {total['matched_code']}/{total['total_code']} "
              f"code bytes ({total['matched_code_percent']:.3f} %)")
    note = readme_disagreement(README_PATH.read_text(encoding="utf-8"), report)
    if note:
        print("  NOTE: " + note, file=sys.stderr)
    return 1 if diffs else 0


if __name__ == "__main__":
    raise SystemExit(main())
