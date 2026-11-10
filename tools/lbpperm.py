#!/usr/bin/env python3
"""Random-mutation search ("permuter") for near-miss sources.

    python tools/lbpperm.py --tag p1 [--max-diff 8] [--rounds 12] [--mutants 16] [--parallel 2] [--limit N]
                            [--wip] [--no-profiles] [--sideways 0.02] [--restart 0.15] [wip/file ...]

Weighted score idea and penalty constants from simonlindholm/decomp-permuter (MIT); no code copied.

Seeds: each unmatched function's few best MISMATCH candidates from try/gen-*/summary-*.json; with --wip also every
top-level wip/ near miss (header on the first line after lbplane.wip_text, no extra= or helpers=, a known cc=; the
words left of try/nearmiss-report.json, when tools/lbpnearmiss.py wrote one, is its pre-score for --pre-diff); with
files named, only those files (any words left). Every seed is recompiled under a unique name and scored.

Score (the search objective): our linked function words against the retail words, aligned on the register-masked
words (tools/lbpnearmiss.regmask): an aligned pair that differs only in register fields costs REGALLOC (5); a word
present on both sides but not aligned (moved) costs REORDER (60) per moved pair; every other unaligned word (missing,
extra or substituted; a substituted pair counts once) costs INDEL (100). 0 only for an EXACT row. "Words left"
(words outside the longest common raw-word alignment, so an inserted instruction costs one) is kept for --max-diff
and the reports.

Search: each round compiles `--mutants` variants of every open seed, each with 1-3 random source mutations (swap
adjacent statements, reorder declarations, commute operands, flip comparisons, rewrite increments, change local
signedness, invert if/else, regroup float chains, inline temporaries, merge if-chains, ternaries, hoist tails, reorder
address sums, bounds, unsigned compares, and the 2026-11-02 operators of hst-decomp's hstperm.py: introduce
temporaries, add or remove casts, x = x op y versus x op= y, a[i] versus *(a + i), move declarations,
for/while/do-while, `register`, commute struct-field reads; plus the zero-test forms !x <-> x == 0 and x != 0 -> x).
Mutations are confined to the header symbol's definition when the file defines it by name (a library file compiled
whole keeps its other functions as they are). A variant is built from the seed's current text, or from its best
text with probability --restart; it replaces the current text when its score is lower, or equal with probability
--sideways (a plateau move). Mutations need not preserve meaning: only a byte-exact build of the retail function
counts.

Profile phase (the GCC analogue of hstnearperm.py's MWCC pragma phase; --no-profiles skips it): after the rounds, the
best text of every open seed is compiled once under each other LOCKED compiler profile of
data/toolchain-manifest.json (lbpsweep.lock_profiles; only the header's cc= changes, lbpsweep.set_cc; never extra
flags, and a seed whose header has extra= is skipped).

Nothing is installed: an EXACT variant is a proposal written to try/perm-<tag>/exact/ (with a provenance note for a
generated seed and a trailing `//` line naming the run); it still needs tools/lbpmatch.py EXACT and
tools/lbppromote.py like any other source. Builds go to try/perm-<tag>/build (LBPMATCH_BUILD_ROOT), not build/.
"""
from __future__ import annotations

import argparse
import difflib
import glob
import json
import os
import random
import re
import struct
import subprocess
import sys
from collections import Counter
from pathlib import Path
from typing import Callable

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402
import lbpnearmiss as N  # noqa: E402

DETAIL = re.compile(r"(\d+)/(\d+) words; (\d+) differ")
SIMPLE = re.compile(r"^\s+[^{}\n]*;\s*$")
DECL = re.compile(r"^(\s+)((?:unsigned |signed )?\w+(?:\s*\*+)?)\s+(\w+(?:\s*\[\w+\])?);\s*$")
COMMUTE = re.compile(r"(?<![\w\]\)])(\w+|\(\w+\)) (\+|\*|&|\||\^|==|!=) (\w+|\(\w+\))(?![\w\(\[])")
COMPARE = re.compile(r"(?<![\w\]\)])(\w+) (<=|>=|<|>) (\w+)(?![\w\(\[])")
FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
SIGN = {"int": "uint", "uint": "int", "char": "uchar", "uchar": "char", "short": "ushort", "ushort": "short",
        "undefined4": "int", "undefined2": "short", "undefined1": "char"}

# decomp-permuter's penalty constants (the idea and the numbers, not its code)
REGALLOC, REORDER, INDEL = 5, 60, 100
FAILED = 1_000_000          # weighted score of a variant that did not reach the comparison
FAILED_WORDS = 10_000
SIDEWAYS = 0.02             # chance of accepting an equal-score variant
RESTART = 0.15              # chance of building a variant from the best text instead of the current one


def body_bounds(lines: list[str]) -> tuple[int, int]:
    """Line range of the function body: after the opening brace that follows the signature."""
    start = next(i for i, line in enumerate(lines) if line.strip() == "{" or line.rstrip().endswith(") {")) + 1
    return start, len(lines)


def m_swap(lines, lo, hi, rng):
    cands = [i for i in range(lo, hi - 1) if SIMPLE.match(lines[i]) and SIMPLE.match(lines[i + 1])
             and "return" not in lines[i] + lines[i + 1]
             and len(lines[i]) - len(lines[i].lstrip()) == len(lines[i + 1]) - len(lines[i + 1].lstrip())]
    if not cands:
        return False
    i = rng.choice(cands)
    lines[i], lines[i + 1] = lines[i + 1], lines[i]
    return True


def m_decl(lines, lo, hi, rng):
    decls = [i for i in range(lo, hi) if DECL.match(lines[i]) and not re.search(r"\breturn\b", lines[i])]
    if len(decls) < 2:
        return False
    i, j = rng.sample(decls, 2)
    lines[i], lines[j] = lines[j], lines[i]
    return True


def m_regex(lines, lo, hi, rng, pattern, repl):
    cands = [(i, m) for i in range(lo, hi) for m in pattern.finditer(lines[i]) if not DECL.match(lines[i])]
    if not cands:
        return False
    i, m = rng.choice(cands)
    lines[i] = lines[i][:m.start()] + repl(m) + lines[i][m.end():]
    return True


def m_incdec(lines, lo, hi, rng):
    forms = []
    for i in range(lo, hi):
        line = lines[i]
        m = re.match(r"^(\s+)(\w+) = \2 \+ (-?\d+);\s*$", line)
        if m:
            n = int(m.group(3))
            alts = [f"{m.group(1)}{m.group(2)} += {n};"]
            if n == 1:
                alts.append(f"{m.group(1)}{m.group(2)}++;")
            if n == -1:
                alts.append(f"{m.group(1)}{m.group(2)}--;")
            if n < 0:
                alts.append(f"{m.group(1)}{m.group(2)} = {m.group(2)} - {-n};")
            forms.append((i, alts))
        m = re.search(r"(\w+) \+ -(\d+)", line)
        if m:
            forms.append((i, [line.replace(m.group(0), f"{m.group(1)} - {m.group(2)}", 1)]))
    if not forms:
        return False
    i, alts = rng.choice(forms)
    lines[i] = rng.choice(alts)
    return True


