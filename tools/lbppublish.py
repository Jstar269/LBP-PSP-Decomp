#!/usr/bin/env python3
"""Build the would-be public tree: the tracked files minus everything that must not leave the private repository.

    python tools/lbppublish.py --out <new or empty directory> [--list]
    python tools/lbppublish.py --audit <tree>        check a built tree (exit 1 on a refusal)

Copies every file git tracks (`git ls-files`) into --out, byte for byte, except (each reason is counted):

  * sources (src/, wip/) whose origin class is in EXCLUDED_CLASSES (tools/lbporigin.py, computed fresh: Sony SDK
    libraries, Sony crt0, Sony system modules, cross-project copies of hst-decomp Sony SDK functions, code not
    attributed yet). Matched newlib, zlib, libgcc and GNU libstdc++ sources are open-source and stay;
  * DISC_DERIVED files: tables made from the retail executable (function boundary lists, the compiler sample, the
    target and toolchain manifests with retail digests and machine paths, the accepted ledger, which also names
    excluded sources). A user regenerates the boundary lists from their own disc (tools/lbpfuncs.py). The symbol map
    (symbols/, names at addresses) is published (owner decision 2026-10-28; set PUBLISH_SYMBOL_MAP);
  * PRIVATE_PATHS: maintainer-only files (state and hand-over notes with machine paths, the external-lane tooling
    and its notes, files that state this tree's private rules);
  * files git tracks although .gitignore ignores them (added by force).

Written in addition: ORIGINS.json (the classes of the copied sources only), the public versions of README.md and
AGENTS.md (PUBLIC_FILES, from docs/public/), data/public-figures.json (below) and docs/objdiff-report.json, which is
a pure function of those figures (tools/lbpreport.py build_report: the objdiff report progress trackers read, counts
and percentages only). The report is not tracked in this repository: it is made from the figures in the same run, and
--audit refuses a tree whose report differs from its figures. The figure blocks of the public README
(`<!-- lbpfigures:begin NAME -->`, render_blocks) are filled from the same figures, dated by the newest accepted proof
they count, and --audit refuses a README whose blocks disagree. Nothing else is generated and no file is edited.

Public figures follow hst-decomp's rule (docs/COMPLETION.md): Sony code kept out of the public tree is not counted in
the public denominator; it stays in the strict scope. Public scope = the game region and the libstdc++ region of
data/origin-regions.json plus every function a published open-source source covers, minus the functions whose
matched source is held back (Sony code by its class, wherever it sits). T1 numerator = bytes owned by accepted proofs
(data/accepted-ledger.jsonl, tools/lbpproof.py) whose source is published. T2 (the whole-module build,
tools/lbpbuild.py) is read from the public build's report (build/whole/<module>/public/report.json) and counted only
when that build was byte-identical to the retail module and made from the current accepted ledger: public_t2_bytes =
the code bytes of the public scope that published sources produce in that identical image (strict: scaffolding never
counts); the data bytes they produce and their share of the whole image are given beside it. T3 is not measured. The same scope and ledger rules give the function counts: public_scope_functions = the functions
of the public scope, public_t1_functions = those of them with an accepted proof of a published source (counts only).

--audit scans a built tree for: machine or private paths and user names; key-like material; source files whose
class is not publishable or that ORIGINS.json does not list; files the policy excludes. It also counts, without
refusing, decompiler-style text (Ghidra typedefs in mechanical translations), disassembly-like lines in docs, and the
cross-project clones of hst-decomp sources it publishes (by class).

This tool never touches a git remote and never uses the network: the only git command it runs is the local, read-only
`git ls-files`. It writes only inside --out.
"""
from __future__ import annotations

import argparse
import bisect
import json
import os
import re
import shutil
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbporigin
import lbpreport

ROOT = lbporigin.ROOT

