#!/usr/bin/env python3
"""Measure and classify the near misses in wip/ (the sources that are close to exact but are not).

    python tools/lbpnearmiss.py [--parallel 2] [--json OUT] [--no-compile] [wip/file ...]

Runs tools/lbpmatch.py --quiet --json on every top-level wip/ source (or on the files named; their rows are merged
into the last report at OUT), building into try/nearmiss/build (LBPMATCH_BUILD_ROOT, so build/ is not touched), then
labels each file with exactly ONE class from the words it still gets wrong and writes OUT (default
try/nearmiss-report.json) plus a short text table (OUT with .txt, also printed). --no-compile reuses the last
lbpmatch rows (try/nearmiss/match.json and the build directories) and only re-labels. A file whose header lbpmatch
cannot read, or whose cc= no profile knows, is not compiled (lbpmatch would stop the whole chunk) and is reported as
tool_state.

Port of hst-decomp's tools/hstnearmiss.py onto lbpmatch's outputs (the word classes and levers are the same rules;
the HST MWCC-DIGEST section references are dropped). Differences: the words-off histogram below; no markdown report
and no example diffs; no `blocked` list (LBP has no data/blocked_functions.json, so the class is never given).

"words left" is lbpmatch's own count: the number of word positions that differ between our linked bytes and the
retail bytes (a function of the wrong length counts the surplus words too). "words aligned" is the count outside the
longest common word alignment (one inserted instruction costs one; tools/lbpperm.py reports this one). Rank = code
bytes per word left.

Histogram (JSON "histogram", and the text table): per compiler profile (the file's cc=, the primary profile
gcc404xs-Os without one), the live files (not superseded) counted in the words-left bands 0, 1-2, 3-5, 6-10, 11-20,
>20 and "unmeasured" (did not reach the comparison), overall, per language (C or C++) and per class.

Classes (first rule that applies; the rules look at registers, opcodes and immediates only, so a label is a hint
about where to look, not a diagnosis):

  exact_now              lbpmatch reports EXACT: install it (header, provenance note, lbppromote)
  superseded             src/ already has a source for this function: the wip file is stale
  blocked                (not used in LBP; kept so the class list matches HST's)
  tool_state             COMPILE_FAILED / MISSING_SYMBOLS / LINK_FAILED / NO_SYMBOL / MISPLACED / a bad header: the
                         file does not reach the comparison
  jump_table_rodata      UNPLACED_SECTION / SECTION_MISMATCH: the code matches or the table/data is not placed
  frame_layout           only the frame size (first addiu sp) and sp-relative slots differ
  type_signedness        only lb/lbu, lh/lhu, slt/sltu, sra/srl, mult/multu, div/divu style pairs differ
  register_colouring     same words with different register fields (same opcodes, same immediates)
  constant_materialisation  the differing words are lui/ori/addiu-from-zero/hi-lo immediates, or the surplus
                         words are exactly such constants
  instruction_schedule   the same words (registers aside) in a different order
  branch_shape           only branch words differ (likely vs normal, polarity, offsets)
  missing_extra_insn     our function has more or fewer words than retail and the surplus is not constants
  unknown                a mixture the rules above do not separate (the JSON lists "facets")

Each file also gets a lever ("lever", with the deciding evidence in "lever_why"), decided from the function's calls,
then its control flow, then whether only relocated fields differ, and only then from its class:

  install / none / tooling   exact now; superseded; the file does not reach the comparison
  body                   the calls differ (count, or jal targets not explained by the referent rule)
  structure              the conditional-branch kinds, jumps or returns differ
  referent               same length, and every differing word is a jal or a relocated word that differs only in its
                         16-bit field (relocations read from our object): a symbol is wrong, not the code
  permute / types / frame / expression
                         calls and control flow agree: chosen by the class (register_colouring and
                         instruction_schedule -> permute, type_signedness -> types, frame_layout -> frame, the rest
                         -> expression)

tools/lbpnearperm.py seeds the permuter from this report by class and lever.
"""
from __future__ import annotations

