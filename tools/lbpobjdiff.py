#!/usr/bin/env python3
"""The objdiff project (objdiff.json) and the objects it names, made from the matcher's output and your own copy.

    python tools/lbpobjdiff.py [generate] [--no-wip] [--only GLOB] [--out FILE] [--quiet]
    python tools/lbpobjdiff.py check [--out FILE] [--objdiff-cli PATH] [--random N]

objdiff (https://github.com/encounter/objdiff) shows, per unit, our compiled code next to the original with the
differences marked, and prints a progress report. tools/lbpmatch.py already links every source at its retail address,
so both sides of a unit are fully resolved code at the same address: this tool only repacks bytes into minimal ELF32
little-endian MIPS objects (one code section and its function symbols, no relocations). It follows the design of
hst-decomp's tools/hstobjdiff.py and docs/OBJDIFF.md; the code is this project's own.

generate writes, all under build/objdiff/ (gitignored; the objects hold retail bytes and are never committed):

    units/<source>.base.o    our linked code of one source's range (build/<module>/<key>/linked.elf of the matcher)
    units/<source>.target.o  the retail bytes of the same range, at the same address
    unmatched/<module>.target.o  every function of the module no source covers, against empty.base.o
    lbpobjdiff.json          what was generated: counts, warnings, skipped sources

and objdiff.json in the repository root (gitignored too: it names those objects), which objdiff's GUI and
`objdiff-cli report generate` read. Function boundaries and names on both sides come from the campaign ledger
(FUN_<address>), the list tools/lbpmatch.py and tools/lbpcov.py measure against, so a unit stands for exactly the
ledger functions its source covers (tools/lbpcov.py's rule: the function at the header start and every function
inside the header span) and every ledger function is in exactly one unit. The base side is the header span, or the
whole compiled function when that is longer (what the matcher compares). A wip/ near miss whose function an installed
src/ source already covers is left out (superseded), so no function is counted twice. The project sets objdiff's
instruction category to r4000allegrex: its default MIPS category decodes ext/ins, seb/seh, min/max and the VFPU as
INVALID and compares every INVALID word as equal.

check needs no objdiff and compares the generated project with the ledger and tools/lbpcov.py: every ledger function
in exactly one unit; per module the units' target code equals the ledger's code bytes and the src/ units' bytes equal
lbpcov's exact bytes (presence-based, the sources in src/); every src/ unit's base equals its target and every wip/
unit's differs. With objdiff-cli (--objdiff-cli, or try/objdiff-bin/objdiff-cli[.exe], or on PATH) it also compares
objdiff's own report: total_code and complete_code per module with the same figures, and a sample of src/ units
(--random N, 0 = all) at 100%. Exit 1 on any difference.

Needs your own copy of the game (lbp-config campaign_root) and the matcher's build products: run
`python tools/lbpmatch.py --all --parallel 4` (and the wip/ files) first. A source whose artifact is missing is
reported and skipped, never guessed.
"""
from __future__ import annotations

import argparse
import fnmatch
import json
import random
import shutil
import struct
import subprocess
import sys
from bisect import bisect_left
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402

OUT_DIR_REL = "build/objdiff"
PROJECT_REL = "objdiff.json"
OPTIONS = {"mips_instr_category": "r4000allegrex"}
CLI_CANDIDATES = ("try/objdiff-bin/objdiff-cli.exe", "try/objdiff-bin/objdiff-cli", "objdiff-cli")
DEFAULT_E_FLAGS = 0x10000001        # E_MIPS_ARCH_2 | EF_MIPS_NOREORDER; a unit copies the matcher object's own flags
SHF_EXECINSTR = 4