def m_sign(lines, lo, hi, rng):
    decls = [(i, DECL.match(lines[i])) for i in range(lo, hi) if DECL.match(lines[i])]
    decls = [(i, m) for i, m in decls if m.group(2).replace("*", "").strip() in SIGN]
    if not decls:
        return False
    i, m = rng.choice(decls)
    base = m.group(2).replace("*", "").strip()
    lines[i] = lines[i].replace(base, SIGN[base], 1)
    return True


def m_invert(lines, lo, hi, rng):
    """if (C) { A } else { B }  ->  if (!(C)) { B } else { A } (single-level brace matching)."""
    cands = []
    for i in range(lo, hi):
        m = re.match(r"^(\s+)if \((.*)\) \{\s*$", lines[i])
        if not m:
            continue
        depth, j = 1, i + 1
        while j < hi and depth:
            depth += lines[j].count("{") - lines[j].count("}")
            j += 1
        if j < hi + 1 and re.match(r"^\s+\} else \{\s*$", lines[j - 1]):
            depth, k = 1, j
            while k < hi and depth:
                depth += lines[k].count("{") - lines[k].count("}")
                k += 1
            cands.append((i, j - 1, k - 1, m.group(1), m.group(2)))
    if not cands:
        return False
    i, mid, end, indent, cond = rng.choice(cands)
    a, b = lines[i + 1:mid], lines[mid + 1:end]
    lines[i:end + 1] = [f"{indent}if (!({cond})) {{"] + b + [f"{indent}}} else {{"] + a + [f"{indent}}}"]
    return True


ASSIGN_STMT = re.compile(r"^(\s*[^=;]*?[^=!<>+\-*/%&|^]= )(.+);\s*$")


def split_top(expr: str, op: str) -> list[str]:
    """Split expr at top-level ` op ` (outside parentheses and brackets)."""
    parts, depth, start, i = [], 0, 0, 0
    token = f" {op} "
    while i < len(expr):
        ch = expr[i]
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        elif depth == 0 and expr.startswith(token, i):
            parts.append(expr[start:i])
            i += len(token)
            start = i
            continue
        i += 1
    parts.append(expr[start:])
    return parts


def m_reassoc(lines, lo, hi, rng):
    """a * b * c -> a * (b * c): Ghidra drops the parentheses of float product/sum chains, which are not
    associative, so the printed grouping can differ from the computed one."""
    cands = []
    for i in range(lo, hi):
        m = ASSIGN_STMT.match(lines[i])
        if not m:
            continue
        for op in ("*", "+"):
            parts = split_top(m.group(2), op)
            if len(parts) >= 3:
                cands.append((i, m, op, parts))
    if not cands:
        return False
    i, m, op, parts = rng.choice(cands)
    k = rng.randrange(1, len(parts) - 1)
    left = f" {op} ".join(parts[:k])
    right = f" {op} ".join(parts[k:])
    lines[i] = f"{m.group(1)}{left} {op} ({right});"
    return True


PURE = re.compile(r"^[\w\s+\-*<>&|^()]+$")


def m_inline(lines, lo, hi, rng):
    """t = <pure expression>; ... t ...  ->  ... (<expression>) ...: Ghidra hoists repeated subexpressions
    (array index products, field addresses) into temporaries the source wrote inline."""
    cands = []
    for i in range(lo, hi):
        m = re.match(r"^\s+(\w+) = ([^;=]+);\s*$", lines[i])
        if not m or not PURE.match(m.group(2)) or re.search(r"\b\w+\s*\(", m.group(2)):
            continue
        var, expr = m.group(1), m.group(2).strip()
        names = set(re.findall(r"\b[A-Za-z_]\w*\b", expr))
        if var in names:
            continue
        later = lines[i + 1:hi]
        if any(re.match(rf"^\s+{var}\s*(?:[-+*/|&^]|<<|>>)?=(?!=)", ln) for ln in later):
            continue  # reassigned
        if any(re.match(rf"^\s+{n}\s*(?:[-+*/|&^]|<<|>>)?=(?!=)|.*\b{n}\s*(?:\+\+|--)", ln)
               for n in names for ln in later):
            continue  # an operand changes afterwards
        if not any(re.search(rf"\b{var}\b", ln) for ln in later):
            continue
        cands.append((i, var, expr))
    if not cands:
        return False
    i, var, expr = rng.choice(cands)
    simple = re.fullmatch(r"\w+", expr) is not None
    rep = expr if simple else f"({expr})"
    for j in range(i + 1, hi):
        lines[j] = re.sub(rf"\b{var}\b", rep, lines[j])
    del lines[i]
    return True


BOUND = re.compile(r"(?<![\w\]\)])(\w+) (<|<=|>|>=) (-?(?:0x[0-9a-f]+|\d+))(?![\w.])")


def m_bound(lines, lo, hi, rng):
    """i < C  <->  i <= C-1 (and > / >=): the two spellings can pick different compare registers."""
    cands = [(i, m) for i in range(lo, hi) for m in BOUND.finditer(lines[i]) if not DECL.match(lines[i])]
    if not cands:
        return False
    i, m = rng.choice(cands)
    var, op, lit = m.groups()
    n = int(lit, 0)
    new = {"<": ("<=", n - 1), "<=": ("<", n + 1), ">": (">=", n + 1), ">=": (">", n - 1)}[op]
    text = hex(new[1]) if lit.lower().startswith(("0x", "-0x")) and new[1] >= 0 else str(new[1])
    lines[i] = lines[i][:m.start()] + f"{var} {new[0]} {text}" + lines[i][m.end():]
    return True


UCMP = re.compile(r"(?<![\w\])])([\w*]+(?: [-+] (?:0x[0-9a-f]+|\d+))?) (<|<=|>|>=) (0x[0-9a-f]+|\d+)\b")


def m_ucmp(lines, lo, hi, rng):
    """x - 4 < 2  ->  (unsigned int)(x - 4) < 2: Ghidra drops the unsigned cast of range checks (sltiu vs slti)."""
    cands = [(i, m) for i in range(lo, hi) for m in UCMP.finditer(lines[i]) if not DECL.match(lines[i])]
    if not cands:
        return False
    i, m = rng.choice(cands)
    lines[i] = lines[i][:m.start()] + f"(unsigned int)({m.group(1)}) {m.group(2)} {m.group(3)}" + lines[i][m.end():]
    return True


NEG = {"<": ">=", ">=": "<", ">": "<=", "<=": ">", "==": "!=", "!=": "=="}


def negate(cond: str) -> str:
    m = re.fullmatch(r"\s*([\w*.\[\]()+ -]+?) (<|<=|>|>=|==|!=) ([\w*.\[\]()+ -]+?)\s*", cond)
    if m and m.group(1).count("(") == m.group(1).count(")") and m.group(3).count("(") == m.group(3).count(")"):
        return f"{m.group(1)} {NEG[m.group(2)]} {m.group(3)}"
    return f"!({cond})"