import argparse
import datetime
import difflib
import json
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402

ROOT = M.ROOT
WORK = ROOT / "try" / "nearmiss"
MATCH_JSON = WORK / "match.json"
REPORT_JSON = ROOT / "try" / "nearmiss-report.json"
SCHEMA = "lbp-decomp.nearmiss.v1"
CLASSES = ("exact_now", "superseded", "blocked", "tool_state", "jump_table_rodata", "frame_layout", "type_signedness",
           "register_colouring", "constant_materialisation", "instruction_schedule", "branch_shape",
           "missing_extra_insn", "unknown")
DEAD = ("superseded", "blocked")
SP = 29
BANDS = (("1-2", 1, 2), ("3-5", 3, 5), ("6-10", 6, 10), ("11-20", 11, 20), (">20", 21, None))
BAND_NAMES = ("0",) + tuple(b[0] for b in BANDS) + ("unmeasured",)
CHUNK = 40


# --------------------------------------------------------------------------------------------------------------
# word-level classification (pure functions)

def words(data: bytes) -> list[int]:
    return [int.from_bytes(data[i:i + 4], "little") for i in range(0, len(data) - len(data) % 4, 4)]


def regmask(w: int) -> int:
    """The word with its general and floating-point register fields cleared (opcode and immediates kept)."""
    op = w >> 26
    if op == 0:                                   # SPECIAL: rs, rt, rd
        return w & ~((31 << 21) | (31 << 16) | (31 << 11))
    if op == 1:                                   # REGIMM: rs; rt is the branch kind
        return w & ~(31 << 21)
    if op in (2, 3):                              # j, jal: a target, no registers
        return w
    if op == 0x11:                                # COP1
        fmt = (w >> 21) & 31
        if fmt in (0x10, 0x11, 0x14):             # s/d/w arithmetic: ft, fs, fd
            return w & ~((31 << 16) | (31 << 11) | (31 << 6))
        if fmt in (0, 2, 4, 6):                   # mfc1/cfc1/mtc1/ctc1: rt and fs
            return w & ~((31 << 16) | (31 << 11))
        return w                                  # bc1x: condition bits and an offset
    if op in (0x18, 0x19, 0x1a, 0x1b, 0x34, 0x35, 0x36, 0x37, 0x3c, 0x3d, 0x3e, 0x3f, 0x32, 0x3a):
        return w                                  # VFPU: not decoded, a difference is a plain difference
    return w & ~((31 << 21) | (31 << 16))         # I-type, loads/stores, SPECIAL3 (ext/ins/seb/seh), lwc1/swc1


def loosekey(w: int) -> int:
    """regmask(), and for loads/stores also without the displacement (a folded and an unfolded access differ in it)."""
    op = w >> 26
    if 0x20 <= op <= 0x2F or op in (0x31, 0x39):
        return op << 26
    return regmask(w)


def is_branch(w: int) -> bool:
    op = w >> 26
    return op in (4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17, 1) or (op == 0x11 and (w >> 21) & 31 == 8)


def is_constant_word(w: int) -> bool:
    """lui, or ori/addiu/addiu-from-zero that completes or loads a constant."""
    op = w >> 26
    rs, rt = (w >> 21) & 31, (w >> 16) & 31
    if op == 0x0F:
        return True
    if op in (0x09, 0x0D, 0x0C, 0x08):            # addiu, ori, andi, addi
        return rs == 0 or rs == rt
    return False


def is_sp_word(w: int) -> bool:
    """addiu/addi sp or a sp-relative load/store/addiu-address, the instructions a frame change touches."""
    op = w >> 26
    rs = (w >> 21) & 31
    if op in (0x08, 0x09):
        return rs == SP or ((w >> 16) & 31) == SP
    return (0x20 <= op <= 0x2F or op in (0x31, 0x35, 0x39, 0x3d, 0x32, 0x36, 0x3a, 0x3e)) and rs == SP


