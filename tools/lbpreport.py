#!/usr/bin/env python3
"""The public objdiff progress report (docs/objdiff-report.json), a pure function of data/public-figures.json,
data/public-functions.txt and data/translation-units.json.

    python tools/lbpreport.py [--tree DIR]           write DIR/docs/objdiff-report.json from DIR's figures and rows
    python tools/lbpreport.py --check [--tree DIR]   exit 1 when DIR's report differs from DIR's figures and rows

DIR is a public tree: the public repository's checkout (the default: the tree this file sits in) or one that
tools/lbppublish.py built. In the private repository there is no public-figures.json: lbppublish.py computes the
figures and the rows from the private proofs and writes the report beside them in the same run (build_report below),
and its --audit runs the same check on the tree it audits, so the files cannot disagree in a published tree. This
command is for looking at, or re-deriving, a tree's report.

WHY. Progress trackers (bytematch.dev, recomp.fyi) read an objdiff progress report that the default branch's
.github/workflows/progress-report.yml uploads as the artifact `UCUS98744_report`. Public CI cannot build this project
(no game, no keys, no compiler), and the accepted proofs behind the figures stay private. So the report is made from the
figures and committed; the workflow only uploads the committed file.

FORMAT. objdiff-core's report (REPORT_VERSION 2) in its JSON form (pbjson with preserve_proto_field_names): snake_case
names, 64-bit counters as decimal strings, 32-bit counters and percentages as numbers. The top-level measures and the
categories (one per module) carry every field; a unit leaves out its zero fields, as objdiff's own reports do. One unit
per line.

UNITS (what bytematch.dev draws as its grid: one tile per unit, one tile per function when a unit is opened), from
data/public-functions.txt, one row per function of the public scope, in this order of precedence:
  <module>/tu-<index>@<first address>   a translation unit of the game recovered from the program's static
                                        constructors (data/translation-units.json, tools/lbptu.py, which also gives the
                                        evidence for each end): all its functions of the public scope, matched or not;
                                        complete when every one is matched, source_path only when they are all in one
                                        published source. The program keeps no file names, so a unit has a number
  src/<dir>/<file>                      a published source with an accepted proof outside any recovered unit: its
                                        function, with metadata.source_path its path
  <module>/not-yet-recovered/FUN_<addr> a function of the public scope in neither; auto_generated, since it is no
                                        source file
Units are in address order; their code and function counts add up to the module figures (`unit_problems` checks it,
and build_report refuses rows that disagree with the figures).

WHAT THE NUMBERS MEAN (honest by construction). The report is always tier T1, so bytes and functions share one basis
(the whole-module build, T2, has no function count; its figure is the README headline, not the trackers').
  total_code / matched_code      the module's public scope in bytes (the game's own code and the open-source libraries
                                 it links; Sony SDK code is outside it) / the bytes of it owned by accepted proofs of
                                 published sources (public_t1_bytes): functions that match exactly in isolation
  total_functions / matched_...  the public scope's functions / those of them with such a proof
                                 (public_scope_functions, public_t1_functions)
  fuzzy_match_percent            equal to matched_code_percent: near misses are not counted
  total_data / matched_data      0 and 0: data is not a progress claim here
  complete_code / complete_units the code and number of recovered units whose every function is matched
The report carries what the public tree already publishes: function names (FUN_<address> or a source's header name),
addresses and sizes (data/functions-EBOOT.BIN.txt), and source paths. No game bytes, disassembly or decompiler output.
"""
from __future__ import annotations

import argparse
import bisect
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FIGURES_REL = "data/public-figures.json"
ROWS_REL = "data/public-functions.txt"
TU_REL = "data/translation-units.json"
REPORT_REL = "docs/objdiff-report.json"
FIGURES_PATH = ROOT / FIGURES_REL
ROWS_PATH = ROOT / ROWS_REL
REPORT_PATH = ROOT / REPORT_REL
REPORT_VERSION = 2
NOT_RECOVERED = "not-yet-recovered"
ROWS_HEADER = ("# The public scope's functions (tools/lbppublish.py, tools/lbpreport.py): module, start (hex), size, "
               "matched bytes, source, name\n"
               "# source and name are - for a function no published source matches yet\n")


def pct(part: int, whole: int) -> float:
    return round(100 * part / whole, 3) if whole else 0.0


def measures(total_code: int, matched_code: int, total_functions: int, matched_functions: int, units: int,
             complete_code: int = 0, complete_units: int = 0) -> dict:
    """One objdiff `Measures` message (JSON form): u64 as strings, u32 and floats as numbers."""
    matched_pct = pct(matched_code, total_code)
    return {"fuzzy_match_percent": matched_pct,
            "total_code": str(total_code), "matched_code": str(matched_code), "matched_code_percent": matched_pct,
            "total_data": "0", "matched_data": "0", "matched_data_percent": 0.0,
            "total_functions": total_functions, "matched_functions": matched_functions,
            "matched_functions_percent": pct(matched_functions, total_functions),
            "complete_code": str(complete_code), "complete_code_percent": pct(complete_code, total_code),
            "complete_data": "0", "complete_data_percent": 0.0,
            "total_units": units, "complete_units": complete_units}