def chain_arms(lines, i, hi):
    """Parse a Ghidra if / else if / else chain starting at line i: ([(cond, body)], else_body, end)."""
    arms, j = [], i
    m = re.match(r"^(\s+)if \((.*)\) \{\s*$", lines[j])
    if not m:
        return None
    indent = m.group(1)
    cond = m.group(2)
    while True:
        depth, k = 1, j + 1
        while k < hi and depth:
            depth += lines[k].count("{") - lines[k].count("}")
            k += 1
        if depth:
            return None
        arms.append((cond, lines[j + 1:k - 1]))
        nxt = lines[k] if k < hi else ""
        if lines[k - 1].strip() == "}" and re.match(rf"^{indent}else if \((.*)\) \{{\s*$", nxt):
            cond = re.match(rf"^{indent}else if \((.*)\) \{{\s*$", nxt).group(1)
            j = k
            continue
        if lines[k - 1].strip() == "}" and re.match(rf"^{indent}else \{{\s*$", nxt):
            depth, e = 1, k + 1
            while e < hi and depth:
                depth += lines[e].count("{") - lines[e].count("}")
                e += 1
            return arms, lines[k + 1:e - 1], e, indent
        return None


def m_chain_merge(lines, lo, hi, rng):
    """if (A) {E} else if (B) {X} else {E}  ->  if (!A && B) {X} else {E}: Ghidra splits range checks into
    chains that repeat the else body."""
    cands = []
    for i in range(lo, hi):
        parsed = chain_arms(lines, i, hi)
        if not parsed:
            continue
        arms, else_body, end, indent = parsed
        for a in range(len(arms) - 1):
            if [s.strip() for s in arms[a][1]] == [s.strip() for s in else_body]:
                cands.append((i, arms, else_body, end, indent, a))
    if not cands:
        return False
    i, arms, else_body, end, indent, a = rng.choice(cands)
    merged = (f"{negate(arms[a][0])} && ({arms[a + 1][0]})", arms[a + 1][1])
    arms = arms[:a] + [merged] + arms[a + 2:]
    out = []
    for n, (cond, body) in enumerate(arms):
        out.append(f"{indent}{'if' if n == 0 else 'else if'} ({cond}) {{")
        out += body
        out.append(f"{indent}}}")
    out += [f"{indent}else {{"] + else_body + [f"{indent}}}"]
    lines[i:end] = out
    return True


def m_ternary(lines, lo, hi, rng):
    """if (C) { v = A; } else { v = B; }  ->  v = C ? A : B;"""
    cands = []
    for i in range(lo, hi - 5):
        m = re.match(r"^(\s+)if \((.*)\) \{\s*$", lines[i])
        a = re.match(r"^\s+(\w+) = (.+);\s*$", lines[i + 1])
        b = re.match(r"^\s+(\w+) = (.+);\s*$", lines[i + 4])
        if (m and a and b and lines[i + 2].strip() == "}" and lines[i + 3].strip() == "else {"
                and lines[i + 5].strip() == "}" and a.group(1) == b.group(1)):
            cands.append((i, m.group(1), m.group(2), a.group(1), a.group(2), b.group(2)))
    if not cands:
        return False
    i, indent, cond, var, x, y = rng.choice(cands)
    lines[i:i + 6] = [f"{indent}{var} = ({cond}) ? {x} : {y};"]
    return True


def m_hoist(lines, lo, hi, rng):
    """if (C) { T } else { X; T } -> if (!C) { X } T (Ghidra's duplicated tails; lbpgen.hoist_common_tails)."""
    import lbpgen
    body = "\n".join(lines[lo:hi])
    new = lbpgen.hoist_common_tails(body)
    if new == body:
        return False
    lines[lo:hi] = new.split("\n")
    return True


INNER_SUM = re.compile(r"\(([^()]+)\)")


def m_sumorder(lines, lo, hi, rng):
    """(base + i * 4 + 0x3c) -> (base + 0x3c + i * 4): the order of address terms decides operand order and
    strength reduction."""
    cands = []
    for i in range(lo, hi):
        if DECL.match(lines[i]):
            continue
        for m in INNER_SUM.finditer(lines[i]):
            parts = m.group(1).split(" + ")
            if len(parts) >= 3:
                cands.append((i, m, parts))
    if not cands:
        return False
    i, m, parts = rng.choice(cands)
    rest = parts[1:]
    rng.shuffle(rest)
    if rest == parts[1:]:
        rest = rest[1:] + rest[:1]
    lines[i] = lines[i][:m.start()] + "(" + " + ".join([parts[0]] + rest) + ")" + lines[i][m.end():]
    return True


# ---- operators of hst-decomp's hstperm.py (2026-11-02); each keeps the text a single readable C function ----
KEYWORDS = frozenset("if else for while do return switch case default goto break continue sizeof int unsigned signed char "
                     "short long void struct union enum const static register volatile float double this new delete "
                     "true false".split())
TYPE_WORDS = frozenset("int unsigned signed char short long uint ushort uchar byte undefined undefined1 undefined2 "
                       "undefined4 undefined8 float double void struct const bool".split())
CAST = re.compile(r"\((?:unsigned int|unsigned char|unsigned short|int|uint|short|ushort|char|uchar|byte|undefined[124])\) ?")
STMT = re.compile(r"^(\s+)(?!return\b|for\b|while\b|if\b|else\b|do\b|switch\b|case\b)[^{};]*;\s*$")


def is_decl(line: str) -> bool:
    """A local declaration (DECL also matches `return a[i];`)."""
    return bool(DECL.match(line)) and not re.match(r"\s+return\b", line)


def indent_of(line: str) -> int:
    return len(line) - len(line.lstrip())


def block_end(lines, i, hi):
    """Index of the line holding the brace that closes the block opened on line i, or None."""
    depth = 0
    for k in range(i, min(hi, len(lines))):
        depth += lines[k].count("{") - lines[k].count("}")
        if depth == 0 and k > i:
            return k
    return None


def m_temp(lines, lo, hi, rng):
    """... (A + B) ... -> int tN = A + B; ... tN ...: a temporary for a parenthesised, call-free sub-expression
    (a declaration after statements: C99, C++, and GCC's C extension)."""
    cands = []
    for i in range(max(lo, 1), hi):
        ln = lines[i]
        if not STMT.match(ln) or is_decl(ln) or lines[i - 1].strip() not in ("",) and not lines[i - 1].rstrip().endswith(("{", ";", "}")):
            continue
        for m in INNER_SUM.finditer(ln):
            inner = m.group(1).strip()
            if (" " in inner and PURE.match(inner) and not CAST.fullmatch("(" + inner + ")") and "," not in inner
                    and not re.search(r"[\w\]]$", ln[:m.start()].rstrip()[-1:])
                    and not ln[:m.start()].rstrip().endswith(("sizeof", "-"))):
                cands.append((i, m, inner))
    if not cands:
        return False
    i, m, inner = rng.choice(cands)
    text = "\n".join(lines)
    n = 0
    while re.search(rf"\bt{n}\b", text):
        n += 1
    ind = lines[i][:indent_of(lines[i])]
    lines[i] = lines[i][:m.start()] + f"t{n}" + lines[i][m.end():]
    lines.insert(i, f"{ind}int t{n} = {inner};")
    return True