# ---------------------------------------------------------------------------------------------------------------
# PUBLISH POLICY (owner decisions 2026-10-28): Sony SDK code never leaves (the game-region copies of hst-decomp SDK
# functions are held back too, class sony-sdk); open-source libraries do (newlib, zlib, libgcc, libstdc++); the
# unattributed gap stays out for now; mechanical translations are published as they are; the symbol map is published.
EXCLUDED_CLASSES = ("startup", "sony-sdk", "sony-prx", "unattributed")
PUBLISH_SYMBOL_MAP = True
DISC_DERIVED = ("data/functions-EBOOT.BIN.txt", "data/functions-pspnet_ap_dialog_dummy.prx.txt",
                "data/cceval-sample.json", "data/target-manifest.json", "data/toolchain-manifest.json",
                "data/accepted-ledger.jsonl", "data/accepted-journal.jsonl")
SYMBOL_MAPS = ("symbols/",)
PRIVATE_PATHS = ("docs/HANDOFF.md", "docs/LANE-BRIEF.md", "docs/LANE-NOTES.md", "AGENTS.md",
                 "tools/lbplane.py", "tools/test_lbplane.py",
                 "docs/public/")
# public versions of files whose private version states this tree's private rules: source -> published path
PUBLIC_FILES = {"docs/public/README.md": "README.md", "docs/public/AGENTS.md": "AGENTS.md"}
# ---------------------------------------------------------------------------------------------------------------

MANIFEST = "ORIGINS.json"
FIGURES = "data/public-figures.json"
RUN_DATE = re.compile(r"\d{8}T\d{6}Z")      # a proof's run id starts with its UTC time (tools/lbpproof.py)
# machine paths of the maintainers' PCs and user names; the public repository URLs of the owner's projects
# (github.com/Jstar269/..., the bytematch.dev page of the same name), the public copyright line ("Jstar269 and
# contributors", as hst-decomp) and the maintainer's public handle in .recomp.json ("name": "Jstar269") are allowed
PRIVATE_TEXT = re.compile(r"(?i)[a-z]:[\\/]+(?:nk\b|users[\\/]+jstar|psvitabackup|pspzip|all_psp_games)|"
                          r"/mnt/[a-z]/(?:nk\b|users/jstar)|home[\\/]+jstar\b|"
                          r"(?<!github\.com/)(?<!bytematch\.dev/)(?<!\x22name\x22: \x22)\bjstar(?:269)?\b(?! and contributors)|"
                          r"psp-content-universal|nakagawa-recomp-history-private|"
                          r"\bprivate[\\/]+(?:hst-|lbp-|compilers|campaign-builds|regression|worktrees)")
AUDIT_SELF = ("tools/lbppublish.py", "tools/test_lbppublish.py")   # they hold the patterns and fixtures themselves
KEY_TEXT = re.compile(r"-----BEGIN [A-Z ]*PRIVATE KEY|(?i:\b(?:api[_-]?key|secret|token|password)\s*[:=]\s*\S{8,})|"
                      r"\b[0-9a-fA-F]{32}\b(?![0-9a-fA-F])")
DIGEST_CONTEXT = re.compile(r"(?i)sha-?256|sha-?1|md5|digest|hash|nid|0x[0-9a-f]{8}")
GHIDRA_TEXT = re.compile(r"typedef unsigned char undefined;|WARNING: |undefined4\b")
DISASM_LINE = re.compile(r"^\s*(?:[0-9a-f]{8}\s+){1,2}(?:addiu|lui|lw|sw|jal|jr|ins|ext|beqz|bnez|move)\b", re.M)


def git_lines(*args: str) -> list[str]:
    """A local read-only `git ls-files` variant in the repository root; the NUL-separated paths."""
    assert args and args[0] == "ls-files", "lbppublish runs git ls-files only"
    out = subprocess.run(["git", *args, "-z"], cwd=ROOT, check=True, capture_output=True,
                         env={**os.environ, "GIT_TERMINAL_PROMPT": "0"}).stdout
    return [p.decode("utf-8") for p in out.split(b"\0") if p]