def unit_measures(code: int, matched_code: int, functions: int, matched_functions: int,
                  complete: bool = False) -> dict:
    """A unit's `Measures` without its zero fields (pbjson leaves defaults out)."""
    return {k: v for k, v in measures(code, matched_code, functions, matched_functions, 0,
                                      code if complete else 0, int(complete)).items()
            if v not in (0, 0.0, "0")}


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


# --------------------------------------------------------------------------------------------------------------
# rows: data/public-functions.txt

def render_rows(rows: dict[str, list[tuple]]) -> str:
    """The rows file: `module start size matched source name`, start in hex, `-` for no source."""
    out = [ROWS_HEADER]
    for module in sorted(rows):
        for start, size, matched, source, name in rows[module]:
            out.append(f"{module} {start:08x} {size} {matched} {source or '-'} {name or '-'}\n")
    return "".join(out)


def parse_rows(text: str) -> dict[str, list[tuple]]:
    rows: dict[str, list[tuple]] = {}
    for line in text.splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        module, start, size, matched, source, name = line.split()
        rows.setdefault(module, []).append((int(start, 16), int(size), int(matched),
                                            None if source == "-" else source, None if name == "-" else name))
    return rows


def load_rows(path: Path = ROWS_PATH) -> dict[str, list[tuple]] | None:
    return parse_rows(path.read_text(encoding="utf-8")) if path.exists() else None


def function_item(name: str, start: int, size: int, matched: int) -> dict:
    item = {"name": name, "size": str(size)}
    if matched:
        item["fuzzy_match_percent"] = pct(matched, size)
    item["metadata"] = {"virtual_address": str(start)}
    item["address"] = str(start)
    return item


def read_units(path: Path) -> dict[str, list[tuple[int, int, str]]]:
    """{module: [(first, last, name)]} from data/translation-units.json ({} when there is none)."""
    if not path.exists():
        return {}
    doc = json.loads(path.read_text(encoding="utf-8"))
    return {m: sorted((int(u["first"], 16), int(u["last"], 16), u["name"]) for u in us)
            for m, us in doc["modules"].items()}


def unit_of(units: list[tuple[int, int, str]], start: int) -> str | None:
    """The recovered unit holding the function at `start`, else None."""
    i = bisect.bisect_right(units, (start, 0xFFFFFFFF)) - 1
    return units[i][2] if i >= 0 and units[i][0] <= start <= units[i][1] else None


def module_units(module: str, rows: list[tuple], tus: list[tuple[int, int, str]] | None = None) -> list[dict]:
    """The units of one module from its rows and its recovered units (see UNITS above), in address order."""
    by_source: dict[str, list[tuple]] = {}
    in_tu: dict[str, list[tuple]] = {}
    keyed: list[tuple[int, dict]] = []
    for row in sorted(rows):
        start, size, _matched, source, _name = row
        tu = unit_of(tus, start) if tus else None
        if tu:
            in_tu.setdefault(tu, []).append(row)
        elif source:
            by_source.setdefault(source, []).append(row)
        else:
            fname = f"FUN_{start:08x}"
            keyed.append((start, {"name": f"{module}/{NOT_RECOVERED}/{fname}",
                                  "measures": unit_measures(size, 0, 1, 0),
                                  "functions": [function_item(fname, start, size, 0)],
                                  "metadata": {"complete": False, "module_name": module,
                                               "progress_categories": [module], "auto_generated": True}}))
    for tu, fs in in_tu.items():
        code, done = sum(r[1] for r in fs), sum(r[2] for r in fs)
        matched = [r for r in fs if r[3]]
        complete = len(matched) == len(fs)
        meta = {"complete": complete, "module_name": module, "progress_categories": [module]}
        paths = {r[3] for r in matched}
        if complete and len(paths) == 1:
            meta["source_path"] = paths.pop()
        keyed.append((fs[0][0], {"name": f"{module}/{tu}",
                                 "measures": unit_measures(code, done, len(fs), len(matched), complete),
                                 "functions": [function_item(n or f"FUN_{s:08x}", s, size, m)
                                               for s, size, m, _src, n in fs],
                                 "metadata": meta}))
    for source, fs in by_source.items():
        code, done = sum(r[1] for r in fs), sum(r[2] for r in fs)
        keyed.append((fs[0][0], {"name": source, "measures": unit_measures(code, done, len(fs), len(fs)),
                                 "functions": [function_item(n or f"FUN_{s:08x}", s, size, m)
                                               for s, size, m, _src, n in fs],
                                 "metadata": {"complete": False, "module_name": module, "source_path": source,
                                              "progress_categories": [module]}}))
    return [u for _s, u in sorted(keyed, key=lambda t: t[0])]