def elf32(addr: int, code: bytes, symbols: list[tuple[str, int, int]], e_flags: int = DEFAULT_E_FLAGS) -> bytes:
    """A minimal ELF32 little-endian MIPS image (ET_EXEC): `.text` at `addr` holding `code`, and `symbols`
    (name, address, size) as global functions in it. Both sides of a unit are linked images, so nothing else is
    needed: no relocations, no other section."""
    names = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    strtab = bytearray(b"\0")
    symtab = bytearray(16)
    for name, value, size in symbols:
        symtab += struct.pack("<IIIBBH", len(strtab), value, size, 0x12, 0, 1)    # STB_GLOBAL, STT_FUNC, .text
        strtab += name.encode("utf-8") + b"\0"
    text_off = 52
    sym_off = text_off + len(code) + (-len(code)) % 4
    str_off = sym_off + len(symtab)
    names_off = str_off + len(strtab)
    shoff = names_off + len(names) + (-(names_off + len(names))) % 4
    body = bytearray(code) + bytes(sym_off - text_off - len(code)) + symtab + strtab + names
    body += bytes(shoff - 52 - len(body))

    def sh(name: bytes, typ: int, flags: int, a: int, off: int, size: int, link: int = 0, info: int = 0,
           align: int = 1, entsize: int = 0) -> bytes:
        return struct.pack("<IIIIIIIIII", names.index(name) if name else 0, typ, flags, a, off, size, link, info,
                           align, entsize)

    shdrs = (sh(b"", 0, 0, 0, 0, 0) + sh(b".text\0", 1, 0x6, addr, text_off, len(code), align=4)
             + sh(b".symtab\0", 2, 0, 0, sym_off, len(symtab), link=3, info=1, align=4, entsize=16)
             + sh(b".strtab\0", 3, 0, 0, str_off, len(strtab)) + sh(b".shstrtab\0", 3, 0, 0, names_off, len(names)))
    ehdr = b"\x7fELF" + bytes([1, 1, 1]) + bytes(9)
    ehdr += struct.pack("<HHIIIIIHHHHHH", 2, 8, 1, addr, 0, shoff, e_flags, 52, 0, 0, 40, 5, 4)
    return ehdr + bytes(body) + shdrs


def read_text(obj: bytes) -> tuple[int, bytes, list[tuple[str, int, int]]]:
    """(address, code, function symbols) of an object elf32() wrote, read back."""
    secs = M.elf_sections(obj)
    text = dict(secs)[".text"]
    index = [n for n, _ in secs].index(".text")
    syms = [(n, v, s) for n, v, s, i, sh in M.elf_symbols(obj) if sh == index and n]
    return text[3], obj[text[4]:text[4] + text[5]], syms


def covered(functions: list[tuple[int, int]], start: int, end: int) -> list[tuple[int, int]]:
    """The ledger functions a header [start, end) covers (tools/lbpcov.py): the one at its start and every function
    inside the span."""
    starts = [f[0] for f in functions]
    return [(a, b) for a, b in functions[bisect_left(starts, start):] if a < end and (a == start or b <= end)]


def linked_code(linked: bytes, symbol: str, start: int, span: int) -> tuple[bytes, list[str]]:
    """Our bytes of a source's range from the matcher's linked.elf: the header span, or the whole compiled function
    when it is longer, as tools/lbpmatch.py compares them; and warnings."""
    secs = M.elf_sections(linked)
    at = [v for n, v, s, i, sh in M.elf_symbols(linked) if n == symbol and sh != 0]
    if at != [start]:
        raise ValueError(f"{symbol} is not linked once at 0x{start:08x}")
    size = next(s for n, v, s, i, sh in M.elf_symbols(linked) if n == symbol and sh != 0)
    hold = [s for _n, s in secs if s[2] & SHF_EXECINSTR and s[1] == 1 and s[3] <= start < s[3] + s[5]]
    if len(hold) != 1:
        raise ValueError(f"no single linked code section holds 0x{start:08x}")
    sec = hold[0]
    want = max(span, size)
    code = linked[sec[4] + start - sec[3]:sec[4] + min(start - sec[3] + want, sec[5])]
    warnings = []
    if len(code) != span:
        warnings.append(f"compiled function is {size} B, header span {span} B; the unit compares {len(code)} B")
    return code, warnings