def m_cast(lines, lo, hi, rng):
    """Add or remove a value-preserving cast on an operand."""
    removals = [(i, m) for i in range(lo, hi) for m in CAST.finditer(lines[i]) if not is_decl(lines[i])]
    adds = []
    for i in range(lo, hi):
        ln = lines[i]
        if is_decl(ln) or not STMT.match(ln) or "=" not in ln:
            continue
        rhs = ln.index("=") + 1
        for m in re.finditer(r"(?<![\w.>&*(:])([A-Za-z_]\w*)(?![\w(\[.>:])", ln):
            if (m.start() >= rhs and m.group(1) not in KEYWORDS and m.group(1) not in TYPE_WORDS
                    and ln[:m.start()].count("(") <= ln[:m.start()].count(")")):
                adds.append((i, m))
    if not removals and not adds:
        return False
    if removals and (not adds or rng.random() < 0.5):
        i, m = rng.choice(removals)
        lines[i] = lines[i][:m.start()] + lines[i][m.end():]
    else:
        i, m = rng.choice(adds)
        t = rng.choice(("int", "unsigned int", "int"))
        lines[i] = lines[i][:m.start()] + f"({t}){m.group(1)}" + lines[i][m.end():]
    return True


COMPOUND = re.compile(r"(?<![\w.\]>])(\w+) = \1 (\+|-|\*|\||&|\^|<<|>>) ([^;)]+?)(?=;|\))")
COMPOUND_ASSIGN = re.compile(r"(?<![\w.\]>])(\w+) (\+|-|\*|\||&|\^|<<|>>)= ([^;)]+?)(?=;|\))")


def m_compound(lines, lo, hi, rng):
    """x = x op y  <->  x op= y"""
    cands = []
    for i in range(lo, hi):
        if is_decl(lines[i]):
            continue
        cands += [(i, m, True) for m in COMPOUND.finditer(lines[i])]
        cands += [(i, m, False) for m in COMPOUND_ASSIGN.finditer(lines[i])]
    if not cands:
        return False
    i, m, forward = rng.choice(cands)
    if forward:
        new = f"{m.group(1)} {m.group(2)}= {m.group(3)}"
    else:
        y = m.group(3)
        if re.search(r" (\+|-|\||&|\^|<<|>>|\?) ", y) and not (y.startswith("(") and y.endswith(")")):
            y = f"({y})"
        new = f"{m.group(1)} = {m.group(1)} {m.group(2)} {y}"
    lines[i] = lines[i][:m.start()] + new + lines[i][m.end():]
    return True


INDEX = re.compile(r"(?<![\w.>])([A-Za-z_]\w*)\[(\w+)\]")
DEREF_SUM = re.compile(r"\*\(([A-Za-z_]\w*) \+ (\w+)\)")


def m_index(lines, lo, hi, rng):
    """a[i] <-> *(a + i)"""
    cands = []
    for i in range(lo, hi):
        if is_decl(lines[i]):
            continue
        cands += [(i, m, True) for m in INDEX.finditer(lines[i])]
        cands += [(i, m, False) for m in DEREF_SUM.finditer(lines[i])]
    if not cands:
        return False
    i, m, forward = rng.choice(cands)
    new = f"*({m.group(1)} + {m.group(2)})" if forward else f"{m.group(1)}[{m.group(2)}]"
    lines[i] = lines[i][:m.start()] + new + lines[i][m.end():]
    return True


def m_decl_scope(lines, lo, hi, rng):
    """Move a declaration to just before its first use (or into the block that holds every use), or up to the top."""
    base = min((indent_of(lines[i]) for i in range(lo, hi) if lines[i].strip() and indent_of(lines[i])), default=0)
    cands = []
    for i in range(lo, hi):
        m = DECL.match(lines[i]) if is_decl(lines[i]) else None
        if not m:
            continue
        var = re.match(r"\w+", m.group(3)).group(0)
        uses = [k for k in range(lo, hi) if k != i and re.search(rf"\b{var}\b", lines[k])]
        if not uses:
            continue
        d, first = indent_of(lines[i]), uses[0]
        if indent_of(lines[first]) == d and first > i + 1:
            cands.append(("sink", i, first, d))
        elif indent_of(lines[first]) > d and first > i:
            k = first
            while k > i and not (lines[k].rstrip().endswith("{") and indent_of(lines[k]) < indent_of(lines[first])):
                k -= 1
            e = block_end(lines, k, hi) if k > i else None
            if e is not None and all(k < u < e for u in uses):
                cands.append(("into", i, k + 1, indent_of(lines[k + 1])))
        if d > base:
            cands.append(("up", i, lo, base))
    if not cands:
        return False
    kind, i, at, ind = rng.choice(cands)
    text = " " * ind + lines[i].strip()
    del lines[i]
    if at > i:
        at -= 1
    lines.insert(at, text)
    return True


FOR_HDR = re.compile(r"^(\s*)for \((.*?);(.*?);(.*?)\) \{\s*$")
WHILE_HDR = re.compile(r"^(\s*)while \((.+)\) \{\s*$")


def m_loop_form(lines, lo, hi, rng):
    """for <-> while <-> if + do-while with an equivalent guard."""
    cands = []
    for i in range(lo, hi):
        e = block_end(lines, i, hi) if lines[i].rstrip().endswith("{") else None
        if e is None:
            continue
        if any(re.search(r"\bcontinue\b", b) for b in lines[i + 1:e]):
            continue
        if FOR_HDR.match(lines[i]):
            cands.append(("for2while", i, e))
        elif WHILE_HDR.match(lines[i]):
            cands += [("while2for", i, e), ("while2do", i, e)]
    if not cands:
        return False
    kind, i, e = rng.choice(cands)
    if kind == "for2while":
        ind, init, cond, step = FOR_HDR.match(lines[i]).groups()
        init, cond, step = init.strip(), cond.strip() or "1", step.strip()
        head = ([f"{ind}{init};"] if init else []) + [f"{ind}while ({cond}) {{"]
        tail = ([f"{ind}  {step};"] if step else []) + [lines[e]]
        lines[i:e + 1] = head + lines[i + 1:e] + tail
    elif kind == "while2for":
        ind, cond = WHILE_HDR.match(lines[i]).groups()
        lines[i] = f"{ind}for (; {cond}; ) {{"
    else:
        ind, cond = WHILE_HDR.match(lines[i]).groups()
        body = [f"  {b}" if b.strip() else b for b in lines[i + 1:e]]
        lines[i:e + 1] = ([f"{ind}if ({cond}) {{", f"{ind}  do {{"] + body +
                          [f"{ind}  }} while ({cond});", f"{ind}}}"])
    return True