SIGN_OPS = {(0x20, 0x24), (0x24, 0x20), (0x21, 0x25), (0x25, 0x21), (0x0A, 0x0B), (0x0B, 0x0A)}
SIGN_FUNCT = {(0x2A, 0x2B), (0x2B, 0x2A), (2, 3), (3, 2), (6, 7), (7, 6), (0x18, 0x19), (0x19, 0x18), (0x1A, 0x1B),
              (0x1B, 0x1A)}


def sign_pair(a: int, b: int) -> bool:
    oa, ob = a >> 26, b >> 26
    if oa == 0 and ob == 0:
        return (a & 63, b & 63) in SIGN_FUNCT and ((a >> 6) == (b >> 6) or regmask(a & ~63) == regmask(b & ~63))
    return (oa, ob) in SIGN_OPS and (a & 0x03FFFFFF) == (b & 0x03FFFFFF)


def classify_words(ours: list[int], theirs: list[int]) -> tuple[str, list[str]]:
    """(class, facets) for a function whose bytes differ."""
    facets: list[str] = []
    if len(ours) == len(theirs):
        pairs = [(a, b) for a, b in zip(ours, theirs) if a != b]
        if not pairs:
            return "exact_now", []
        if all(regmask(a) == regmask(b) for a, b in pairs):
            return "register_colouring", ["reg"]
        if all(is_sp_word(a) and is_sp_word(b) for a, b in pairs):
            return "frame_layout", ["sp"]
        if all(sign_pair(a, b) or regmask(a) == regmask(b) for a, b in pairs):
            return "type_signedness", ["sign"]
        if all(is_constant_word(a) and is_constant_word(b) for a, b in pairs):
            return "constant_materialisation", ["const"]
        if sorted(regmask(w) for w in ours) == sorted(regmask(w) for w in theirs):
            return "instruction_schedule", ["order"] + (["reg"] if any(regmask(a) == regmask(b) for a, b in pairs) else [])
        if all(is_branch(a) and is_branch(b) for a, b in pairs):
            return "branch_shape", ["branch"]
        if any(is_branch(a) or is_branch(b) for a, b in pairs):
            facets.append("branch")
        if any(regmask(a) == regmask(b) for a, b in pairs):
            facets.append("reg")
        if any(is_constant_word(a) or is_constant_word(b) for a, b in pairs):
            facets.append("const")
        if any(is_sp_word(a) or is_sp_word(b) for a, b in pairs):
            facets.append("sp")
        return "unknown", facets or ["mixed"]
    # different lengths: align the register-masked words and look at what only one side has
    sm = difflib.SequenceMatcher(None, [loosekey(w) for w in ours], [loosekey(w) for w in theirs], autojunk=False)
    only_ours: list[int] = []
    only_theirs: list[int] = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag in ("delete", "replace"):
            only_ours += ours[i1:i2]
        if tag in ("insert", "replace"):
            only_theirs += theirs[j1:j2]
    surplus = only_ours + only_theirs
    if ours and theirs and ours[0] != theirs[0] and (ours[0] >> 16) == 0x27BD and (theirs[0] >> 16) == 0x27BD \
            and all(is_sp_word(w) for w in surplus):
        return "frame_layout", ["sp", "length"]
    if surplus and all(is_constant_word(w) for w in surplus):
        return "constant_materialisation", ["const", "length"]
    return "missing_extra_insn", ["length"] + (["const"] if any(is_constant_word(w) for w in surplus) else [])


# --------------------------------------------------------------------------------------------------------------
# levers: the kind of work a near miss needs (pure functions)
#
# The word class says which words are wrong; the lever says what to change to fix them, decided from the function's
# structure first: a missing call or branch also shifts registers and the schedule, so a word class alone would send
# such a file to the permuter.