def e_flags(obj_path: Path) -> int:
    try:
        head = obj_path.read_bytes()[:0x28]
    except OSError:
        return DEFAULT_E_FLAGS
    return struct.unpack_from("<I", head, 0x24)[0] if len(head) >= 0x28 else DEFAULT_E_FLAGS


def clamp(symbols: list[tuple[str, int, int]], start: int, length: int) -> list[tuple[str, int, int]]:
    """Symbols cut to what an object of `length` bytes at `start` holds (objdiff refuses a symbol past its section)."""
    out = []
    for name, a, size in symbols:
        size = min(size, max(0, start + length - a))
        if size:
            out.append((name, a, size))
    return out


def plan(sources: list[tuple[str, dict]], ledger: dict[str, list[tuple[int, int]]]
         ) -> tuple[list[dict], dict[str, list[tuple[int, int]]], list[str], list[str]]:
    """(units to make, {module: functions no unit covers}, superseded wip drafts, skipped sources) from the parsed
    headers [(relative path, header)]; src/ first, so a wip/ draft of an installed function is superseded."""
    units, superseded, skipped = [], [], []
    taken: dict[str, set[int]] = defaultdict(set)
    for rel, h in sorted(sources, key=lambda t: (not t[0].startswith("src/"), t[0])):
        fns = ledger.get(h["module"])
        if fns is None:
            skipped.append(f"{rel}: unknown module {h['module']}")
            continue
        cover = covered(fns, h["start"], h["end"])
        if not cover:
            skipped.append(f"{rel}: 0x{h['start']:08x} is no ledger function")
            continue
        if any(a in taken[h["module"]] for a, _b in cover):
            (superseded if rel.startswith("wip/") else skipped).append(
                f"{rel}: a function it covers is already in another unit")
            continue
        taken[h["module"]].update(a for a, _b in cover)
        units.append({"source": rel, "header": h, "functions": cover, "complete": rel.startswith("src/")})
    rest = {m: [f for f in fns if f[0] not in taken[m]] for m, fns in ledger.items()}
    return units, rest, superseded, skipped


def unit_name(rel: str) -> str:
    return rel.rsplit(".", 1)[0]