def m_register(lines, lo, hi, rng):
    """Add or remove `register` on a local declaration (GCC still reads it as an allocation hint at -O0 and when
    it orders pseudos; for C++ it is only a deprecated keyword, so the operator stays on)."""
    plain = [i for i in range(lo, hi) if is_decl(lines[i]) and "register" not in lines[i] and "[" not in lines[i]]
    regs = [i for i in range(lo, hi) if re.match(r"^\s+register ", lines[i])]
    if not plain and not regs:
        return False
    if regs and (not plain or rng.random() < 0.4):
        i = rng.choice(regs)
        lines[i] = lines[i].replace("register ", "", 1)
    else:
        i = rng.choice(plain)
        lines[i] = lines[i][:indent_of(lines[i])] + "register " + lines[i].lstrip()
    return True


FIELD = r"(?:\w+(?:->|\.))+\w+|\w+"
FIELD_COMMUTE = re.compile(rf"(?<![\w\]\)>.])({FIELD}) (\+|\*|&|\||\^|==|!=) ({FIELD})(?![\w(\[.])")


def m_field_commute(lines, lo, hi, rng):
    """a->x + b->y -> b->y + a->x: reorders independent struct-field reads (COMMUTE only sees plain names)."""
    return m_regex(lines, lo, hi, rng, FIELD_COMMUTE, lambda m: f"{m.group(3)} {m.group(2)} {m.group(1)}")


# ---- zero tests (2026-11-10): !x <-> x == 0, and x != 0 -> x in a condition ----
OPERAND = r"(?:\w+(?:->|\.))*\w+"
# an operand position where `!x`, `x == 0` and `x` all parse as the whole operand: after `(`, `&& `, `|| `,
# `return ` or an assignment's `= `, and before `)`, `;`, ` &&` or ` ||`
_LEFT = r"(?:(?<=\()|(?<=&& )|(?<=\|\| )|(?<=return )|(?<=[^=!<>]= ))"
_RIGHT = r"(?=\)|;| &&| \|\|)"
NOT_X = re.compile(rf"{_LEFT}!({OPERAND}){_RIGHT}")
X_EQ0 = re.compile(rf"{_LEFT}({OPERAND}) == 0{_RIGHT}")
X_NE0 = re.compile(rf"(?:(?<=\()|(?<=&& )|(?<=\|\| ))({OPERAND}) != 0(?=\)| &&| \|\|)")
CONDITION = re.compile(r"\b(?:if|while|for)\s*\(|\?")


def m_zero_test(lines, lo, hi, rng):
    """!x <-> x == 0, and x != 0 -> x where it is a condition (if/while/for or a ternary's): GCC can expand the
    spellings to different compare sequences (sltiu vs xori/sltu, beqz vs bnez with the arms swapped)."""
    cands = []
    for i in range(lo, hi):
        ln = lines[i]
        if is_decl(ln):
            continue
        cands += [(i, m, f"{m.group(1)} == 0") for m in NOT_X.finditer(ln)]
        cands += [(i, m, f"!{m.group(1)}") for m in X_EQ0.finditer(ln)]
        if CONDITION.search(ln):
            cands += [(i, m, m.group(1)) for m in X_NE0.finditer(ln)]
    if not cands:
        return False
    i, m, new = rng.choice(cands)
    lines[i] = lines[i][:m.start()] + new + lines[i][m.end():]
    return True


NEW_MUTATIONS = [(2, m_temp), (2, m_cast), (2, m_compound), (2, m_index), (2, m_decl_scope), (2, m_loop_form),
                 (1, m_register), (2, m_field_commute), (2, m_zero_test)]

MUTATIONS = [
    (3, m_swap), (2, m_decl), (2, lambda l, lo, hi, r: m_regex(l, lo, hi, r, COMMUTE, lambda m: f"{m.group(3)} {m.group(2)} {m.group(1)}")),
    (2, lambda l, lo, hi, r: m_regex(l, lo, hi, r, COMPARE, lambda m: f"{m.group(3)} {FLIP[m.group(2)]} {m.group(1)}")),
    (1, m_incdec), (2, m_sign), (2, m_invert), (2, m_reassoc), (2, m_inline), (2, m_sumorder), (2, m_bound),
    (1, m_ucmp), (2, m_chain_merge), (1, m_ternary), (1, m_hoist),
] + NEW_MUTATIONS


# ---- where to mutate: the header symbol's definition ----
_STRING = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'')
_SIG_PREFIX = re.compile(r'^[\w\s\*&:<>,~"]*$')


def _code_lines(text: str) -> list[str]:
    """The text's lines with comments and string contents blanked (line numbers kept), for brace counting."""
    import lbpgate
    return [_STRING.sub('""', line) for line in lbpgate.strip_comments(text).split("\n")]


def source_name(symbol: str) -> str | None:
    """How a definition of `symbol` is spelled: the symbol itself for C; `A::B::f` (`A::A`, `A::~A` for constructors
    and destructors) for a plain Itanium-mangled C++ name; None for anything else (templates, std::, operators)."""
    if not symbol.startswith("_Z"):
        return symbol
    s, nested, names = symbol[2:], False, []
    if s.startswith("N"):
        nested, s = True, s[1:]
        while s[:1] in ("K", "V", "r"):
            s = s[1:]
    while True:
        m = re.match(r"\d+", s)
        if m:
            n = int(m.group(0))
            s = s[m.end():]
            if len(s) < n:
                return None
            names.append(s[:n])
            s = s[n:]
            if not nested:
                break
            continue
        if nested and names and re.match(r"C[123]", s):
            names.append(names[-1])
            s = s[2:]
            continue
        if nested and names and re.match(r"D[012]", s):
            names.append("~" + names[-1])
            s = s[2:]
            continue
        break
    if not names or (nested and not s.startswith("E")):
        return None
    return "::".join(names)


def function_span(text: str, symbol: str) -> tuple[int, int] | None:
    """(first, last) line indices of the definition of `symbol` (a name as source_name spells it): from the first
    line of its signature to the line of its closing brace. None when the text does not define it by that name."""
    code = _code_lines(text)
    sig = re.compile(rf"(?<![\w.>:]){re.escape(symbol)}\s*\(")
    for d, line in enumerate(code):
        m = sig.search(line)
        if not m or not _SIG_PREFIX.match(line[:m.start()]) or re.search(r"\b(?:return|if|while|for|switch|else|case)\b",
                                                                         line[:m.start()]):
            continue
        rest = "\n".join(code[d:])
        pos, depth = rest.index("(", m.start()), 0
        for pos in range(pos, len(rest)):
            depth += {"(": 1, ")": -1}.get(rest[pos], 0)
            if depth == 0:
                break
        tail = rest[pos + 1:]
        brace, semi = tail.find("{"), tail.find(";")
        if brace < 0 or (0 <= semi < brace):
            continue                                    # a declaration
        depth, end = 0, None
        for i in range(d, len(code)):
            for ch in (code[i] if i > d else code[i][m.start():]):
                depth += {"{": 1, "}": -1}.get(ch, 0)
                if ch == "}" and depth == 0:
                    end = i
                    break
            if end is not None:
                break
        if end is None:
            return None
        first = d
        while first > 1 and code[first - 1].strip() and not code[first - 1].rstrip().endswith((";", "}", "{")) \
                and not code[first - 1].lstrip().startswith("#"):
            first -= 1
        return first, end
    return None