def matches(path: str, patterns: tuple[str, ...]) -> bool:
    return any(path == q or q.endswith("/") and path.startswith(q) for q in patterns)


def plan(tracked: list[str], ignored: set[str], classes: dict[str, str], with_symbols: bool = PUBLISH_SYMBOL_MAP,
         root: Path = ROOT) -> tuple[list[str], dict[str, list[str]]]:
    """(kept paths, {reason: excluded paths})."""
    keep: list[str] = []
    excluded: dict[str, list[str]] = defaultdict(list)
    for path in tracked:
        if path in ignored:
            excluded["gitignored (tracked by force)"].append(path)
        elif matches(path, PRIVATE_PATHS):
            excluded["maintainer-only path (PRIVATE_PATHS)"].append(path)
        elif matches(path, DISC_DERIVED):
            excluded["disc-derived table (DISC_DERIVED)"].append(path)
        elif not with_symbols and matches(path, SYMBOL_MAPS):
            excluded["symbol map (PUBLISH_SYMBOL_MAP is off)"].append(path)
        elif classes.get(path) in EXCLUDED_CLASSES:
            excluded[f"origin class {classes[path]}"].append(path)
        elif path.split("/")[0] in lbporigin.SOURCE_TREES and path.endswith(lbporigin.SOURCE_SUFFIXES) \
                and path not in classes:
            excluded["source without an origin class"].append(path)
        elif not (root / path).is_file():
            excluded["missing in the work tree"].append(path)
        else:
            keep.append(path)
    return keep, excluded