def write_project(root: Path, units: list[dict], rest: dict[str, list[tuple[int, int]]], retail, artifacts,
                  out_dir: Path, project: Path, subset: bool = False) -> dict:
    """Write the unit objects, the unmatched objects (not for a --only subset) and the project file. `retail(module,
    start, size)` gives retail bytes; `artifacts(rel, header)` gives (linked.elf bytes, obj.o path) or None. Returns
    the summary."""
    entries, warnings, skipped = [], [], []
    per_module: dict[str, int] = defaultdict(int)
    (out_dir / "unmatched").mkdir(parents=True, exist_ok=True)
    for u in units:
        rel, h = u["source"], u["header"]
        got = artifacts(rel, h)
        if got is None:
            skipped.append(f"{rel}: no matcher artifact (run tools/lbpmatch.py)")
            continue
        linked, obj = got
        span = h["end"] - h["start"]
        try:
            base, notes = linked_code(linked, h["symbol"], h["start"], span)
        except (ValueError, StopIteration, struct.error) as exc:
            skipped.append(f"{rel}: {exc}")
            continue
        warnings += [f"{rel}: {n}" for n in notes]
        symbols = [(f"FUN_{a:08x}", a, b - a) for a, b in u["functions"]]
        flags = e_flags(obj) if obj else DEFAULT_E_FLAGS
        stem = out_dir / "units" / unit_name(rel)
        stem.parent.mkdir(parents=True, exist_ok=True)
        base_path, target_path = stem.with_name(stem.name + ".base.o"), stem.with_name(stem.name + ".target.o")
        base_path.write_bytes(elf32(h["start"], base, clamp(symbols, h["start"], len(base)), flags))
        target_path.write_bytes(elf32(h["start"], retail(h["module"], h["start"], span), symbols, flags))
        entries.append({"name": unit_name(rel), "path": base_path.relative_to(root).as_posix(),
                        "target_path": target_path.relative_to(root).as_posix(),
                        "metadata": {"complete": u["complete"], "module_name": h["module"],
                                     "progress_categories": [h["module"]], "source_path": rel}})
        per_module[h["module"]] += 1
    empty = out_dir / "empty.base.o"
    empty.write_bytes(elf32(0, b"", []))
    unmatched = {}
    for module, fns in ([] if subset else sorted(rest.items())):
        if not fns:
            continue
        lo, hi = fns[0][0], max(b for _a, b in fns)
        path = out_dir / "unmatched" / f"{module}.target.o"
        path.write_bytes(elf32(lo, retail(module, lo, hi - lo), [(f"FUN_{a:08x}", a, b - a) for a, b in fns]))
        entries.append({"name": f"unmatched/{module}", "path": empty.relative_to(root).as_posix(),
                        "target_path": path.relative_to(root).as_posix(),
                        "metadata": {"auto_generated": True, "module_name": module, "progress_categories": [module]}})
        unmatched[module] = len(fns)
    doc = {"min_version": "2.0.0", "build_base": False, "build_target": False,
           "watch_patterns": ["src/**/*.c", "src/**/*.cpp", "wip/**/*.c", "wip/**/*.cpp", "include/**/*.h"],
           "progress_categories": [{"id": m, "name": m} for m in sorted(rest)], "options": OPTIONS, "units": entries}
    if not subset:
        keep = {root / e["path"] for e in entries} | {root / e["target_path"] for e in entries} | {empty}
        for stale in out_dir.rglob("*.o"):
            if stale not in keep:
                stale.unlink()
    project.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8", newline="\n")
    summary = {"project": project.name, "units": len(entries), "source_units": dict(sorted(per_module.items())),
               "unmatched_functions": unmatched, "warnings": warnings, "skipped": skipped}
    (out_dir / "lbpobjdiff.json").write_text(json.dumps(summary, indent=1) + "\n", encoding="utf-8", newline="\n")
    return summary


# --------------------------------------------------------------------------------------------------------------
# check

def project_problems(root: Path, doc: dict, ledger: dict[str, list[tuple[int, int]]], cov: dict[str, dict]
                     ) -> tuple[list[str], dict[str, dict]]:
    """(problems, {module: totals}) of a generated project against the ledger and tools/lbpcov.py's figures
    ({module: {"code_bytes", "exact_bytes"}})."""
    out = []
    seen: dict[str, dict[int, str]] = defaultdict(dict)
    totals: dict[str, dict] = defaultdict(lambda: {"total_code": 0, "complete_code": 0})
    for u in doc["units"]:
        module = u["metadata"]["module_name"]
        try:
            t_addr, target, t_syms = read_text((root / u["target_path"]).read_bytes())
            b_addr, base, _ = read_text((root / u["path"]).read_bytes())
        except (OSError, KeyError, ValueError, struct.error) as exc:
            out.append(f"{u['name']}: its objects do not load ({exc!r})")
            continue
        for name, a, size in t_syms:
            if a in seen[module]:
                out.append(f"{module} {name}: in {seen[module][a]} and {u['name']}")
            seen[module][a] = u["name"]
            totals[module]["total_code"] += size
        if u["metadata"].get("auto_generated"):
            continue
        same = base[:len(target)] == target and len(base) == len(target) and b_addr == t_addr
        if u["metadata"].get("complete"):
            if same:
                totals[module]["complete_code"] += sum(s for _n, _a, s in t_syms)
            else:
                out.append(f"{u['name']}: an installed source whose linked bytes differ from retail")
        elif same:
            out.append(f"{u['name']}: a near miss whose linked bytes equal retail (promote it)")
    for module, fns in ledger.items():
        missing = [a for a, _b in fns if a not in seen[module]]
        extra = sorted(set(seen[module]) - {a for a, _b in fns})
        if missing:
            out.append(f"{module}: {len(missing)} ledger function(s) in no unit (first 0x{missing[0]:08x})")
        if extra:
            out.append(f"{module}: {len(extra)} unit symbol(s) that are no ledger function (first 0x{extra[0]:08x})")
        row = cov.get(module)
        t = totals[module]
        if row is not None and (t["total_code"], t["complete_code"]) != (row["code_bytes"], row["exact_bytes"]):
            out.append(f"{module}: the project has {t['complete_code']}/{t['total_code']} B, lbpcov.py "
                       f"{row['exact_bytes']}/{row['code_bytes']} B")
    return out, dict(totals)