LEVERS = {
    "install":    "exact now: give it a header and provenance note and promote it (tools/lbppromote.py)",
    "none":       "superseded: nothing to do",
    "tooling":    "the file does not reach the comparison: fix the header, symbols, helpers= or the section placement "
                  "before touching the code",
    "body":       "the calls differ: a call is missing or extra, or retail inlined a helper that this file calls (or "
                  "the reverse): reconstruct the body; look for a same-file helper or a macro",
    "structure":  "branch, return or branch-kind counts differ: rebuild the control flow (if/else order, loop form, "
                  "early returns, switch) from the retail branches",
    "referent":   "the instructions agree and only relocated fields differ: the code is right and a symbol is not "
                  "(wrong global, extern, symbol address or addend); fix declarations or symbols/, not the code",
    "permute":    "registers or order only: try declaration order, temporaries and CSE first, then lbpnearperm.py",
    "types":      "signedness or width only: fix the declared types",
    "frame":      "only the stack frame differs: the locals (count, sizes, order, lifetimes)",
    "expression": "same calls and control flow, different instructions inside blocks: expression and statement shape "
                  "(loads, CSE, constants, casts, struct copies)",
}
CLASS_LEVER = {"exact_now": "install", "superseded": "none", "blocked": "none", "tool_state": "tooling",
               "jump_table_rodata": "tooling", "register_colouring": "permute", "instruction_schedule": "permute",
               "type_signedness": "types", "frame_layout": "frame", "branch_shape": "structure",
               "constant_materialisation": "expression", "missing_extra_insn": "expression", "unknown": "expression"}
JR_RA = 0x03E00008


def is_call(w: int) -> bool:
    """jal, or jalr (SPECIAL funct 9)."""
    return w >> 26 == 3 or (w >> 26 == 0 and w & 63 == 9)


def call_set(ws: list[int]) -> Counter:
    """The calls a function makes: jal by target field, jalr counted as one indirect call."""
    return Counter(("jal", w & 0x03FFFFFF) if w >> 26 == 3 else ("jalr",) for w in ws if is_call(w))


def flow_shape(ws: list[int]) -> tuple[Counter, int, int]:
    """(conditional branch kinds, jumps, returns): a branch kind is the opcode with its rt/format field (beq vs bne,
    bltz vs bgez, bc1t vs bc1f), so an inverted condition counts; registers and offsets do not."""
    kinds: Counter = Counter()
    for w in ws:
        if is_branch(w):
            op = w >> 26
            kinds[(op, (w >> 16) & 31) if op in (1, 0x11) else (op,)] += 1
    return kinds, sum(1 for w in ws if w >> 26 == 2), sum(1 for w in ws if w == JR_RA)


def unlikely(kind: tuple) -> tuple:
    """A branch kind with its likely bit cleared: beql..bgtzl -> beq..bgtz, bltzl/bgezl(al) -> bltz/bgez(al),
    bc1xl -> bc1x."""
    if kind[0] in (0x14, 0x15, 0x16, 0x17):
        return (kind[0] - 0x10,)
    if kind[0] == 1:
        return (1, kind[1] & ~2)
    if kind[0] == 0x11:
        return (0x11, kind[1] & ~2)
    return kind


def likely_blind(kinds: Counter) -> Counter:
    out: Counter = Counter()
    for kind, n in kinds.items():
        out[unlikely(kind)] += n
    return out