def functions(root: Path, module: str) -> dict[int, int]:
    """The inventory (ledger functions of the campaign) as {start: end}; empty when the campaign is unavailable."""
    import lbpmatch as M
    try:
        ledger = json.loads((M.campaign_root() / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    except (OSError, SystemExit, ValueError):
        return {}
    mod = next((m for m in ledger["modules"] if m["name"] == module), None)
    return {f["start"]: f["end"] for f in mod["functions"]} if mod else {}


def overlap_bytes(ranges: list[tuple[int, int]], scope: list[tuple[int, int]]) -> int:
    """Bytes of `ranges` inside `scope` (both lists of [start, end); `scope` sorted and disjoint)."""
    starts = [a for a, _b in scope]
    total = 0
    for s, e in ranges:
        k = max(bisect.bisect_right(starts, s) - 1, 0)
        while k < len(scope) and scope[k][0] < e:
            total += max(0, min(e, scope[k][1]) - max(s, scope[k][0]))
            k += 1
    return total


def public_t2(report: dict, kept_sources: set[str], scope: list[tuple[int, int]], scope_bytes: int) -> dict:
    """The public T2 figures of one module from its public whole-module build report (tools/lbpbuild.py load_report:
    identical image, current ledger): code bytes of the public scope that published sources produce, their data bytes,
    and their share of the whole image."""
    code: list[tuple[int, int]] = []
    data = image = 0
    for path, rows in report.get("t2_source_ranges", {}).items():
        if path not in kept_sources:
            continue
        image += report["t2_source_bytes"].get(path, 0)
        for start, end, kind in rows:
            s, e = int(start, 16), int(end, 16)
            if kind == "source-code":
                code.append((s, e))
            else:
                data += e - s
    done = overlap_bytes(sorted(code), scope)
    return {"public_t2_bytes": done, "public_t2_percent": round(100.0 * done / scope_bytes, 3) if scope_bytes else 0.0,
            "public_t2_data_bytes": data, "image_bytes": report["image_bytes"], "image_identical": True,
            "image_t2_bytes": image, "image_t2_percent": round(100.0 * image / report["image_bytes"], 3),
            "method": "tools/lbpbuild.py public build: bytes the compiled objects of published accepted sources produce "
                      "in the whole-module image, which is byte-identical to the retail module; scaffolding never counts"}


def figures(kept_sources: set[str], classes: dict[str, str], regions: dict, inventory: dict[str, dict[int, int]],
            proofs: list[dict], t2_reports: dict[str, dict | str] | None = None) -> dict:
    """Public T1 figures (see the module docstring) per module, from the inventory and the accepted proofs, and T2 from
    `t2_reports` ({module: public build report, or the reason there is none}; tools/lbpbuild.py load_report)."""
    out = {}
    published_open = defaultdict(set)   # module -> starts covered by published open-source sources
    owned = defaultdict(dict)           # module -> {start: (end, source)} for proofs of published sources
    held = defaultdict(set)             # module -> starts whose matched source is held back as Sony code
    run = defaultdict(dict)             # module -> {start: the proof's run id, "YYYYMMDDTHHMMSSZ-..."}
    for p in proofs:
        if p.get("status") == "EXACT" and classes.get(p.get("source")) in EXCLUDED_CLASSES:
            held[p["module"]].add(int(p["start"], 16))
        if p.get("source") in kept_sources and p.get("status") == "EXACT":
            s, e = int(p["start"], 16), int(p["end"], 16)
            owned[p["module"]][s] = (e, p["source"])
            if RUN_DATE.match(str(p.get("run", ""))):
                run[p["module"]][s] = p["run"]
            if classes.get(p["source"]) in ("newlib", "zlib", "libgcc"):
                published_open[p["module"]].add(s)
    for module, inv in inventory.items():
        total = sum(e - s for s, e in inv.items())
        scope = {s for s in inv
                 if (lbporigin.region_class(regions, module, s) in ("game", "libstdc++") and s not in held[module])
                 or s in published_open[module]}
        scope_bytes = sum(inv[s] - s for s in scope)
        done = sum(min(e, inv[s]) - s for s, (e, _src) in owned[module].items() if s in scope)
        done_functions = sum(1 for s in owned[module] if s in scope)
        runs = [run[module][s] for s in owned[module] if s in scope and run[module].get(s)]
        by_region = defaultdict(int)
        for s, e in inv.items():
            by_region[lbporigin.region_class(regions, module, s)] += e - s
        rep = (t2_reports or {}).get(module, "not measured (no whole-module build report)")
        t2 = public_t2(rep, kept_sources, sorted((s, inv[s]) for s in scope), scope_bytes) if isinstance(rep, dict) \
            else rep
        out[module] = {"code_bytes": total, "public_scope_bytes": scope_bytes,
                       "public_scope_functions": len(scope), "public_t1_functions": done_functions,
                       "public_t1_bytes": done, "public_t1_percent": round(100.0 * done / scope_bytes, 3)
                       if scope_bytes else 0.0,
                       "strict_scope_bytes": total, "code_bytes_by_region_class": dict(sorted(by_region.items())),
                       "t2": t2, "t3": "not measured"}
        if runs:    # the newest accepted proof the figures count: they change only when the ledger does
            r = max(runs)
            out[module]["ledger_date"] = f"{r[0:4]}-{r[4:6]}-{r[6:8]}"
    return out


# ---------------------------------------------------------------------------------------------------------------
# generated figure blocks of the public README (docs/public/README.md): `<!-- lbpfigures:begin NAME -->` ...
# `<!-- lbpfigures:end -->`. build() fills them from the figures it writes; --audit refuses a tree whose README blocks
# differ from its data/public-figures.json. The layout follows hst-decomp's README (tools/hstprogress.py), so the two
# projects show progress the same way.

def render_blocks(figs: dict) -> dict[str, str]:
    """{block name: text} for the public README, a pure function of the public figures."""
    tot = {k: sum(f[k] for f in figs.values()) for k in ("public_scope_bytes", "public_scope_functions",
                                                          "public_t1_bytes", "public_t1_functions")}
    t2s = [f["t2"] for f in figs.values()]
    t2 = sum(t["public_t2_bytes"] for t in t2s) if t2s and all(isinstance(t, dict) for t in t2s) else None
    pct = lambda part, whole: 100.0 * part / whole if whole else 0.0      # noqa: E731
    dates = sorted(f["ledger_date"] for f in figs.values() if f.get("ledger_date"))
    measured = (f"Measured from the accepted proofs of {dates[-1]}" if dates else "Measured") \
        + " with `python tools/lbppublish.py`"
    built = (f"| **Built from source** (T2) | **{pct(t2, tot['public_scope_bytes']):.1f}%** | The share of that code "
             "which the whole executable, rebuilt in place from this source, reproduces byte for byte; the rebuilt "
             "file is identical to the original. |") if t2 is not None else \
        ("| **Built from source** (T2) | not measured | No whole-module build of the current accepted proofs yet "
         "(`python tools/lbpbuild.py`). |")
    glance = "\n".join([
        "| | Progress | What it counts |",
        "|---|---|---|",
        f"| **Code matched** (T1) | **{pct(tot['public_t1_bytes'], tot['public_scope_bytes']):.1f}%** | "
        f"{tot['public_t1_bytes']:,} of the {tot['public_scope_bytes']:,} code bytes of the public scope come from C or "
        "C++ that compiles, function by function, to the game's exact bytes. |",
        f"| **Functions matched** (T1) | **{pct(tot['public_t1_functions'], tot['public_scope_functions']):.1f}%** | "
        f"{tot['public_t1_functions']:,} of the {tot['public_scope_functions']:,} functions of the public scope. |",
        built,
        "",
        "Code and functions matched are the figures the trackers "
        "([bytematch.dev](https://bytematch.dev/Jstar269/lbp-psp-decomp), [recomp.fyi](https://recomp.fyi)) show. "
        "The public scope is the game's own code and the open-source libraries it links; Sony SDK code the game links "
        "is not published and not counted here, but stays in the project's strict finish line "
        f"([`docs/COMPLETION.md`](docs/COMPLETION.md)). {measured}."])
    rows = [f"| `{m}` | {f['public_t1_functions']:,} / {f['public_scope_functions']:,} | "
            f"{f['public_t1_bytes']:,} / {f['public_scope_bytes']:,} | "
            f"{pct(f['public_t1_bytes'], f['public_scope_bytes']):.3f} % |" for m, f in figs.items()]
    t2_lines = [f"`{m}` built from source (T2): " + (
        f"{f['t2']['public_t2_bytes']:,} code bytes of the public scope ({f['t2']['public_t2_percent']:.3f} %), and "
        f"{f['t2']['public_t2_data_bytes']:,} data bytes; the rebuilt file is byte-identical to the original."
        if isinstance(f["t2"], dict) else f"{f['t2']}.") for m, f in figs.items()]
    detail = "\n".join([
        f"**{tot['public_t1_functions']:,} functions** matched exactly in isolation (T1), {measured[0].lower()}"
        f"{measured[1:]} and counted in code bytes of the public scope:",
        "",
        "| Module | Matched / functions | Matched / code bytes | Share of bytes |",
        "|---|---|---|---|",
        *rows,
        "",
        *t2_lines])
    return {"readme-glance": glance, "readme-progress": detail}


README_BLOCK = re.compile(r"<!-- lbpfigures:begin ([\w-]+) -->\n\n(.*?)\n\n<!-- lbpfigures:end -->", re.S)


def fill_blocks(text: str, blocks: dict[str, str]) -> str:
    """`text` with every lbpfigures block replaced by its rendering (a block with no rendering is left as it is)."""
    return README_BLOCK.sub(lambda m: f"<!-- lbpfigures:begin {m.group(1)} -->\n\n{blocks.get(m.group(1), m.group(2))}"
                                      "\n\n<!-- lbpfigures:end -->", text)


def block_differences(text: str, figs: dict) -> list[str]:
    want = render_blocks(figs)
    have = {m.group(1): m.group(2) for m in README_BLOCK.finditer(text)}
    return [f"README.md: block {name} " + ("is missing" if name not in have else "differs from the figures")
            for name in want if have.get(name) != want[name]] if have else []


def build(out: Path, list_all: bool) -> int:
    out = out.resolve()
    if out == ROOT or ROOT in out.parents or out in ROOT.parents:
        raise SystemExit(f"refusing --out {out}: it must be outside the repository")
    if out.exists() and (not out.is_dir() or any(out.iterdir())):
        raise SystemExit(f"refusing --out {out}: it exists and is not empty")
    tracked = git_lines("ls-files")
    ignored = set(git_lines("ls-files", "-c", "-i", "--exclude-standard"))
    classes, warnings = lbporigin.classify(ROOT)
    for w in warnings:
        print(f"warning: {w}", file=sys.stderr)
    keep, excluded = plan(tracked, ignored, classes)
    out.mkdir(parents=True, exist_ok=True)
    copied = 0
    for path in keep:
        dest = out / path
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / path, dest)
        copied += dest.stat().st_size
    kept_set = set(keep)
    kept_classes = {p: c for p, c in classes.items() if p in kept_set}
    (out / MANIFEST).write_text(json.dumps(kept_classes, indent=0, sort_keys=True) + "\n", encoding="utf-8",
                                newline="\n")
    for src_path, dest_path in PUBLIC_FILES.items():
        if (ROOT / src_path).is_file():
            shutil.copyfile(ROOT / src_path, out / dest_path)
    regions = lbporigin.load_regions(ROOT / "data" / "origin-regions.json")
    inventory = {m: functions(ROOT, m) for m in regions}
    ledger = ROOT / "data" / "accepted-ledger.jsonl"
    proofs = [json.loads(ln) for ln in ledger.read_text(encoding="utf-8").splitlines()[1:]] if ledger.exists() else []
    import lbpbuild
    t2_reports = {m: lbpbuild.load_report(m, "public") for m, i in inventory.items() if i}
    figs = figures({p for p in kept_classes}, classes, regions, {m: i for m, i in inventory.items() if i}, proofs,
                   t2_reports)
    (out / FIGURES).parent.mkdir(parents=True, exist_ok=True)
    (out / FIGURES).write_text(json.dumps(figs, indent=1, sort_keys=True) + "\n", encoding="utf-8", newline="\n")
    readme = out / "README.md"
    if readme.is_file():
        readme.write_text(fill_blocks(readme.read_text(encoding="utf-8"), render_blocks(figs)), encoding="utf-8",
                          newline="\n")
    (out / lbpreport.REPORT_REL).parent.mkdir(parents=True, exist_ok=True)
    (out / lbpreport.REPORT_REL).write_text(lbpreport.render(lbpreport.build_report(figs)), encoding="utf-8",
                                            newline="\n")
    print(f"public tree: {out}")
    print(f"tracked files: {len(tracked)}; copied: {len(keep)} ({copied} bytes); "
          f"excluded: {sum(len(v) for v in excluded.values())}")
    for reason, paths in sorted(excluded.items()):
        size = sum((ROOT / p).stat().st_size for p in paths if (ROOT / p).is_file())
        print(f"  excluded {len(paths):5d} files, {size:10d} bytes: {reason}")
        if list_all:
            for p in paths:
                print(f"    {p}")
    counts = defaultdict(int)
    for c in kept_classes.values():
        counts[c] += 1
    print("  sources kept by class: " + ", ".join(f"{c} {counts[c]}" for c in lbporigin.CLASSES if counts[c]))
    for module, f in figs.items():
        print(f"  {module}: public T1 {f['public_t1_bytes']}/{f['public_scope_bytes']} B = {f['public_t1_percent']}% "
              f"({f['public_t1_functions']}/{f['public_scope_functions']} functions; strict scope "
              f"{f['strict_scope_bytes']} B)")
        t2 = f["t2"]
        print(f"  {module}: public T2 " + (f"{t2['public_t2_bytes']}/{f['public_scope_bytes']} B = {t2['public_t2_percent']}% "
                                           f"(+{t2['public_t2_data_bytes']} data B; {t2['image_t2_percent']}% of the "
                                           "identical image)" if isinstance(t2, dict) else t2))
    return 0


def audit(tree: Path) -> int:
    tree = tree.resolve()
    origins = json.loads((tree / MANIFEST).read_text(encoding="utf-8")) if (tree / MANIFEST).exists() else {}
    refusals: list[str] = []
    notes: dict[str, list[str]] = defaultdict(list)
    files = sorted(p for p in tree.rglob("*") if p.is_file() and ".git" not in p.relative_to(tree).parts)
    for p in files:
        rel = p.relative_to(tree).as_posix()
        if (matches(rel, PRIVATE_PATHS) or matches(rel, DISC_DERIVED)) and rel not in PUBLIC_FILES.values():
            refusals.append(f"{rel}: excluded by the policy")
        text = p.read_text(encoding="utf-8", errors="replace")
        for n, line in enumerate(text.splitlines() if rel not in AUDIT_SELF else [], 1):
            if PRIVATE_TEXT.search(line):
                refusals.append(f"{rel}:{n}: private path or user name: {line.strip()[:120]}")
            m = KEY_TEXT.search(line)
            if m and not DIGEST_CONTEXT.search(line):
                refusals.append(f"{rel}:{n}: key-like text: {line.strip()[:120]}")
        if rel.split("/")[0] in lbporigin.SOURCE_TREES and rel.endswith(lbporigin.SOURCE_SUFFIXES):
            cls = origins.get(rel)
            if cls is None or cls in EXCLUDED_CLASSES:
                refusals.append(f"{rel}: origin class {cls or 'missing'}")
            if GHIDRA_TEXT.search(text):
                notes["sources with decompiler-style text (Ghidra typedefs; mechanical translations)"].append(rel)
            first = text.split("\n", 2)
            if len(first) > 1 and first[1].startswith("/* Clone of hst-decomp"):
                notes[f"cross-project clones of hst-decomp sources, published as {cls}"].append(rel)
        elif rel.endswith(".md") and DISASM_LINE.search(text):
            notes["docs with disassembly-like lines"].append(rel)
    if (tree / FIGURES).exists() or (tree / lbpreport.REPORT_REL).exists():
        try:
            for d in lbpreport.differences(lbpreport.load_figures(tree / FIGURES), tree / lbpreport.REPORT_REL):
                refusals.append(f"{lbpreport.REPORT_REL}: {d}")
            if (tree / "README.md").is_file():
                refusals += block_differences((tree / "README.md").read_text(encoding="utf-8"),
                                              lbpreport.load_figures(tree / FIGURES))
        except (OSError, ValueError, KeyError) as exc:
            refusals.append(f"{lbpreport.REPORT_REL}: figures and report do not load ({exc!r})")
    for name in ("README.md", "LICENSE", "ORIGINS.json"):
        if not (tree / name).exists():
            notes["missing top-level files"].append(name)
    print(f"audit of {tree}: {len(files)} files")
    for r in refusals[:200]:
        print(f"REFUSED {r}")
    for k, v in sorted(notes.items()):
        print(f"NOTE {k}: {len(v)}" + (f" ({', '.join(v[:6])}{', ...' if len(v) > 6 else ''})" if v else ""))
    print(f"refusals: {len(refusals)}")
    return 1 if refusals else 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, help="destination directory (new or empty), outside the repository")
    ap.add_argument("--list", action="store_true", help="print every excluded path")
    ap.add_argument("--audit", type=Path, metavar="TREE", help="audit a built tree instead of building one")
    args = ap.parse_args()
    if args.audit:
        return audit(args.audit)
    if not args.out:
        ap.error("--out or --audit is required")
    return build(args.out, args.list)


if __name__ == "__main__":
    raise SystemExit(main())