def build_report(figures: dict, rows: dict[str, list[tuple]] | None = None,
                 tus: dict[str, list[tuple[int, int, str]]] | None = None) -> dict:
    """The report for the content of data/public-figures.json and, when given, data/public-functions.txt (without the
    rows, one unit per module) and data/translation-units.json (read_units). Refuses rows that do not add up to the
    figures."""
    units, categories = [], []
    sums = [0, 0, 0, 0]
    complete = [0, 0]
    for name, fig in figures.items():
        row = module_figure(name, fig)
        if rows is None:
            mod_units = [{"name": name, "measures": measures(*row, 1),
                          "metadata": {"module_name": name, "progress_categories": [name]}}]
        else:
            mine = rows.get(name, [])
            got = (sum(r[1] for r in mine), sum(r[2] for r in mine), len(mine), sum(1 for r in mine if r[3]))
            if got != row:
                raise ValueError(f"{name}: the rows give {got} (scope bytes, matched bytes, functions, matched "
                                 f"functions), the figures {row}")
            mod_units = module_units(name, mine, (tus or {}).get(name))
        done = [u for u in mod_units if u["metadata"].get("complete")]
        mine_complete = [sum(int(u["measures"]["total_code"]) for u in done), len(done)]
        m = measures(*row, len(mod_units), *mine_complete)
        units.extend(mod_units)
        categories.append({"id": name, "name": name, "measures": m})
        sums = [a + b for a, b in zip(sums, row)]
        complete = [a + b for a, b in zip(complete, mine_complete)]
    return {"measures": measures(*sums, len(units), *complete), "units": units, "version": REPORT_VERSION,
            "categories": categories}


def unit_problems(report: dict) -> list[str]:
    """Why the units do not add up to their module (category) figures ([] when they do)."""
    out = []
    for c in report["categories"]:
        us = [u for u in report["units"] if u["metadata"]["module_name"] == c["id"]]
        m = c["measures"]
        for key in ("total_code", "matched_code"):
            got = sum(int(u["measures"].get(key, "0")) for u in us)
            if got != int(m[key]):
                out.append(f"{c['id']}: units' {key} {got}, module {m[key]}")
        for key in ("total_functions", "matched_functions"):
            got = sum(u["measures"].get(key, 0) for u in us)
            if got != m[key]:
                out.append(f"{c['id']}: units' {key} {got}, module {m[key]}")
        if len(us) != m["total_units"]:
            out.append(f"{c['id']}: {len(us)} units, module total_units {m['total_units']}")
        for u in us:
            fs = u.get("functions", [])
            if fs and (len(fs) != u["measures"].get("total_functions", 0)
                       or sum(int(f["size"]) for f in fs) != int(u["measures"].get("total_code", "0"))):
                out.append(f"{u['name']}: its functions do not add up to its measures")
    names = [u["name"] for u in report["units"]]
    if len(set(names)) != len(names):
        out.append("two units share a name")
    return out


def render(report: dict) -> str:
    """JSON with one unit per line (a day's change touches only the units that changed)."""
    def dump(v: object) -> str:
        return json.dumps(v, separators=(",", ":"))
    return ("{\n" + f'"measures":{dump(report["measures"])},\n"units":[\n'
            + ",\n".join(dump(u) for u in report["units"])
            + f'\n],\n"version":{dump(report["version"])},\n"categories":[\n'
            + ",\n".join(dump(c) for c in report["categories"]) + "\n]\n}\n")


def load_figures(path: Path = FIGURES_PATH) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def differences(figures: dict, path: Path = REPORT_PATH, rows: dict[str, list[tuple]] | None = None,
                tus: dict[str, list[tuple[int, int, str]]] | None = None) -> list[str]:
    """Why the report at `path` does not match the figures, rows and units (an empty list when it does)."""
    if not path.exists():
        return [f"{path.name} does not exist"]
    want = build_report(figures, rows, tus)
    text = path.read_text(encoding="utf-8")
    try:
        have = json.loads(text)
    except ValueError as exc:
        return [f"{path.name} is not valid JSON: {exc}"]
    if have == want and text == render(want):
        return []
    h, w = have.get("measures", {}), want["measures"]
    out = [f"{path.name} {k}: {h.get(k)}, figures say {w[k]}"
           for k in ("total_code", "matched_code", "matched_code_percent", "total_functions", "matched_functions",
                     "total_units")
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
    rows = load_rows(args.tree / ROWS_REL)
    tus = read_units(args.tree / TU_REL)
    report = build_report(figures, rows, tus)
    total = report["measures"]
    diffs: list[str] = []
    if args.check:
        diffs = differences(figures, report_path, rows, tus)
        print("objdiff report: " + ("agrees with data/public-figures.json" if not diffs else "DIFFERS"))
        for d in diffs:
            print("  " + d)
    else:
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(render(report), encoding="utf-8", newline="\n")
        print(f"wrote {REPORT_REL}: {total['matched_code']}/{total['total_code']} code bytes "
              f"({total['matched_code_percent']:.3f} %), {total['matched_functions']}/{total['total_functions']} "
              f"functions, {total['total_units']} units")
    return 1 if diffs else 0


if __name__ == "__main__":
    raise SystemExit(main())