def route(cls: str, ours: list[int] | None, theirs: list[int] | None,
          relocated: set[int] | None = None) -> tuple[str, str]:
    """(lever, the evidence that decided it). `relocated` holds the indexes of our words that carry a relocation
    (None when unknown: then the referent test only accepts call targets)."""
    if cls in ("exact_now", "superseded", "blocked", "tool_state", "jump_table_rodata") or ours is None or theirs is None:
        return CLASS_LEVER.get(cls, "expression"), f"class {cls}"
    oc, tc = call_set(ours), call_set(theirs)
    same_length = len(ours) == len(theirs)
    pairs = [(i, a, b) for i, (a, b) in enumerate(zip(ours, theirs)) if a != b] if same_length else []

    def field_only(i: int, a: int, b: int) -> bool:
        if a >> 26 == 3 and b >> 26 == 3:                  # jal: only the target field can differ
            return True
        return relocated is not None and i in relocated and (a >> 16) == (b >> 16) and a >> 26 not in (0, 2, 3)

    if oc != tc:
        if same_length and pairs and all(field_only(i, a, b) for i, a, b in pairs):
            return "referent", f"{len(pairs)} relocated word(s) differ in their fields only (calls to another symbol)"
        n_o, n_t = sum(oc.values()), sum(tc.values())
        return "body", (f"calls {n_o} vs retail {n_t}" if n_o != n_t else f"{n_o} calls, different call targets")
    fo, ft = flow_shape(ours), flow_shape(theirs)
    if fo != ft:
        parts = []
        if sum(fo[0].values()) != sum(ft[0].values()):
            parts.append(f"branches {sum(fo[0].values())} vs retail {sum(ft[0].values())}")
        elif fo[0] != ft[0]:
            if likely_blind(fo[0]) == likely_blind(ft[0]):
                parts.append("likely vs normal branch only (loop form or delay-slot fill)")
            else:
                parts.append("branch kinds differ (an inverted condition or a different comparison)")
        if fo[1] != ft[1]:
            parts.append(f"jumps {fo[1]} vs retail {ft[1]}")
        if fo[2] != ft[2]:
            parts.append(f"returns {fo[2]} vs retail {ft[2]}")
        return "structure", ", ".join(parts)
    if same_length and pairs and all(field_only(i, a, b) for i, a, b in pairs):
        return "referent", f"{len(pairs)} relocated word(s) differ in their fields only"
    return CLASS_LEVER.get(cls, "expression"), f"same calls and control flow; class {cls}"