def body_range(text: str, symbol: str | None = None) -> tuple[int, int]:
    """(lo, hi) lines a mutation may touch: the body of the header symbol's definition when the text defines it by
    name (hi = its closing-brace line), else body_bounds (to the end of the text). StopIteration when neither."""
    name = source_name(symbol) if symbol else None
    span = function_span(text, name) if name else None
    if span is not None:
        code = _code_lines(text)
        first, last = span
        k = first
        while k < last and "{" not in code[k]:
            k += 1
        return k + 1, last
    return body_bounds(text.split("\n"))


def mutate(text: str, rng: random.Random, symbol: str | None = None) -> str | None:
    lines = text.split("\n")
    try:
        lo, hi = body_range(text, symbol)
    except StopIteration:
        return None
    if hi <= lo:
        return None
    tail = len(lines) - hi                  # lines after the mutated range stay as they are
    applied = 0
    for _ in range(rng.choice((1, 1, 2, 3))):
        weights = [w for w, _ in MUTATIONS]
        fn = rng.choices([f for _, f in MUTATIONS], weights)[0]
        if fn(lines, lo, len(lines) - tail, rng):
            applied += 1
    return "\n".join(lines) if applied else None


# ---- the score ----

def weighted_score(ours: list[int], theirs: list[int]) -> int:
    """decomp-permuter-style penalties over our words and the retail words (see the module docstring): align the
    register-masked words; aligned pairs that still differ are register-only (REGALLOC each); of the unaligned words,
    a pair with the same masked word on both sides is a moved instruction (REORDER per pair), and the rest is missing,
    extra or substituted (INDEL per word of the longer side)."""
    if ours == theirs:
        return 0
    ko, kt = [N.regmask(w) for w in ours], [N.regmask(w) for w in theirs]
    sm = difflib.SequenceMatcher(None, ko, kt, autojunk=False)
    total, only_o, only_t = 0, [], []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            total += REGALLOC * sum(1 for a, b in zip(ours[i1:i2], theirs[j1:j2]) if a != b)
        else:
            only_o += ko[i1:i2]
            only_t += kt[j1:j2]
    moved = sum((Counter(only_o) & Counter(only_t)).values())
    return total + REORDER * moved + INDEL * max(len(only_o) - moved, len(only_t) - moved)


def score(status: str, detail: str) -> int:
    """Positional score from an lbpmatch row (used only to pick seed candidates)."""
    if status == "EXACT":
        return 0
    m = DETAIL.match(detail or "")
    if status == "MISMATCH" and m:
        ours, n, d = map(int, m.groups())
        return d if ours == n else 1000 + abs(ours - n) * 10 + d
    return 10_000


_RETAIL: dict = {}


