#!/usr/bin/env python3
"""Quality report over the exact sources in src/ (they all match; this is about how they read).

    python tools/lbplint.py [--json try/lint-report.json] [--show KIND]

Checks:
  header      malformed LBPMATCH header, header not on the first line, or missing/unknown second-line
              provenance note
  lang        file extension disagrees with the header's lang= (.c is lang=c99, .cpp is lang=c++)
  dupcond     `if (X)` immediately nested inside `if (X)` (a matching idiom)
  volatile    `volatile` qualifier
  selfassign  `x = x;`
  goto        `goto` statements
  idiom-noted note line declares a "matching idiom"
  gate        the fake-match gate of tools/lbpgate.py (asm that emits instructions, .word/.byte/.incbin, code-emitting
              pragmas, extra= headers); gate-idiom lists the sources with an allowed register asm binding
  protos      functions/globals declared with different types in different sources (whole-tree
              consistency: a unified decomp needs one declaration per symbol)
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpgate
import lbpmatch as M

# The note kinds the campaign importer accepts (campaign reports/import_hst_sources_v14.py KIND_PATTERNS; keep
# them identical): a note the importer cannot classify keeps the source out of the ledger.
NOTE_KINDS = (("clone", re.compile(r"Clone of src/\S+ \(lbpclone\.py tag [\w./-]+\)")),
              ("xclone", re.compile(r"Clone of hst-decomp src/\S+ \(lbpxclone\.py tag [\w./-]+\)")),
              ("corrected", re.compile(r"Ghidra pseudocode \(lbpgen\.py tag [^)]+\) corrected by hand")),
              ("generated", re.compile(r"Mechanical translation of Ghidra pseudocode \(lbpgen\.py tag [\w./-]+\)")),
              ("m2c", re.compile(r"Mechanical translation of the disassembly \(m2c [0-9a-f]{7,40}, lbpm2c\.py tag [\w./-]+\)")),
              ("library", re.compile(r"Library source: newlib |^\s*zlib \d+\.\d+\.\d+ \(zlib\.net, ")),
              ("template", re.compile(r"[^.;]*\(template instance, lbptmpl\.py tag [\w./-]+\)")),
              ("hand", re.compile(r"Hand-written reconstruction\b")))
# One-line comments that may sit between the header and the note (pointers such as a docs/ reference), as in
# hst-decomp (hstlint.provenance_note).
ANNOTATIONS_BEFORE_NOTE = 2
LEADING_COMMENT = re.compile(r"[ \t]*\n?[ \t]*/\*(.*?)\*/", re.S)


def provenance_note(text: str) -> tuple[str | None, str]:
    """(kind, comment) of a source's provenance note (CONTRIBUTING.md, "Line 2: the note"): the comment right after the
    header line (it may span lines, as the zlib sources' notes do), or, after at most ANNOTATIONS_BEFORE_NOTE one-line
    annotation comments, the first one of a NOTE_KINDS kind. (None, the comment after the header or "") when there is
    none. The one rule lbplint.py and lbpprcheck.py share."""
    first, sep, rest = text.partition("\n")
    if not sep:
        return None, ""
    pos, seen = 0, []
    for _ in range(ANNOTATIONS_BEFORE_NOTE + 1):
        m = LEADING_COMMENT.match(rest, pos)
        if not m:
            break
        seen.append(m.group(0).strip())
        kind = next((k for k, rx in NOTE_KINDS if rx.search(m.group(1))), None)
        if kind:
            return kind, m.group(0).strip()
        if "\n" in m.group(1):                  # an annotation is one line; a longer comment is not one
            break
        pos = m.end()
    return None, seen[0] if seen else ""


DUPCOND = re.compile(r"if\s*\(([^()\n]*(?:\([^()\n]*\)[^()\n]*)*)\)\s*\{\s*\n?\s*if\s*\(\1\)")
SELF = re.compile(r"(?<![\w.>])(\w+)\s*=\s*\1\s*;")
DECL = re.compile(r"^(?:extern\s+)?(?:\"C\"\s+)?([A-Za-z_][\w\s\*]*?)\s*\b((?:FUN|DAT|PTR)_[0-9a-fA-F]{8})\b\s*(\([^;{]*\)|\[[^\]]*\])?\s*;",
                  re.M)


def norm(s: str) -> str:
    s = re.sub(r"/\*.*?\*/", "", s)
    s = re.sub(r"\b(extern|static|const)\b", "", s)
    s = re.sub(r"\s+", " ", s).strip()
    s = re.sub(r"\s*([*(),\[\]])\s*", r"\1", s)
    # drop parameter names: keep types only (last identifier in each comma part when >1 token)
    m = re.match(r"(.*)\((.*)\)$", s)
    if m:
        parts = []
        for p in m.group(2).split(","):
            toks = re.findall(r"[A-Za-z_]\w*|\*+|\.\.\.|\[\]", p)
            if len(toks) > 1 and re.match(r"[A-Za-z_]", toks[-1]) and toks[-1] not in (
                    "int", "char", "short", "long", "float", "double", "void", "unsigned", "signed"):
                toks = toks[:-1]
            parts.append(" ".join(toks))
        s = f"{m.group(1)}({','.join(parts)})"
    return s


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--json", type=Path, default=M.ROOT / "try" / "lint-report.json")
    ap.add_argument("--show", help="print the files flagged with this check")
    args = ap.parse_args()
    flags = defaultdict(list)
    kinds = defaultdict(int)
    gate_kinds = defaultdict(int)
    decls = defaultdict(lambda: defaultdict(list))
    for p in sorted((M.ROOT / "src").rglob("*")):
        if p.suffix not in (".c", ".cpp"):
            continue
        rel = p.relative_to(M.ROOT).as_posix()
        try:
            text = p.read_text(encoding="utf-8")
        except FileNotFoundError:  # renamed by a concurrent agent while scanning
            continue
        lines = text.splitlines()
        try:
            h = M.parse_header(p)
            if not lines[0].startswith("/* LBPMATCH ") and rel not in flags["header"]:
                flags["header"].append(rel)
            if (h["lang"] == "c++") != (p.suffix == ".cpp"):
                flags["lang"].append(rel)
        except Exception:  # noqa: BLE001 - any parse failure is a finding
            flags["header"].append(rel)
        gate = lbpgate.check_file(p)
        for f in gate:
            gate_kinds[f.kind] += 1
        if lbpgate.refusals(gate):
            flags["gate"].append(rel)
        if any(f.severity == lbpgate.IDIOM for f in gate):
            flags["gate-idiom"].append(rel)
        kind, note = provenance_note(text)
        if kind is None and rel not in flags["header"]:
            flags["header"].append(rel)
        kinds[kind or "unknown"] += 1
        if "matching idiom" in note:
            flags["idiom-noted"].append(rel)
        body = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
        if DUPCOND.search(body):
            flags["dupcond"].append(rel)
        if re.search(r"\bvolatile\b", body):
            flags["volatile"].append(rel)
        if SELF.search(body):
            flags["selfassign"].append(rel)
        if re.search(r"\bgoto\b", body):
            flags["goto"].append(rel)
        if kind == "clone":
            continue  # clones repeat their exemplar's declarations
        module = M.parse_header(p)["module"]
        for m in DECL.finditer(body):
            ret, name, tail = m.group(1), m.group(2), m.group(3) or ""
            if ret.strip() in ("return", "else", "goto"):
                continue
            decls[(module, name)][norm(f"{ret}{tail}")].append(rel)
    conflicts = {f"{mod}:{name}": {sig: files for sig, files in sigs.items()}
                 for (mod, name), sigs in decls.items() if len(sigs) > 1}
    report = {"sources": sum(kinds.values()), "note_kinds": dict(kinds), "gate_findings": dict(gate_kinds),
              "flags": {k: len(v) for k, v in flags.items()},
              "declared_symbols": len(decls), "conflicting_symbols": len(conflicts),
              "files": {k: v for k, v in flags.items()}, "conflicts": conflicts}
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(report, indent=1), encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("sources", "note_kinds", "flags", "declared_symbols",
                                               "conflicting_symbols")}, indent=1))
    if args.show:
        for f in flags.get(args.show, []):
            print(f)
    if flags.get("gate"):
        print(f"GATE: {len(flags['gate'])} source(s) fail the fake-match gate (python tools/lbpgate.py --all)")
    return 1 if flags.get("gate") else 0


if __name__ == "__main__":
    raise SystemExit(main())