def report_problems(report: dict, totals: dict[str, dict], sample: list[str]) -> list[str]:
    """Differences between `objdiff-cli report generate`'s report and the project's own totals."""
    out = []
    cats = {c["id"]: c["measures"] for c in report.get("categories", [])}
    for module, t in totals.items():
        m = cats.get(module)
        if m is None:
            out.append(f"objdiff's report has no category {module}")
        elif (int(m.get("total_code", 0)), int(m.get("complete_code", 0))) != (t["total_code"], t["complete_code"]):
            out.append(f"{module}: objdiff reports complete {m.get('complete_code')}/{m.get('total_code')} B, the "
                       f"project {t['complete_code']}/{t['total_code']} B")
    units = {u["name"]: u.get("measures", {}) for u in report.get("units", [])}
    for name in sample:
        m = units.get(name, {})
        if m.get("matched_code") != m.get("total_code"):
            out.append(f"{name}: objdiff matches {m.get('matched_code')}/{m.get('total_code')} B")
    return out


def objdiff_cli(configured: str | None, root: Path = ROOT) -> Path | None:
    for candidate in ([configured] if configured else CLI_CANDIDATES):
        path = Path(candidate)
        path = path if path.is_absolute() else root / path
        if path.is_file():
            return path
        found = shutil.which(candidate)
        if found:
            return Path(found)
    return None