def retail_words(key: tuple[str, int], end: int) -> list[int]:
    module, start = key
    data, seg = M.module_bytes(_RETAIL, module)
    off = seg["file_offset"] + start - seg["start"]
    raw = data[off:off + end - start]
    return list(struct.unpack(f"<{len(raw) // 4}I", raw[:len(raw) // 4 * 4]))


def score_row(row: dict, path: Path, theirs: list[int], build_root: Path) -> tuple[int, int]:
    """(weighted score, words left) of one lbpmatch row: (0, 0) only for EXACT; FAILED for a row that did not reach
    the comparison (or any status but MISMATCH: a gate refusal or a data difference is not progress)."""
    if row["status"] == "EXACT":
        return 0, 0
    if row["status"] != "MISMATCH":
        return FAILED, FAILED_WORDS
    h = M.parse_header(path)
    body = N.our_body(build_root / h["module"] / M.build_key(path), h)
    if body is None:
        return FAILED, FAILED_WORDS
    ours = N.words(body)
    return max(1, weighted_score(ours, theirs)), max(1, N.words_aligned_of(ours, theirs))


def accept(new: int, current: int, rng: random.Random, sideways: float = SIDEWAYS) -> bool:
    """Hill climbing with plateau moves: a lower score always, an equal one with probability `sideways`."""
    if new < current:
        return True
    return new == current and rng.random() < sideways


def base_text(entry: dict, rng: random.Random, restart: float = RESTART) -> str:
    """The text a new variant starts from: the best text with probability `restart`, else the current one."""
    return entry["best_text"] if rng.random() < restart else entry["text"]


Runner = Callable[[list[Path], Path, dict], dict]


def make_runner(build_root: Path, parallel: int, ends: dict) -> Runner:
    """runner(files, part_dir, owner) -> {resolved path text: (weighted score, words left, status)}: lbpmatch.py
    --quiet --json over the files (at most 40 files and about 24,000 characters of paths per run, for the Windows
    command line), building into build_root; a COMPILE_CRASHED file is compiled once more."""
    env = dict(os.environ, LBPMATCH_BUILD_ROOT=str(build_root))
    cmd = [sys.executable, str(Path(M.__file__)), "--quiet", "--parallel", str(parallel), "--json"]

    def run(files: list[Path], part_dir: Path, owner: dict) -> dict:
        results: dict = {}
        crashed = unscored = 0
        part_dir.mkdir(parents=True, exist_ok=True)
        chunks: list[list[str]] = [[]]
        for p in files:
            s = str(p)
            if chunks[-1] and (len(chunks[-1]) >= 40 or sum(len(x) + 1 for x in chunks[-1]) + len(s) > 24_000):
                chunks.append([])
            chunks[-1].append(s)
        for c, todo in enumerate(chunks if files else []):
            for attempt in range(2):
                part = part_dir / f"summary-{c}-{attempt}.json"
                part.unlink(missing_ok=True)
                done = subprocess.run(cmd + [str(part)] + todo, stdout=subprocess.DEVNULL, env=env)
                if done.returncode == 3 or not part.exists():
                    raise SystemExit(f"lbpmatch stopped (exit {done.returncode}): wsl.exe or the compiler machinery "
                                     "failed; not scoring this part. Re-run after WSL answers again.")
                again = []
                for row in json.loads(part.read_text(encoding="utf-8")):
                    path = (M.ROOT / row["source"]).resolve()
                    if row["status"] == "COMPILE_CRASHED":
                        crashed += attempt == 0
                        if attempt == 0:
                            again.append(str(path))
                        else:
                            unscored += 1
                        continue
                    key = owner[str(path)]
                    s, w = score_row(row, path, retail_words(key, ends[key]), build_root)
                    results[str(path)] = (s, w, row["status"])
                todo = again
                if not todo:
                    break
        if crashed:
            print(f"compiler crashes: {crashed} compiled again, {unscored} still crashed and left unscored", flush=True)
        return results
    return run


# ---- the search, shared with tools/lbpnearperm.py ----

def new_entry(text: str, origin: str, suffix: str, symbol: str, s: int = FAILED, w: int = FAILED_WORDS) -> dict:
    return {"score": s, "words": w, "text": text, "best_score": s, "best_words": w, "best_text": text,
            "origin": origin, "suffix": suffix, "symbol": symbol, "original": text}


def take(entry: dict, s: int, w: int, text: str) -> bool:
    """Make (s, w, text) the entry's current state; True when it is also a new best."""
    entry.update(score=s, words=w, text=text)
    if s < entry["best_score"]:
        entry.update(best_score=s, best_words=w, best_text=text)
        return True
    return False


def search(work: Path, tag: str, state: dict, keys: list, rounds: int, mutants: int, rng: random.Random, run: Runner,
           on_exact: Callable[[tuple, Path, str], None], sideways: float = SIDEWAYS, restart: float = RESTART,
           accepted: dict | None = None) -> tuple[dict, int]:
    """Rounds of mutation and selection over state[key] (new_entry dicts). Returns ({key: exact path}, compiles)."""
    accepted = {} if accepted is None else accepted
    compiles = 0
    for rnd in range(rounds):
        active = [k for k in keys if k not in accepted]
        if not active:
            break
        files, owner = [], {}
        for idx, key in enumerate(active):
            st = state[key]
            seen = {st["text"], st["best_text"]}
            for k in range(mutants):
                text = mutate(base_text(st, rng, restart), rng, st["symbol"])
                if text is None or text in seen:
                    continue
                seen.add(text)
                path = work / f"r{rnd:02d}" / f"{tag}_f{idx:04d}_{key[1]:08x}_m{k:02d}{st['suffix']}"
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text, encoding="utf-8", newline="\n")
                files.append(path)
                owner[str(path.resolve())] = key
        compiles += len(files)
        results = run(files, work / f"r{rnd:02d}", owner)
        improved = 0
        for path_text, (s, w, _status) in sorted(results.items()):
            key = owner[path_text]
            if key in accepted:
                continue
            if s == 0:
                accepted[key] = Path(path_text)
                on_exact(key, Path(path_text), "")
            elif accept(s, state[key]["score"], rng, sideways):
                improved += take(state[key], s, w, Path(path_text).read_text(encoding="utf-8"))
        print(f"round={rnd} variants={len(files)} exact_total={len(accepted)} improved={improved} compiles={compiles}",
              flush=True)
    return accepted, compiles


def locked_profiles() -> tuple[list[str], str]:
    """(locked profiles in the manifest's order, primary profile), as tools/lbpsweep.py reads them."""
    import lbpsweep as S
    locked, primary, _digest = S.lock_profiles(json.loads(S.MANIFEST.read_text(encoding="utf-8")))
    return locked, primary


def profile_variants(text: str, locked: list[str], primary: str) -> list[tuple[str, str]]:
    """[(profile, text with only cc= changed)] for every locked profile but the text's own; none for a text whose
    first line is not a closed LBPMATCH header or whose header has extra= (never extra flags)."""
    import lbpsweep as S
    h = S.header_of(text)
    if h is None or "extra" in h:
        return []
    own = h.get("cc", primary)
    return [(p, S.set_cc(text, p)) for p in locked if p != own]


def profile_phase(work: Path, tag: str, state: dict, keys: list, accepted: dict, run: Runner,
                  locked: list[str], primary: str, on_exact: Callable[[tuple, Path, str], None]) -> tuple[dict, dict, int]:
    """The best text of every open seed under each other locked profile. Returns ({key: profile} of the exact ones,
    {origin: {profile: score}} of those that lowered the score without reaching zero, compiles)."""
    files, owner, label = [], {}, {}
    for idx, key in enumerate(k for k in keys if k not in accepted):
        st = state[key]
        for prof, new in profile_variants(st["best_text"], locked, primary):
            path = work / "profiles" / prof / f"{tag}_p{idx:04d}_{key[1]:08x}{st['suffix']}"
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(new, encoding="utf-8", newline="\n")
            files.append(path)
            owner[str(path.resolve())] = key
            label[str(path.resolve())] = prof
    results = run(files, work / "profiles", owner) if files else {}
    exact: dict = {}
    better: dict = {}
    for path_text, (s, _w, _status) in sorted(results.items()):
        key, prof = owner[path_text], label[path_text]
        if key in accepted:
            continue
        if s == 0:
            accepted[key] = Path(path_text)
            exact[key] = prof
            on_exact(key, Path(path_text), prof)
        elif s < state[key]["best_score"]:
            better.setdefault(state[key]["origin"], {})[prof] = s
    print(f"profile phase: variants={len(files)} exact={len(exact)} improved_files={len(better)}", flush=True)
    return exact, better, len(files)


def write_proposal(tag: str, dest_dir: Path, path: Path, origin: str, profile: str = "", tool: str = "lbpperm.py") -> Path:
    """An exact variant as a proposal under dest_dir (never src/): a generated seed's note says what refined it, and
    a trailing `//` line names the run (lbpsweep.strip_trailing_comments drops it). The proposal still needs
    tools/lbpmatch.py EXACT and tools/lbppromote.py."""
    text = path.read_text(encoding="utf-8")
    gen = re.search(r"gen-(\w+)[\\/]", origin)
    if gen:
        head, rest = text.split("\n", 1)
        rest = re.sub(r"^/\*\s*Mechanical translation.*?\*/\n", "", rest, count=1)
        text = (head + "\n" + f"/* Mechanical translation of Ghidra pseudocode (lbpgen.py tag {gen.group(1)}), refined by "
                f"{tool} tag {tag} random mutation from {origin}; names and types await human review. */\n" + rest)
    under = f" under cc={profile}" if profile else ""
    text = text.rstrip("\n") + f"\n// proposal: {tool} tag {tag} found this variant of {origin} exact{under}; " \
                               "verify with lbpmatch.py and lbppromote.py\n"
    dest_dir.mkdir(parents=True, exist_ok=True)
    out = dest_dir / (re.sub(r"\.(c|cpp)$", "", Path(origin).name) + path.suffix)
    out.write_text(text, encoding="utf-8", newline="\n")
    return out


# ---- seeds ----

def nearmiss_left() -> dict[str, int]:
    """words left per wip path from try/nearmiss-report.json (tools/lbpnearmiss.py), when there is one."""
    if not N.REPORT_JSON.exists():
        return {}
    try:
        rows = json.loads(N.REPORT_JSON.read_text(encoding="utf-8")).get("files", [])
    except ValueError:
        return {}
    return {r["wip"]: r["words_left"] for r in rows if r.get("words_left") is not None}


def wip_seed(p: Path, covered: set, max_words: int) -> tuple[tuple[str, int], str] | None:
    """(key, text) of a wip near miss the permuter can work on, else None."""
    import lbplane as L
    import lbpsweep as S
    text = L.wip_text(p)
    h = S.header_of(text)
    if h is None or "extra" in h or "helpers" in h or h.get("cc", M.PRIMARY_PROFILE) not in S.KNOWN_PROFILES:
        return None
    key = (h["module"], h["start"])
    if key in covered or (h["end"] - h["start"]) // 4 > max_words:
        return None
    return key, text


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", type=Path, help="seed only these near-miss files (any words left)")
    ap.add_argument("--tag", required=True)
    ap.add_argument("--max-diff", type=int, default=8, help="seed only targets with at most this many words left")
    ap.add_argument("--rounds", type=int, default=12)
    ap.add_argument("--mutants", type=int, default=16)
    ap.add_argument("--parallel", type=int, default=2)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--pre-diff", type=int, default=0,
                    help="seed only targets whose best recorded mismatch differs in at most this many words")
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--seeds", type=int, default=3, help="seed candidates per target, recompiled and scored")
    ap.add_argument("--max-words", type=int, default=400, help="skip retail functions longer than this")
    ap.add_argument("--start-min", type=lambda s: int(s, 0), default=0)
    ap.add_argument("--start-max", type=lambda s: int(s, 0), default=0xFFFFFFFF)
    ap.add_argument("--wip", action="store_true", help="also seed from the near misses in wip/")
    ap.add_argument("--no-profiles", action="store_true", help="skip the locked-profile phase after the rounds")
    ap.add_argument("--sideways", type=float, default=SIDEWAYS)
    ap.add_argument("--restart", type=float, default=RESTART)
    args = ap.parse_args()
    import lbplane as L
    rng = random.Random(args.seed)
    covered = L.covered(M.ROOT)
    work = M.ROOT / "try" / f"perm-{args.tag}"
    if work.exists():
        raise SystemExit(f"{work} exists; use a new tag")
    in_range = lambda key: args.start_min <= key[1] <= args.start_max  # noqa: E731
    # pool: {key: {source: (pre-score, text)}}
    pool: dict[tuple[str, int], dict[str, tuple[int, str]]] = {}
    if args.files:
        for f in args.files:
            p = f if f.is_absolute() else M.ROOT / f
            got = wip_seed(p, covered, 1 << 30)
            if got is None:
                raise SystemExit(f"{f}: not a seed (header not on line 1, extra=/helpers=, unknown cc= or covered)")
            pool.setdefault(got[0], {})[N.rel(p)] = (0, got[1])
    else:
        for summary in glob.glob(str(M.ROOT / "try" / "gen-*" / "summary-*.json")):
            for row in json.loads(Path(summary).read_text(encoding="utf-8")):
                key = (row["module"], int(row["start"], 16))
                if key in covered or row["status"] != "MISMATCH" or not in_range(key):
                    continue
                m = DETAIL.match(row.get("detail") or "")
                if not m or int(m.group(2)) > args.max_words:
                    continue
                src = M.ROOT / row["source"]
                if src.exists():
                    pool.setdefault(key, {})[row["source"]] = (score(row["status"], row.get("detail", "")),
                                                               src.read_text(encoding="utf-8"))
        if args.wip:
            left = nearmiss_left()
            for p in sorted((M.ROOT / "wip").iterdir()):
                if not (p.is_file() and p.suffix in (".c", ".cpp")):
                    continue
                got = wip_seed(p, covered, args.max_words)
                if got is not None and in_range(got[0]):
                    pool.setdefault(got[0], {})[N.rel(p)] = (left.get(N.rel(p), 0), got[1])
        if args.pre_diff:  # recompiling every seed of thousands of targets takes hours
            pool = {k: v for k, v in pool.items() if min(s for s, _t in v.values()) <= args.pre_diff}
    seed_files, owner, ends, origin, symbols = [], {}, {}, {}, {}
    for idx, key in enumerate(sorted(pool)):
        seen_text: set = set()
        for k, source in enumerate(sorted(pool[key], key=lambda s: pool[key][s][0])[:args.seeds]):
            text = pool[key][source][1]
            if text in seen_text:
                continue
            seen_text.add(text)
            h = M.HEADER.search(text[:2000])
            fields = {a: b.strip('"') for a, b in re.findall(r'(\w+)=("[^"]*"|\S+)', h.group(1))} if h else {}
            ends[key] = int(fields["end"], 16)
            symbols[key] = fields["symbol"]
            path = work / "seed" / f"{args.tag}_s{idx:05d}_{k}{Path(source).suffix}"
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8", newline="\n")
            seed_files.append(path)
            owner[str(path.resolve())] = key
            origin[str(path.resolve())] = source
    run = make_runner(work / "build", args.parallel, ends)
    print(f"seeding targets={len(pool)} seed_files={len(seed_files)}", flush=True)
    seeded = run(seed_files, work / "seed", owner)
    compiles = len(seed_files)
    state: dict = {}
    exact_dir = work / "exact"
    exact: dict = {}

    def on_exact(key: tuple, path: Path, profile: str) -> None:
        out = write_proposal(args.tag, exact_dir, path, state[key]["origin"], profile)
        exact[key] = {"proposal": N.rel(out), "origin": state[key]["origin"], "profile": profile or None}
        print(f"EXACT {key[0]} 0x{key[1]:08x} {state[key]['origin']}{' [cc=' + profile + ']' if profile else ''} "
              f"-> {N.rel(out)}", flush=True)

    accepted: dict = {}
    for path_text, (s, w, _status) in sorted(seeded.items()):
        key = owner[path_text]
        if not args.files and w > args.max_diff:
            continue
        if key not in state or s < state[key]["score"]:
            text = Path(path_text).read_text(encoding="utf-8")
            state[key] = new_entry(text, origin[path_text], Path(path_text).suffix, symbols[key], s, w)
        if s == 0 and key not in accepted:
            accepted[key] = Path(path_text)
            on_exact(key, Path(path_text), "")
    keys = sorted(state, key=lambda k: state[k]["score"])
    if args.limit:
        keys = keys[:args.limit]
    print(f"targets={len(keys)} by_words=" + json.dumps(dict(sorted(Counter(state[k]['words'] for k in keys).items()))),
          flush=True)
    _acc, n = search(work, args.tag, state, keys, args.rounds, args.mutants, rng, run, on_exact, args.sideways,
                     args.restart, accepted)
    compiles += n
    profile_better: dict = {}
    if not args.no_profiles:
        locked, primary = locked_profiles()
        _exact, profile_better, n = profile_phase(work, args.tag, state, keys, accepted, run, locked, primary,
                                                         on_exact)
        compiles += n
    # keep each unfinished target's best variant, so agents can start from it
    best_dir = work / "best"
    best_dir.mkdir(parents=True, exist_ok=True)
    best_index = {}
    for key in keys:
        if key in accepted:
            continue
        st = state[key]
        path = best_dir / f"{key[0]}_{key[1]:08x}{st['suffix']}"
        path.write_text(st["best_text"], encoding="utf-8", newline="\n")
        best_index[f"{key[0]}:0x{key[1]:08x}"] = {"score": st["best_score"], "words_left": st["best_words"],
                                                  "source": N.rel(path), "origin": st["origin"]}
    (work / "best.json").write_text(json.dumps(best_index, indent=1) + "\n", encoding="utf-8")
    report = {"tag": args.tag, "targets": len(keys), "compiles": compiles, "exact": len(exact),
              "exact_files": {f"{k[0]}:0x{k[1]:08x}": v for k, v in sorted(exact.items())},
              "profile_improved": profile_better,
              "remaining_by_words": dict(sorted(Counter(state[k]["best_words"] for k in keys
                                                        if k not in accepted).items()))}
    (work / "report.json").write_text(json.dumps(report, indent=1) + "\n", encoding="utf-8")
    print(json.dumps({k: v for k, v in report.items() if k != "exact_files"}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