def relocated_words(obj: bytes, h: dict) -> set[int] | None:
    """Indexes of the compared function's words that carry a relocation in our object (the words the linker
    writes), counted from the header symbol as our_body() reads it; None when that cannot be determined."""
    sections = M.elf_sections(obj)
    names = [n for n, _ in sections]
    if ".text" not in names or h.get("helpers"):
        return None
    ti = names.index(".text")
    sym = next(((v, s) for n, v, s, i, sh in M.elf_symbols(obj) if n == h["symbol"] and sh == ti), None)
    if sym is None:
        return None
    lo, hi = sym[0], sym[0] + max(sym[1], h["end"] - h["start"])
    out: set[int] = set()
    for _n, s in sections:
        if s[1] == 4 and s[7] == ti:                       # SHT_RELA: not handled
            return None
        if s[1] != 9 or s[7] != ti:                        # SHT_REL applying to this .text
            continue
        for k in range(s[5] // 8):
            off = int.from_bytes(obj[s[4] + 8 * k:s[4] + 8 * k + 4], "little")
            if lo <= off < hi:
                out.add((off - lo) // 4)
    return out


def words_left_of(ours: list[int], theirs: list[int]) -> int:
    n = max(len(ours), len(theirs))
    return sum(1 for i in range(n) if (ours[i] if i < len(ours) else None) != (theirs[i] if i < len(theirs) else None))


def words_aligned_of(ours: list[int], theirs: list[int]) -> int:
    """Words outside the longest common word alignment (raw words): one inserted instruction costs one."""
    if ours == theirs:
        return 0
    sm = difflib.SequenceMatcher(None, ours, theirs, autojunk=False)
    return max(1, max(len(ours), len(theirs)) - sum(b.size for b in sm.get_matching_blocks()))


# --------------------------------------------------------------------------------------------------------------
# histogram (pure functions)

def band(left: int | None) -> str:
    if left is None:
        return "unmeasured"
    if left == 0:
        return "0"
    for name, lo, hi in BANDS:
        if left >= lo and (hi is None or left <= hi):
            return name
    return "unmeasured"


def language(h: dict, path: Path | str = "") -> str:
    return "C++" if h.get("lang") == "c++" or str(path).endswith(".cpp") else "C"


def _bands() -> dict:
    return {b: 0 for b in BAND_NAMES}


def histogram(rows: list[dict]) -> dict:
    """{profile: {"files", "bands", "lang": {C|C++: bands}, "class": {class: bands}}} over the live rows (not
    superseded); profiles and their keys sorted."""
    out: dict = {}
    for r in rows:
        if r["class"] in DEAD:
            continue
        b = band(r["words_left"])
        p = out.setdefault(r["cc"], {"files": 0, "bands": _bands(), "lang": {}, "class": {}})
        p["files"] += 1
        p["bands"][b] += 1
        p["lang"].setdefault(r["lang"], _bands())[b] += 1
        p["class"].setdefault(r["class"], _bands())[b] += 1
    for p in out.values():
        p["lang"] = dict(sorted(p["lang"].items()))
        p["class"] = {c: p["class"][c] for c in CLASSES if c in p["class"]}
    return dict(sorted(out.items()))


def text_table(hist: dict, rows: list[dict]) -> str:
    """The histogram as a short fixed-width table: one line per profile, then per profile and language."""
    cols = BAND_NAMES
    head = f"{'profile':14} {'lang':4} {'files':>5} " + " ".join(f"{c:>6}" for c in cols)
    out = [head, "-" * len(head)]
    for prof, p in hist.items():
        out.append(f"{prof:14} {'all':4} {p['files']:5} " + " ".join(f"{p['bands'][c]:6}" for c in cols))
        for lang, bands in p["lang"].items():
            out.append(f"{'':14} {lang:4} {sum(bands.values()):5} " + " ".join(f"{bands[c]:6}" for c in cols))
    dead = sum(1 for r in rows if r["class"] in DEAD)
    out.append(f"({dead} superseded file(s) left out)")
    out.append("")
    out.append(f"{'class':26} {'files':>5} {'1-5':>5} {'bytes':>8} {'words left':>10}")
    for t in class_table(rows):
        low = sum(1 for r in rows if r["class"] == t["class"] and r["words_left"] and r["words_left"] <= 5)
        out.append(f"{t['class']:26} {t['files']:5} {low:5} {t['code_bytes']:8} {t['words_left']:10}")
    out.append("")
    out.append(f"{'lever':26} {'files':>5} {'bytes':>8} {'words left':>10}")
    for t in lever_table(rows):
        out.append(f"{t['lever']:26} {t['files']:5} {t['code_bytes']:8} {t['words_left']:10}")
    return "\n".join(out) + "\n"


def class_table(rows: list[dict]) -> list[dict]:
    out = []
    for cls in CLASSES:
        mine = [r for r in rows if r["class"] == cls]
        if not mine:
            continue
        measured = [r for r in mine if r["words_left"] is not None]
        out.append({"class": cls, "files": len(mine), "code_bytes": sum(r["size"] for r in mine),
                    "words_left": sum(r["words_left"] for r in measured),
                    "median_words_left": sorted(r["words_left"] for r in measured)[len(measured) // 2] if measured else None,
                    "cheapest_yield": max((r["yield"] for r in mine), default=0.0)})
    return out


def lever_table(rows: list[dict]) -> list[dict]:
    out = []
    for lever, action in LEVERS.items():
        mine = [r for r in rows if r.get("lever") == lever]
        if not mine:
            continue
        measured = [r for r in mine if r["words_left"] is not None]
        out.append({"lever": lever, "files": len(mine), "code_bytes": sum(r["size"] for r in mine),
                    "words_left": sum(r["words_left"] for r in measured), "action": action})
    return out


# --------------------------------------------------------------------------------------------------------------
# measuring

def our_body(out: Path, h: dict) -> bytes | None:
    """Our linked function bytes exactly as lbpmatch compares them, or None when there is no link output (a helpers=
    source has no text.bin)."""
    text, obj = out / "text.bin", out / "obj.o"
    if not text.exists() or not obj.exists():
        return None
    objbytes = obj.read_bytes()
    names = [n for n, _ in M.elf_sections(objbytes)]
    ti = names.index(".text") if ".text" in names else -1
    sym = [(v, s) for n, v, s, i, sh in M.elf_symbols(objbytes) if n == h["symbol"] and sh == ti]
    text_off, size = (sym[0] if sym else (0, 0))
    linked = text.read_bytes()[text_off:]
    span = h["end"] - h["start"]
    return linked[:span] if size < span else linked[:size]


def retail_body(cache: dict, h: dict) -> bytes:
    data, seg = M.module_bytes(cache, h["module"])
    off = seg["file_offset"] + h["start"] - seg["start"]
    return data[off:off + h["end"] - h["start"]]


def rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def readable(path: Path) -> tuple[dict | None, str]:
    """(header, "") when lbpmatch can compile the file, else (header or None, why not)."""
    import lbpsweep as S
    try:
        h = M.parse_header(path)
    except (ValueError, OSError) as e:
        return None, f"bad header: {e}"
    cc = h.get("cc", M.PRIMARY_PROFILE)
    if cc not in S.KNOWN_PROFILES:
        return h, f"unknown cc={cc}"
    return h, ""


def unread_row(path: Path, h: dict | None, why: str) -> dict:
    h = h or {}
    start, end = h.get("start", 0), h.get("end", 0)
    row = {"wip": rel(path), "module": h.get("module", "?"), "start": f"0x{start:08x}", "end": f"0x{end:08x}",
           "size": end - start, "cc": h.get("cc", M.PRIMARY_PROFILE), "lang": language(h, path),
           "status": "NOT_COMPILED", "detail": why, "words": (end - start) // 4, "words_left": None,
           "words_aligned": None, "class": "tool_state", "facets": [], "origin": "?", "superseded_by": None,
           "lever": "tooling", "lever_why": why, "yield": 0.0}
    row["band"] = band(None)
    return row


def measure(path: Path, h: dict, status: str, detail: str, build_root: Path, origin: dict[str, str],
            src_index: dict, data_cache: dict) -> dict:
    size = h["end"] - h["start"]
    r = rel(path)
    row = {"wip": r, "module": h["module"], "start": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}", "size": size,
           "cc": h.get("cc", M.PRIMARY_PROFILE), "lang": language(h, path), "status": status, "detail": detail,
           "words": size // 4, "words_left": None, "words_aligned": None, "class": "unknown", "facets": [],
           "origin": origin.get(r, "?"), "superseded_by": None}
    ours = theirs = relocated = None
    if status == "MISMATCH":
        out = build_root / h["module"] / M.build_key(path)
        body = our_body(out, h)
        if body is not None:
            ours, theirs = words(body), words(retail_body(data_cache, h))
            row["words_left"] = words_left_of(ours, theirs)
            row["words_aligned"] = words_aligned_of(ours, theirs)
            row["class"], row["facets"] = classify_words(ours, theirs)
            row["ours_words"] = len(ours)
            relocated = relocated_words((out / "obj.o").read_bytes(), h)
    elif status == "EXACT":
        row.update({"words_left": 0, "words_aligned": 0, "class": "exact_now"})
    elif status in ("UNPLACED_SECTION", "SECTION_MISMATCH"):
        row["class"] = "jump_table_rodata"
    else:
        row["class"] = "tool_state"
    row["lever"], row["lever_why"] = route(row["class"], ours, theirs, relocated)
    src_path = src_index.get((h["module"], h["start"]))
    if src_path is not None:
        row["superseded_by"] = src_path
        row["class_if_live"], row["class"] = row["class"], "superseded"
        row["lever_if_live"], row["lever"] = row["lever"], "none"
    left = row["words_left"]
    row["band"] = band(left)
    row["yield"] = round(size / max(left, 1), 2) if left is not None else 0.0
    return row


def run_match(files: list[Path], build_root: Path, parallel: int) -> dict:
    """lbpmatch rows by resolved path, in chunks (the Windows command-line limit), building into build_root."""
    import lbpsweep as S
    rows: dict = {}
    WORK.mkdir(parents=True, exist_ok=True)
    for n in range(0, len(files), CHUNK):
        rows.update(S.run_lbpmatch(files[n:n + CHUNK], build_root, parallel, WORK / f"chunk-{n // CHUNK}.json"))
    return rows


def origins() -> dict[str, str]:
    try:
        import lbporigin
        return lbporigin.classify()[0]
    except Exception as e:  # noqa: BLE001 - the origin is a label only
        print(f"origin classes unavailable ({type(e).__name__}: {e}); origin left '?'", file=sys.stderr)
        return {}


def write_report(rows: list[dict], out: Path, generated: str) -> str:
    rows = sorted(rows, key=lambda r: (-r["yield"], r["wip"]))
    hist = histogram(rows)
    doc = {"schema": SCHEMA, "generated": generated, "primary_profile": M.PRIMARY_PROFILE, "bands": list(BAND_NAMES),
           "histogram": hist, "classes": class_table(rows), "levers": lever_table(rows), "files": rows}
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    table = text_table(hist, rows)
    out.with_suffix(".txt").write_text(table, encoding="utf-8")
    return table


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", type=Path, help="wip files to measure (default: all top-level wip/*.c, *.cpp; "
                                                        "rows merge into the report at --json)")
    ap.add_argument("--parallel", type=int, default=2)
    ap.add_argument("--json", type=Path, default=REPORT_JSON, help="report path (the .txt table is written beside it)")
    ap.add_argument("--build-root", type=Path, default=WORK / "build")
    ap.add_argument("--no-compile", action="store_true")
    ap.add_argument("--no-origin", action="store_true", help="skip tools/lbporigin.py's classes")
    args = ap.parse_args()
    import lbplane as L
    chosen = [p if p.is_absolute() else ROOT / p for p in args.files] or \
        sorted(p for p in (ROOT / "wip").iterdir() if p.is_file() and p.suffix in (".c", ".cpp"))
    src_index = {}
    for p in L.sources(ROOT):
        try:
            h = M.parse_header(p)
        except ValueError:
            continue
        src_index[(h["module"], h["start"])] = rel(p)
    origin = {} if args.no_origin else origins()
    rows, todo = [], []
    for p in chosen:
        h, why = readable(p)
        if why:
            rows.append(unread_row(p, h, why))
        else:
            todo.append((p, h))
    if args.no_compile:
        old = json.loads(MATCH_JSON.read_text(encoding="utf-8")) if MATCH_JSON.exists() else []
        results = {(ROOT / r["source"]).resolve(): r for r in old}
    else:
        results = run_match([p for p, _h in todo], args.build_root, args.parallel)
        merged = {}
        if args.files and MATCH_JSON.exists():
            merged = {r["source"]: r for r in json.loads(MATCH_JSON.read_text(encoding="utf-8"))}
        merged.update({r["source"]: r for r in results.values()})
        MATCH_JSON.parent.mkdir(parents=True, exist_ok=True)
        MATCH_JSON.write_text(json.dumps(list(merged.values()), indent=1) + "\n", encoding="utf-8")
    cache: dict = {}
    for p, h in todo:
        r = results.get(p.resolve())
        if r is None:
            rows.append(unread_row(p, h, "no lbpmatch row (the compiler machinery failed?)"))
            continue
        rows.append(measure(p, h, r["status"], r.get("detail") or "", args.build_root, origin, src_index, cache))
    if args.files and args.json.exists():
        old_rows = {r["wip"]: r for r in json.loads(args.json.read_text(encoding="utf-8")).get("files", [])}
        old_rows.update({r["wip"]: r for r in rows})
        rows = [r for r in old_rows.values() if (ROOT / r["wip"]).exists()]
    table = write_report(rows, args.json, datetime.date.today().isoformat())
    print(f"{len(rows)} files measured; report {rel(args.json)}")
    print(table, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