def campaign_functions() -> dict[str, list[tuple[int, int]]]:
    led = json.loads((M.campaign_root() / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    return {m["name"]: sorted((f["start"], f["end"]) for f in m.get("functions", [])) for m in led["modules"]
            if m.get("functions")}


def lbpcov_figures(root: Path) -> dict[str, dict]:
    out = root / OUT_DIR_REL / "lbpcov.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([sys.executable, str(root / "tools" / "lbpcov.py"), "--json", str(out)], check=True,
                   capture_output=True, text=True)
    return {r["module"]: r for r in json.loads(out.read_text(encoding="utf-8")) if r["module"] != "ALL"}


def source_headers(include_wip: bool, only: list[str] | None, root: Path = ROOT) -> tuple[list[tuple[str, dict]],
                                                                                         list[str]]:
    found, bad = [], []
    for tree in ("src", "wip") if include_wip else ("src",):
        for p in sorted((root / tree).rglob("*")):
            if p.suffix not in (".c", ".cpp"):
                continue
            rel = p.relative_to(root).as_posix()
            if only and not any(fnmatch.fnmatch(rel, g) for g in only):
                continue
            try:
                found.append((rel, M.parse_header(p)))
            except ValueError as exc:
                bad.append(f"{rel}: {exc}")
    return found, bad


def generate(args: argparse.Namespace) -> int:
    ledger = campaign_functions()
    headers, bad = source_headers(not args.no_wip, args.only)
    units, rest, superseded, skipped = plan(headers, ledger)
    cache: dict = {}

    def retail(module: str, start: int, size: int) -> bytes:
        data, seg = M.module_bytes(cache, module)
        off = seg["file_offset"] + start - seg["start"]
        return data[off:off + size]

    def artifacts(rel: str, _h: dict):
        out = ROOT / "build" / _h["module"] / M.build_key(ROOT / rel)
        return ((out / "linked.elf").read_bytes(), out / "obj.o") if (out / "linked.elf").is_file() else None

    project = Path(args.out) if Path(args.out).is_absolute() else ROOT / args.out
    summary = write_project(ROOT, units, rest, retail, artifacts, ROOT / OUT_DIR_REL, project, subset=bool(args.only))
    summary["skipped"] = bad + skipped + summary["skipped"]
    summary["superseded"] = superseded
    print(f"{project.name}: {summary['units']} units; source units {summary['source_units']}; unmatched functions "
          f"{summary['unmatched_functions']}; {len(superseded)} superseded wip draft(s)")
    if not args.quiet:
        for w in summary["warnings"][:20]:
            print(f"  warning: {w}")
    for s in summary["skipped"][:20]:
        print(f"  skipped: {s}", file=sys.stderr)
    return 0


def check(args: argparse.Namespace) -> int:
    project = Path(args.out) if Path(args.out).is_absolute() else ROOT / args.out
    if not project.is_file():
        print(f"no {project}: run `python tools/lbpobjdiff.py` first")
        return 2
    doc = json.loads(project.read_text(encoding="utf-8"))
    problems, totals = project_problems(ROOT, doc, campaign_functions(), lbpcov_figures(ROOT))
    for module, t in sorted(totals.items()):
        print(f"{module}: complete {t['complete_code']:,} of {t['total_code']:,} code bytes")
    cli = objdiff_cli(args.objdiff_cli)
    if cli is None:
        print("objdiff-cli not found (docs/GUIDE.md): objdiff's own report was not compared")
    else:
        run = subprocess.run([str(cli), "report", "generate", "-p", str(project.parent), "-o", "-", "--format",
                              "json"], capture_output=True, text=True, encoding="utf-8", errors="replace")
        if run.returncode:
            problems.append(f"objdiff-cli report generate failed: {run.stderr[-500:]}")
        else:
            report = json.loads(run.stdout[run.stdout.index("{"):])
            src = sorted(u["name"] for u in doc["units"] if u["metadata"].get("complete"))
            sample = src if args.random == 0 else random.Random(0).sample(src, min(args.random, len(src)))
            problems += report_problems(report, totals, sample)
            print(f"objdiff-cli: report compared, {len(sample)} src/ unit(s) checked at 100%")
    print("objdiff project: " + ("agrees with the ledger and lbpcov.py" if not problems else "DIFFERS"))
    for p in problems[:30]:
        print("  " + p)
    return 1 if problems else 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=PROJECT_REL, metavar="FILE", help="the project file (default objdiff.json)")
    sub = ap.add_subparsers(dest="command")
    gen = sub.add_parser("generate", help="write objdiff.json and build/objdiff/")
    gen.add_argument("--only", action="append", metavar="GLOB", help="only sources matching GLOB (debugging)")
    gen.add_argument("--no-wip", action="store_true", help="leave the wip/ near misses out")
    gen.add_argument("--quiet", action="store_true")
    gen.add_argument("--out", default=argparse.SUPPRESS, metavar="FILE")
    chk = sub.add_parser("check", help="compare the project with the ledger, lbpcov.py and objdiff's report")
    chk.add_argument("--objdiff-cli", help="path of objdiff-cli")
    chk.add_argument("--random", type=int, default=30, help="src/ units to check in objdiff's report (0 = all)")
    chk.add_argument("--out", default=argparse.SUPPRESS, metavar="FILE")
    args = ap.parse_args()
    if args.command == "check":
        return check(args)
    args.only = getattr(args, "only", None)
    args.no_wip = getattr(args, "no_wip", False)
    args.quiet = getattr(args, "quiet", False)
    if args.only and Path(args.out).name == PROJECT_REL:
        ap.error("a --only subset has no unmatched units: write it with --out to another file, not objdiff.json")
    return generate(args)


if __name__ == "__main__":
    raise SystemExit(main())
