#!/usr/bin/env python3
"""Mechanical first-pass reconstruction: translate Ghidra pseudocode into candidate C, keep exact matches only.

For each boundary-reviewed function of a module that has Ghidra pseudocode and no source yet, this
writes a few candidate translations (type and declaration variants) under try/gen-<tag>/, runs
lbpmatch.py over all of them, retries compile failures that are only implicit pointer/integer
conversions with the cast the compiler names, and copies the first EXACT candidate of each function
into src/<dir>/. Nothing that is not an exact, unmasked, linked-at-the-retail-address match is kept.

Accepted sources say in their header comment that they are mechanical translations awaiting human
review; a translation is evidence of nothing beyond its exact match.

Usage: lbpgen.py --tag T [--module EBOOT.BIN] [--min-size 4] [--max-size 64] [--min-start 0x88000]
                 [--limit N] [--dry-run]
"""
from __future__ import annotations

import argparse
import json
import re
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M
import lbpproject as P

SRC_DIRS = P.src_dirs()  # {module: src/ subdirectory}, from data/modules.json
FORBIDDEN = re.compile(r"\b(?:in_\w+|unaff_\w+|extraout_\w+|CONCAT\d+|SUB\d+|ZEXT\d+|SEXT\d+|halt_\w+|trap\w*|SYNC|"
                       r"PTR_\w+|CARRY\d|SCARRY\d|SBORROW\d|POPCOUNT|LZCOUNT|func_0x\w+|\w+\._\d+_\d+_)\b"
                       r"|Subroutine does not return|Could not recover jumptable|Treating indirect jump|"
                       r"Bad instruction|overlaps instruction|Removing unreachable block|"
                       r"Control flow encountered bad instruction")
BASE_TYPES = """typedef unsigned char undefined;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulonglong;
typedef long long longlong;
typedef unsigned char uchar;
typedef unsigned char bool;
typedef unsigned long long undefined8;
typedef int code();
#define true 1
#define false 0
#define ABS(x) ((x) < 0 ? -(x) : (x))
"""
SIGNEDNESS = {
    "unsigned": "typedef unsigned char undefined1;\ntypedef unsigned short undefined2;\ntypedef unsigned int undefined4;\n",
    "signed": "typedef signed char undefined1;\ntypedef short undefined2;\ntypedef int undefined4;\n",
    "float4": "typedef unsigned char undefined1;\ntypedef unsigned short undefined2;\ntypedef float undefined4;\n",
}
GLOBAL = re.compile(r"(&?)\b_?DAT_([0-9a-f]{8})\b")
STRING = re.compile(r"\bs_\w*?_([0-9a-f]{8})\b")
CALL = re.compile(r"\b((?:thunk_)?FUN_[0-9a-f]{8})\s*\(")
FLOAT_LIT = re.compile(r"(?<![\w.])(\d+\.\d+(?:[eE][-+]?\d+)?)(?![\w.])")
CONVERSION = re.compile(r"#\s+(\d+):[^\n]*\n#\s+Error:[^\n]*\n#\s+illegal implicit conversion from\s+'([^']+)'\s*"
                        r"(?:\n#\s*)?to\s*(?:\n#\s*)?'([^']+)'")
ASSIGN = re.compile(r"^(\s*)([^=;]*?[^=!<>+\-*/%&|^])=(?!=)\s*(.+);\s*$")
ADDR_CAST = re.compile(r"(\(\s*[\w ]+?\s*\*+\s*\)\s*)0x([0-9a-f]{5,8})\b")
ADDR_BARE = re.compile(r"(?<![\w.&])0x([0-9a-f]{5,8})\b")
MASK_SHIFT = re.compile(r"\((\w+) & 0x([0-9a-f]+)\) >> (0x[0-9a-f]+|\d+)")


def mask_shift(m: re.Match) -> str:
    """(x & 0xffff) >> 0xc  ->  (x >> 0xc) & 0xf: MWCC emits `ext` only for the shift-then-mask form of a
    bit-field read (0x14d788)."""
    mask, shift = int(m.group(2), 16), int(m.group(3), 0)
    if mask & (mask + 1) or mask >> shift == 0:
        return m.group(0)
    return f"({m.group(1)} >> {m.group(3)}) & 0x{mask >> shift:x}"


NULL_ADJ = re.compile(r"\b(\w+) (!=|==) \([\w ]+\*\)0x(ffff[0-9a-f]{4})\b")
ADDR_SUM = re.compile(r"\(([^()]+?) \+ 0x([0-9a-f]{5,8})\)")
ADDR_RANGE = (0x10000, 0)  # [lowest, module memory end); main() sets the end for --module
BARE_MIN = 0x88000  # bare numbers below the game code are left alone
RETURN = re.compile(r"^(\s*)return\s+(.+);\s*$")


def signature(text: str) -> str | None:
    head = text.split("\n{", 1)[0]
    head = re.sub(r"/\*.*?\*/", "", head, flags=re.S)
    lines = [ln.strip() for ln in head.splitlines() if ln.strip()]
    return " ".join(lines) if lines else None


def split_signature(sig: str):
    m = re.fullmatch(r"(.*?)\b([A-Za-z_]\w*)\s*\((.*)\)", sig)
    if not m:
        return None
    return m.group(1).strip(), m.group(2), m.group(3).strip()


def load_pseudocode(campaign: Path, module_dir: str, base: Path | None = None):
    base = base or campaign / "analysis" / "candidate-modules" / module_dir / "pseudocode"
    rows, sizes = {}, {}
    for line in (base / "functions.jsonl").read_text(encoding="utf-8").splitlines():
        row = json.loads(line)
        if row.get("pseudocode_status") == "decompiled" and row.get("pseudocode_file"):
            rows[int(row["address"], 16)] = (row["name"], base / row["pseudocode_file"])
            sizes[int(row["address"], 16)] = row.get("body_bytes")
    return rows, sizes


def existing_starts() -> set[tuple[str, int]]:
    out = set()
    for root in ("src", "wip"):
        for p in (M.ROOT / root).rglob("*"):
            if p.suffix in (".c", ".cpp"):
                h = M.parse_header(p)
                out.add((h["module"], h["start"]))
    return out


def global_type(body: str, addr: str, ret_type: str, int_globals: bool = False) -> str:
    """A simple context guess for the access type of a global used by value."""
    name = rf"_?DAT_{addr}"
    if not int_globals and re.search(rf"\(\s*{name}\s*\+", body):
        # a base for dereferenced offsets: a struct pointer. Integer addition compiles differently once the
        # offset passes 0x7fff (addiu 0x7fff + rest, unaligned swl/swr) where retail does lui/addu (0x22c15c).
        return "undefined *"
    if re.search(rf"{name}\s*[!=]=\s*'|'\s*[!=]=\s*{name}\b", body):
        return "undefined"  # compared with a character literal: a byte (lbu, 0x154dc8)
    ptr = re.search(rf"\b{name}\s*(?:[!=]=|=)\s*\((\w+(?: \w+)?\s*\*+)\)", body)
    if ptr:
        return ptr.group(1)  # compared with / assigned a typed pointer: a pointer global (0xc47a0)
    store = re.search(rf"\*\((\w+(?: \w+)?) \*\)\([^;=]*\)\s*=\s*{name}\s*;", body)
    if store and store.group(1) in ("undefined1", "undefined2", "undefined4", "byte", "char", "uchar", "short",
                                    "ushort", "int", "uint", "float", "bool"):
        return store.group(1)  # stored straight into a typed field: same width (0xda0f8)
    if re.search(rf"return\s+{name}\s*;", body) and ret_type not in ("void", ""):
        return ret_type.replace("*", "").strip() if "*" not in ret_type else "undefined4"
    m = re.search(rf"\b(\w+)\s*=\s*{name}\s*;", body)
    if m:
        decl = re.search(rf"\b(\w+)\s+\*?\s*{re.escape(m.group(1))}\s*;", body)
        if decl and decl.group(1) not in ("return",):
            return decl.group(1)
    if re.search(rf"\bf\w*\s*=\s*{name}\b|{name}\s*[*/]", body):
        return "float"
    return "undefined4"


def call_args(text: str, open_paren: int):
    """Top-level argument strings of the call whose '(' is at open_paren, and the index of its ')'."""
    depth, args, cur = 0, [], open_paren + 1
    for i in range(open_paren, len(text)):
        c = text[i]
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
            if depth == 0:
                tail = text[cur:i].strip()
                if tail or args:
                    args.append(tail)
                return args, i
        elif c == "," and depth == 1:
            args.append(text[cur:i].strip())
            cur = i + 1
    return None, None


def param_types(params: str):
    params = params.strip()
    if params in ("", "void"):
        return []
    if "..." in params:
        return None
    out = []
    for p in params.split(","):
        m = re.fullmatch(r"\s*(.*?[\s*])\s*[A-Za-z_]\w*\s*", p)
        if not m:
            return None
        out.append(" ".join(m.group(1).split()))
    return out


def cast_calls(body: str, callee: str, types: list[str]) -> str | None:
    """Prototype-typed call sites: each argument cast explicitly to its parameter type, so pointer/integer
    differences (which do not change code) never stop the compile and narrow types still convert."""
    sites = [m.end() - 1 for m in re.finditer(rf"\b{callee}\s*\(", body)]
    for open_paren in reversed(sites):
        args, close = call_args(body, open_paren)
        if args is None or len(args) != len(types):
            return None
        new = ",".join(f"({t})({a})" for t, a in zip(types, args))
        body = body[:open_paren + 1] + new + body[close:]
    return body


def fill_dropped_args(body: str, sig: str, protos: dict[str, str], name: str) -> str:
    """Ghidra drops trailing arguments of calls whose callee it typed with fewer parameters at the time the
    caller was decompiled; the commonest case passes the caller's own argument registers through (a base
    constructor called with `this`, 0x1140d0/0xe985c). Fill each short call from the callee's prototype with
    the caller's same-position parameters."""
    caller_params = re.findall(r"\b(param_\d+)\b", split_signature(sig)[2]) if split_signature(sig) else []
    for callee in sorted(set(CALL.findall(body)) - {name}):
        proto = protos.get(callee)
        sp = split_signature(proto) if proto else None
        types = param_types(sp[2]) if sp else None
        if not types:
            continue
        sites = [m.end() - 1 for m in re.finditer(rf"\b{callee}\s*\(", body)]
        for open_paren in reversed(sites):
            args, close = call_args(body, open_paren)
            if args is None or len(args) >= len(types) or len(types) > len(caller_params):
                continue
            filled = args + [caller_params[i] for i in range(len(args), len(types))]
            body = body[:open_paren + 1] + ",".join(filled) + body[close:]
    return body


RETAIL = {"module": None, "ends": {}}  # set by main(): module name and function ends, for retail_args


def _sext(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def retail_call_args(start: int) -> dict[int, list[list[str | None]]] | None:
    """Per callee, per retail jal site (in address order): the argument expressions retail puts in a0..a3,
    as far as they can be read from the instructions before the jal (and its delay slot). Unknown values
    are None. Only values with an unambiguous source form are named: constants, module addresses,
    the function's own parameters (directly or through the prologue's `move sN, aK`), and word loads
    from them."""
    module, end = RETAIL["module"], RETAIL["ends"].get(start)
    if module is None or end is None:
        return None
    data, seg = M.module_bytes(RETAIL.setdefault("cache", {}), module)
    off = seg["file_offset"] + start - seg["start"]
    words = list(struct.unpack(f"<{(end - start) // 4}I", data[off:off + end - start]))
    op = lambda w: w >> 26  # noqa: E731
    rs = lambda w: (w >> 21) & 31  # noqa: E731
    rt = lambda w: (w >> 16) & 31  # noqa: E731
    rd = lambda w: (w >> 11) & 31  # noqa: E731

    def is_move(w):
        return op(w) == 0 and (w & 0x3F) in (0x21, 0x25) and rt(w) == 0

    def is_flow(w):
        return (op(w) in (1, 2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17) or
                (op(w) == 0 and (w & 0x3F) in (8, 9)) or (op(w) == 0x11 and rs(w) == 8))

    def is_call_or_jump(w):
        return op(w) in (2, 3) or (op(w) == 0 and (w & 0x3F) in (8, 9))

    def is_likely(w):
        return op(w) in (0x14, 0x15, 0x16, 0x17) or (op(w) == 1 and rt(w) in (2, 3, 0x12, 0x13))

    targets = set()
    for k, w in enumerate(words):
        if is_flow(w) and not is_call_or_jump(w):
            targets.add(k + 1 + _sext(w & 0xFFFF))
    saved = {}  # prologue: sN <- param
    for w in words[:16]:
        if is_move(w) and 16 <= rd(w) <= 23 and 4 <= rs(w) <= 7:
            saved.setdefault(rd(w), f"param_{rs(w) - 3}")
    # a-registers still holding the incoming parameter (no write and no call anywhere before, in address order)
    intact_until = {}
    for r in range(4, 8):
        k = next((k for k, w in enumerate(words) if op(w) == 3 or (op(w) == 0 and (w & 0x3F) == 9) or
                  (op(w) == 0 and rd(w) == r and (w & 0x3F) not in (8, 0x18, 0x19, 0x1A, 0x1B))
                  or (op(w) not in (0, 1, 2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17, 0x28, 0x29, 0x2B, 0x39)
                      and rt(w) == r)), len(words))
        intact_until[r] = k
    out: dict[int, list[list[str | None]]] = {}
    for i, w in enumerate(words):
        if op(w) != 3:
            continue
        target = (w & 0x3FFFFFF) << 2
        block = []
        j = i - 1
        while j >= 0 and len(block) < 24 and (j + 1) not in targets:
            x = words[j]
            if is_call_or_jump(x):
                if block:
                    block.pop()  # its delay slot belongs to that call/jump
                break
            if is_flow(x):  # conditional branch: we are on its fall-through path
                if is_likely(x) and block:
                    block.pop()  # a likely branch's delay slot runs only when taken
            else:
                block.append((j, x))
            j -= 1
        seq = list(reversed(block)) + ([(i + 1, words[i + 1])] if i + 1 < len(words) else [])

        def base_expr(reg, at):
            """The parameter a base register holds at instruction `at`, if known."""
            if reg in saved:
                return saved[reg]
            if 4 <= reg <= 7 and at <= intact_until[reg]:
                return f"param_{reg - 3}"
            return None

        regs: dict[int, object] = {}
        for at, x in seq:  # forward evaluation of a0..a3
            o = op(x)
            if o == 0x0F and 4 <= rt(x) <= 7:
                regs[rt(x)] = ("hi", (x & 0xFFFF) << 16)
            elif o == 0x09 and 4 <= rt(x) <= 7:
                if rs(x) == rt(x) and isinstance(regs.get(rt(x)), tuple):
                    regs[rt(x)] = f"&DAT_{regs[rt(x)][1] + _sext(x & 0xFFFF):08x}"
                elif rs(x) == 0:
                    regs[rt(x)] = str(_sext(x & 0xFFFF))
                elif rs(x) != rt(x) and base_expr(rs(x), at):
                    regs[rt(x)] = f"(int){base_expr(rs(x), at)} + {_sext(x & 0xFFFF)}"
                else:
                    regs[rt(x)] = None
            elif o == 0x0D and 4 <= rt(x) <= 7 and rs(x) == 0:
                regs[rt(x)] = hex(x & 0xFFFF)
            elif is_move(x) and 4 <= rd(x) <= 7:
                regs[rd(x)] = "0" if rs(x) == 0 else base_expr(rs(x), at)
            elif o == 0x23 and 4 <= rt(x) <= 7 and rs(x) == rt(x) and isinstance(regs.get(rt(x)), tuple):
                regs[rt(x)] = f"*(int *)&DAT_{regs[rt(x)][1] + _sext(x & 0xFFFF):08x}"
            elif o == 0x23 and 4 <= rt(x) <= 7 and rs(x) not in regs and base_expr(rs(x), at):
                regs[rt(x)] = f"*(int *)((int){base_expr(rs(x), at)} + {_sext(x & 0xFFFF)})"
            elif o == 0 and 4 <= rd(x) <= 7:
                regs[rd(x)] = None
            elif o not in (0, 1, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17, 0x28, 0x29, 0x2B, 0x39, 0x11) and 4 <= rt(x) <= 7:
                regs[rt(x)] = None
        n = max([r - 3 for r in regs if 4 <= r <= 7] + [callee_arity(target)], default=0)
        args = []
        for k in range(n):
            if 4 + k in regs:
                v = regs[4 + k]
                args.append(None if isinstance(v, tuple) else v)
            else:  # not written before the call: passed through from the caller's own parameter
                args.append(f"param_{k + 1}" if i <= intact_until[4 + k] else None)
        out.setdefault(target, []).append(args)
    return out


def callee_arity(target: int) -> int:
    """How many of a0..a3 the function at `target` reads before writing (straight-line scan of its first
    instructions; 0 when unknown)."""
    cache = RETAIL.setdefault("arity", {})
    if target in cache:
        return cache[target]
    module, end = RETAIL["module"], RETAIL["ends"].get(target)
    if module is None or end is None:
        return 0
    data, seg = M.module_bytes(RETAIL.setdefault("cache", {}), module)
    off = seg["file_offset"] + target - seg["start"]
    n = min(end - target, 4 * 48) // 4
    written, used = set(), set()
    for w in struct.unpack(f"<{n}I", data[off:off + 4 * n]):
        o, s, t, d = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
        if o == 0:
            reads, writes = ({t} if (w & 0x3F) in (0, 2, 3) else {s, t}), ({d} if (w & 0x3F) not in (8, 0x18, 0x19, 0x1A, 0x1B) else set())
        elif o in (4, 5, 0x14, 0x15):
            reads, writes = {s, t}, set()
        elif o in (1, 6, 7, 0x16, 0x17, 0x31, 0x39):
            reads, writes = {s}, set()
        elif o in (0x28, 0x29, 0x2B):
            reads, writes = {s, t}, set()
        elif o == 0x11:
            reads, writes = ({t} if s == 4 else set()), ({t} if s == 0 else set())
        elif o in (2, 3):
            break
        else:
            reads, writes = {s}, {t}
        used |= {r for r in reads if 4 <= r <= 7 and r not in written}
        written |= writes
        if o == 0 and (w & 0x3F) == 8:  # jr: end of the straight-line prefix
            break
    cache[target] = max([r - 3 for r in used], default=0)
    return cache[target]


def fill_retail_args(body: str, name: str) -> str:
    """Append the trailing arguments Ghidra dropped, read from retail's argument registers at the matching
    jal (n-th source call of a callee <-> n-th retail jal to it). Only fully known values are appended."""
    m = re.fullmatch(r"FUN_([0-9a-f]{8})", name)
    info = retail_call_args(int(m.group(1), 16)) if m else None
    if not info:
        return body
    for callee in sorted(set(CALL.findall(body)) - {name}):
        cm = re.fullmatch(r"FUN_([0-9a-f]{8})", callee)
        sites_retail = info.get(int(cm.group(1), 16)) if cm else None
        if not sites_retail:
            continue
        sites = [mm.end() - 1 for mm in re.finditer(rf"\b{callee}\s*\(", body)]
        if len(sites) != len(sites_retail):
            continue
        for open_paren, retail in reversed(list(zip(sites, sites_retail))):
            args, close = call_args(body, open_paren)
            if args is None:
                continue
            # EABI: floats travel in $f12.., so only the integer arguments map onto a0..a3
            n_int = sum(1 for a in args if not FLOAT_ARG.match(a))
            if n_int >= len(retail):
                continue
            extra = retail[n_int:]
            if None in extra:
                extra = extra[:extra.index(None)]  # the known prefix (callee arity is an upper bound)
            if not extra:
                continue
            body = body[:open_paren + 1] + ",".join(args + extra) + body[close:]
            EXTENDED[callee] = max(EXTENDED.get(callee, 0), len(extra))
    return body


EXTENDED: dict[str, int] = {}  # callee -> int parameters appended by fill_retail_args (for prototypes)


NEGATE_OP = {"<": ">=", ">=": "<", ">": "<=", "<=": ">", "==": "!=", "!=": "=="}


def negate_condition(cond: str) -> str:
    m = re.fullmatch(r"\s*([^<>=!&|]+?) (<|<=|>|>=|==|!=) ([^<>=!&|]+?)\s*", cond)
    if m and all(s.count("(") == s.count(")") for s in (m.group(1), m.group(3))):
        return f"{m.group(1)} {NEGATE_OP[m.group(2)]} {m.group(3)}"
    return f"!({cond})"


def hoist_common_tails(body: str) -> str:
    """`if (C) { T } else { X; T }` -> `if (!C) { X } T` (and the mirror image): Ghidra copies the statement
    after an if into both branches when it structures consecutive null-checked blocks (0x2934ec)."""
    lines = body.split("\n")
    changed = True
    while changed:
        changed = False
        for i in range(len(lines)):
            m = re.match(r"^(\s+)if \((.*)\) \{\s*$", lines[i])
            if not m:
                continue
            ind, cond = m.group(1), m.group(2)
            mid = next((j for j in range(i + 1, len(lines)) if lines[j] == f"{ind}}}"), None)
            if mid is None or mid + 1 >= len(lines) or lines[mid + 1] != f"{ind}else {{":
                continue
            end = next((j for j in range(mid + 2, len(lines)) if lines[j] == f"{ind}}}"), None)
            if end is None:
                continue
            then_b, else_b = lines[i + 1:mid], lines[mid + 2:end]
            if any("{" in s or "}" in s for s in then_b + else_b):
                continue  # single-level bodies only
            ts, es = [s.strip() for s in then_b], [s.strip() for s in else_b]
            if ts and len(es) > len(ts) and es[-len(ts):] == ts:
                new = [f"{ind}if ({negate_condition(cond)}) {{"] + else_b[:-len(ts)] + [f"{ind}}}"] + [ind + s for s in ts]
            elif es and len(ts) > len(es) and ts[-len(es):] == es:
                new = [f"{ind}if ({cond}) {{"] + then_b[:-len(es)] + [f"{ind}}}"] + [ind + s for s in es]
            else:
                continue
            lines[i:end + 1] = new
            changed = True
            break
    return "\n".join(lines)


VCALL_HEAD = "(**(code **)("


def rewrite_vcalls(body: str) -> tuple[str, list[str]] | None:
    """Ghidra's virtual call `(**(code **)(*E + OFF))(E, args)` -> `((VC_n *)(E))->m(args)` on a per-site class
    whose virtual slot (OFF - 8) / 4 is `m` (MWCC vtables: 8-byte header, then 4-byte slots). Only a real C++
    virtual call loads the target into t9 (`lw t9; jalr t9`) like retail (0x1e2990). Returns the new body and
    the class declarations, or None when a site does not fit the pattern."""
    decls, n = [], 0
    while True:
        i = body.find(VCALL_HEAD)
        if i < 0:
            return body, decls
        # inner `*E + OFF` up to the closing `))`
        j = i + len(VCALL_HEAD)
        depth, k = 0, j
        while k < len(body):
            if body[k] == "(":
                depth += 1
            elif body[k] == ")":
                if depth == 0:
                    break
                depth -= 1
            k += 1
        inner = body[j:k]
        m = re.fullmatch(r"\*(.+) \+ (0x[0-9a-f]+|\d+)", inner)
        if not m or body[k:k + 3] != "))(":
            return None
        this_expr, off = m.group(1), int(m.group(2), 0)
        if off < 8 or (off - 8) % 4:
            return None
        args, close = call_args(body, k + 2)
        if not args or args[0].replace(" ", "") != this_expr.replace(" ", ""):
            return None
        rest = args[1:]
        types = ["float" if FLOAT_ARG.match(a) else "int" for a in rest]
        stmt_start = body.rfind("\n", 0, i) + 1
        whole = re.fullmatch(r"\s*", body[stmt_start:i]) and body[close + 1:close + 2] == ";"
        ret = "void" if whole else "int"
        slot = (off - 8) // 4
        cls = f"VC_{n}"
        if slot == 0 and rest == ["1"] and whole:
            # slot 0 with flag 1 is the virtual deleting destructor: `delete p` (0xe99d4)
            decls.append(f"struct {cls} {{ virtual ~{cls}(); }};")
            body = body[:i] + f"delete (({cls} *)({this_expr}))" + body[close + 1:]
            body = collapse_delete_null_check(body, cls, this_expr)
            n += 1
            continue
        dummies = " ".join(f"virtual void d{s}();" for s in range(slot))
        decls.append(f"struct {cls} {{ {dummies} virtual {ret} m({', '.join(types)}); }};")
        call_args_text = ", ".join(f"({t})({a})" for t, a in zip(types, rest))
        body = body[:i] + f"(({cls} *)({this_expr}))->m({call_args_text})" + body[close + 1:]
        n += 1


def collapse_delete_null_check(body: str, cls: str, this_expr: str) -> str:
    """`if (p == 0) { A } else { delete p; A }` -> `delete p; A`: the delete expression's own null check,
    which Ghidra prints as a separate branch."""
    lines = body.split("\n")
    target = f"delete (({cls} *)({this_expr}));"
    for i in range(len(lines)):
        m = re.match(r"^(\s+)if \((.+) == \([\w ]+\*\)0x0\) \{\s*$", lines[i])
        if not m or m.group(2).replace(" ", "") != this_expr.replace(" ", ""):
            continue
        indent = m.group(1)
        try:
            mid = next(j for j in range(i + 1, len(lines)) if lines[j] == f"{indent}}}")
        except StopIteration:
            continue
        if mid + 2 >= len(lines) or lines[mid + 1].strip() != "else {":
            continue
        try:
            end = next(j for j in range(mid + 2, len(lines)) if lines[j] == f"{indent}}}")
        except StopIteration:
            continue
        then_part = [s.strip() for s in lines[i + 1:mid]]
        else_part = [s.strip() for s in lines[mid + 2:end]]
        if else_part and else_part[0] == target and else_part[1:] == then_part:
            inner = indent + "  "
            lines[i:end + 1] = [inner[:-2] + target] + [indent + s for s in then_part]
            return "\n".join(lines)
    return body


def tested_operand(cond_line: str) -> str | None:
    """X in a line ending `... X != 0) {` (X balanced), else None."""
    m = re.search(r" != (?:0|\([\w ]+\*\)0x0)\)+ \{\s*$", cond_line)
    if not m:
        return None
    end, depth, i = m.start(), 0, m.start() - 1
    while i >= 0:
        ch = cond_line[i]
        if ch == ")":
            depth += 1
        elif ch == "(":
            if depth == 0:
                break
            depth -= 1
        elif depth == 0 and ch in " ,&|!":
            break
        i -= 1
    operand = cond_line[i + 1:end].strip()
    return operand or None


def fill_tested_args(body: str) -> str:
    """`if (p->m != 0) { FUN(); ...` -> `FUN(p->m)`: a free/release call Ghidra typed without parameters,
    passed the member it just tested (0xbc6c0, 0xff548, 0xfcb7c)."""
    lines = body.split("\n")
    for i in range(1, len(lines)):
        m = re.match(r"^(\s+)(FUN_[0-9a-f]{8})\(\);\s*$", lines[i])
        if m:
            operand = tested_operand(lines[i - 1])
            if operand:
                lines[i] = f"{m.group(1)}{m.group(2)}({operand});"
    return "\n".join(lines)


FLOAT_ARG = re.compile(r"^(?:-?\(float\)|-?\d+\.\d*f|\*\(float \*\)|-?\*pf\w+\b|-?f\w*Var\d+$)")


def call_site_types(body: str, sig: str, callee: str, sp) -> list[str] | None:
    """Parameter types from the call sites when some argument is a float: float there, the callee's own
    Ghidra type (or int) elsewhere. None when no call passes a float or the sites disagree in arity."""
    floats = set(re.findall(r"\bfloat\s+(\w+)\s*[;,)]", body + " " + sig))
    sites = [m.end() - 1 for m in re.finditer(rf"\b{callee}\s*\(", body)]
    arity, float_pos = None, set()
    for open_paren in sites:
        args, _ = call_args(body, open_paren)
        if args is None or (arity is not None and len(args) != arity):
            return None
        arity = len(args)
        float_pos |= {i for i, a in enumerate(args) if FLOAT_ARG.match(a) or a in floats}
    if not float_pos:
        return None
    known = param_types(sp[2]) if sp else None
    if known is not None and len(known) != arity:
        known = None
    return ["float" if i in float_pos else
            (known[i] if known and known[i] not in ("float", "double") else "int") for i in range(arity)]


STACK_DECL = re.compile(r"^\s+[\w \*]+?\b\w*(?:local|Stack)_([0-9a-f]+)\s*(?:\[[^\]]*\])?;\s*$")


def order_stack_locals(body: str) -> str:
    """MWCC lays out address-taken locals in declaration order from the frame top down; Ghidra names them by
    their distance from the frame top (local_4, uStack_8, auStack_18, ...) but lists them the other way round.
    Re-sort the stack-variable declarations by that distance, leaving register variables where they are."""
    lines = body.split("\n")
    slots = [i for i, line in enumerate(lines[:80]) if STACK_DECL.match(line)]
    if len(slots) < 2:
        return body
    ordered = sorted((lines[i] for i in slots), key=lambda line: int(STACK_DECL.match(line).group(1), 16))
    for i, line in zip(slots, ordered):
        lines[i] = line
    return "\n".join(lines)


SIGNED_DIV = re.compile(r"^[ \t]*if \((\w+) < 0\) \{\n[ \t]*\1 = \1 \+ (0x[0-9a-f]+|\d+);\n[ \t]*\}\n", re.M)


def fold_signed_division(body: str) -> str:
    """Ghidra prints a signed division by 2^k as `if (x < 0) { x = x + (2^k - 1); } ... x >> k`; MWCC only
    produces retail's bgez/sra/addiu/sra sequence from `x / 2^k` (0xee44c). Folded when every later use of x,
    up to its next assignment, is that shift."""
    for m in reversed(list(SIGNED_DIV.finditer(body))):
        var, add = m.group(1), int(m.group(2), 0)
        k = (add + 1).bit_length() - 1
        if add < 1 or (1 << k) != add + 1:
            continue
        rest = body[m.end():]
        stop = re.search(rf"^[ \t]*{var}\s*=(?!=)", rest, re.M)
        scope = rest[:stop.start()] if stop else rest
        uses = list(re.finditer(rf"\b{var}\b", scope))
        shift = re.compile(rf"{var}\s*>>\s*(?:0x)?{k:x}\b" if k >= 10 else rf"{var}\s*>>\s*(?:0x)?{k}\b")
        if not uses or any(not shift.match(scope, u.start()) for u in uses):
            continue
        scope = shift.sub(f"{var} / {1 << k}", scope)
        body = body[:m.start()] + scope + (rest[stop.start():] if stop else "")
    return body


DIV_TRAP_GUARD = re.compile(r"^[ \t]*if \([^\n{}]*== 0\) \{\n[ \t]*trap\(7\);\n[ \t]*\}\n", re.M)
DIV_TRAP_BARE = re.compile(r"^[ \t]*trap\(7\);\n", re.M)


def drop_division_traps(text: str) -> str:
    """GCC guards every integer division by a variable with `bnez divisor; div; break 7` (the MIPS divide-by-zero
    trap); Ghidra prints that guard as `if (d == 0) { trap(7); }` in front of the `/` or `%`. The guard is the
    compiler's, not the source's (0x0894a7bc is EXACT from `x % s->n`), so it is removed; any other trap code stays
    untranslatable."""
    text = DIV_TRAP_GUARD.sub("", text)
    return DIV_TRAP_BARE.sub("", text)


def translate(name: str, text: str, protos: dict[str, str], variant: dict) -> str | None:
    text = drop_division_traps(text)
    if FORBIDDEN.search(text):
        return None
    sig = signature(text)
    if not sig:
        return None
    parts = split_signature(sig)
    if not parts or parts[1] != name:
        return None
    ret_type = parts[0]
    body = text.split("\n{", 1)[1]
    body = "{" + body
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    body = FLOAT_LIT.sub(lambda m: m.group(1) + "f", body)
    body = fold_signed_division(body)
    body = MASK_SHIFT.sub(mask_shift, body)
    # `p != (T *)0xfffffc90` is a sub-object null test, `(char *)p + 0x370 != 0` (0x126c2c): Ghidra folds the
    # member offset into the constant, which compiles to a compare with -880 instead of addiu + beqz
    body = NULL_ADJ.sub(lambda m: f"(char *){m.group(1)} + 0x{0x100000000 - int(m.group(3), 16):x} {m.group(2)} "
                                  "(char *)0", body)
    if variant.get("uchar"):
        plain_char = r"(?<!unsigned )(?<!signed )\bchar\b"
        if not re.search(plain_char, body + sig):
            return None
        body = re.sub(plain_char, "uchar", body)
        sig = re.sub(plain_char, "uchar", sig)
    if variant.get("varargs"):
        # An empty variadic stub: retail spills all eight argument registers into a 32-byte frame,
        # which only a C++ `(...)` signature produces (verified on 0x8822c).
        if ret_type != "void" or re.sub(r"\s", "", body) != "{return;}":
            return None
        return f'extern "C" void {name}(...)\n{{\n}}\n'
    # Module addresses Ghidra printed as numbers: the module is relocatable, so retail loads them as symbol
    # addresses (lui/addiu with %hi/%lo), never as lui/ori literals (0x1b5594). Pointer casts always; bare
    # constants only in the "bare" variant, since some are genuine numbers.
    lo, hi = ADDR_RANGE
    body = ADDR_CAST.sub(lambda m: (m.group(1) + f"&DAT_{int(m.group(2), 16):08x}"
                                    if lo <= int(m.group(2), 16) < hi else m.group(0)), body)
    if variant.get("addr") == "bare":
        # `(index + 0x338b58)` is an array base plus an index: retail adds them base first (0xee44c)
        body = ADDR_SUM.sub(lambda m: (f"((int)&DAT_{int(m.group(2), 16):08x} + {m.group(1)})"
                                       if max(lo, BARE_MIN) <= int(m.group(2), 16) < hi else m.group(0)), body)
        body = ADDR_BARE.sub(lambda m: (f"(int)&DAT_{int(m.group(1), 16):08x}"
                                        if max(lo, BARE_MIN) <= int(m.group(1), 16) < hi else m.group(0)), body)
    EXTENDED.clear()
    if variant.get("retail_args"):
        body = fill_retail_args(body, name)
    if variant.get("fill_tested"):
        body = fill_tested_args(body)
    if variant.get("fill_args"):
        body = fill_dropped_args(body, sig, protos, name)
    decls = []
    # globals
    seen = {}
    for amp, addr in GLOBAL.findall(body):
        seen.setdefault(addr, set()).add(bool(amp))
    for addr, uses in sorted(seen.items()):
        by_value = re.search(rf"(?<!&)\b_?DAT_{addr}\b", body) is not None
        if not by_value:
            t = "undefined"
        elif variant["globals"] == "context":
            t = global_type(body, addr, ret_type, bool(variant.get("int_globals")))
        else:
            t = variant["globals"]
        # an odd address holds a byte and a 2-mod-4 address at most a halfword (0xdb7f4)
        if int(addr, 16) % 2 and t not in ("undefined", "undefined1", "byte", "char", "uchar", "bool"):
            t = "undefined"
        elif int(addr, 16) % 4 == 2 and t in ("undefined4", "int", "uint", "float", "undefined *"):
            t = "undefined2"
        decls.append(f"extern {t} DAT_{addr};")
    body = re.sub(r"\b_DAT_([0-9a-f]{8})\b", r"DAT_\1", body)
    # string labels: the suffix is the label's own address
    strings = sorted(set(STRING.findall(body)))
    body = STRING.sub(lambda m: "DAT_" + m.group(1), body)
    for addr in strings:
        if addr not in seen:
            decls.append(f"extern char DAT_{addr}[];")
    # callees
    declared = set()
    for callee in sorted(set(CALL.findall(body))):
        if callee == name:
            continue
        declared.add(callee)
        proto = protos.get(callee)
        sp = split_signature(proto) if proto else None
        if sp and sp[0].strip() == "void" and value_used(body, callee):
            # Ghidra typed the callee void (stub or thunk body), but this caller uses its result
            sp = ("int", sp[1], sp[2])
        if sp and callee in EXTENDED:
            # retail passes more integer arguments than Ghidra's prototype lists
            params = "" if sp[2].strip() in ("", "void") else sp[2] + ", "
            sp = (sp[0], sp[1], params + ", ".join(f"int arg_extra{k}" for k in range(EXTENDED[callee])))
        if sp and variant["calls"] == "cast-prototype":
            types = param_types(sp[2])
            casted = cast_calls(body, callee, types) if types is not None else None
            if casted is not None:
                body = casted
                decls.append(f"extern {sp[0]} {callee}({sp[2]});")
                continue
        if sp and (variant["calls"] == "prototype" or
                   (variant["calls"] in ("float-prototype", "cast-prototype")
                    and re.search(r"\b(?:float|double)\b", sp[2]))):
            # an unprototyped call would promote float arguments to double
            decls.append(f"extern {sp[0]} {callee}({sp[2]});")
        elif (types := call_site_types(body, sig, callee, sp)) is not None:
            # Ghidra's callee prototype lacks the float parameters this caller passes; without one, C
            # promotes them to double through __extendsfdf2 (0x97110). Prototype from the call sites.
            body = cast_calls(body, callee, types)
            decls.append(f"extern {sp[0] if sp else 'int'} {callee}({', '.join(types)});")
        else:
            rt = sp[0] if sp else "int"
            decls.append(f"extern {rt} {callee}();")
    # functions referenced as values (callbacks, function-pointer tables) rather than called
    for ref in sorted(set(re.findall(r"\b(FUN_[0-9a-f]{8})\b(?!\s*\()", body)) - declared - {name}):
        decls.append(f"extern void {ref}();")
    if re.search(r"\bthunk_FUN_", body + sig):
        return None  # thunk names do not carry the thunk's own address
    if variant.get("ptr_bases"):
        # Ghidra types object pointers as int; struct member access compiles like char-pointer arithmetic
        # (operand order, strength reduction), not integer arithmetic (0x2535f0).
        names = set(re.findall(r"\(\s*(param_\d+|[a-z]Var\d+)\s*\+", body))
        for n in sorted(names):
            pattern = rf"\b(?:int|uint|undefined4)\s+{n}\b"
            sig = re.sub(pattern, f"char *{n}", sig)
            body = re.sub(rf"^(\s+)(?:int|uint|undefined4)\s+{n};", rf"\1char *{n};", body, flags=re.M)
    if not variant.get("ghidra_stack_order"):
        body = order_stack_locals(body)
    base = BASE_TYPES
    if variant.get("bool_int"):
        # Ghidra types 0/1 results as bool; retail often returns them as int, with no 0xff mask (0x127870)
        if not re.search(r"\bbool\b", sig + body):
            return None
        base = base.replace("typedef unsigned char bool;", "typedef int bool;")
    if variant.get("vcall"):
        rewritten = rewrite_vcalls(body)
        if rewritten is None or not rewritten[1]:
            return None
        body, classes = rewritten
        if variant.get("hoist"):
            body = hoist_common_tails(body)
        # C++: builtin bool/true/false; `()` declares no parameters, so unknown prototypes become `(...)`
        base = (base.replace("typedef unsigned char bool;\n", "").replace("#define true 1\n", "")
                .replace("#define false 0\n", "").replace("typedef int code();", "typedef int code(...);"))
        decls = [re.sub(r"^(extern .*\bFUN_[0-9a-f]{8})\(\);$", r"\1(...);", d) for d in decls]
        out = ([base + SIGNEDNESS[variant["sign"]], 'extern "C" {'] + decls + ["}"] + classes +
               ["", 'extern "C" ' + sig, body.rstrip() + "\n"])
        return "\n".join(out)
    if variant.get("hoist"):
        body = hoist_common_tails(body)
    out = [base + SIGNEDNESS[variant["sign"]]] + decls + ["", sig, body.rstrip() + "\n"]
    return "\n".join(out)


# Variant 0 runs for every target; the others only for targets variant 0 did not match.
# (probe2: v0 153, v0 cast retry 6, float4 2, signed 1, full prototypes 0, float globals 0 of 162.)
VARIANTS = [
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype"},
    {"sign": "signed", "globals": "context", "calls": "float-prototype"},
    {"sign": "float4", "globals": "context", "calls": "float-prototype"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "uchar": True},
    {"sign": "unsigned", "globals": "context", "calls": "cast-prototype"},
    {"sign": "signed", "globals": "context", "calls": "cast-prototype"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "varargs": True, "lang": "c++"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "ghidra_stack_order": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "addr": "bare"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "int_globals": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "ptr_bases": True},
    {"sign": "unsigned", "globals": "context", "calls": "cast-prototype", "ptr_bases": True, "addr": "bare"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "fill_args": True},
    {"sign": "unsigned", "globals": "context", "calls": "cast-prototype", "fill_args": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "bool_int": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "fill_tested": True, "fill_args": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "retail_args": True, "addr": "bare"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "vcall": True, "lang": "c++"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "vcall": True, "lang": "c++",
     "retail_args": True, "addr": "bare"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "retail_args": True, "ptr_bases": True,
     "addr": "bare"},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "vcall": True, "lang": "c++",
     "hoist": True},
    {"sign": "unsigned", "globals": "context", "calls": "float-prototype", "hoist": True},
]


def run_batch(gen: Path, files: list, parallel: int, label: str) -> dict:
    cmd = [sys.executable, str(Path(M.__file__)), "--quiet", "--parallel", str(parallel), "--json"]
    status = {}
    chunk = 200
    for k in range(0, len(files), chunk):
        part = gen / f"summary-{label}-{k // chunk}.json"
        subprocess.run(cmd + [str(part)] + [str(p) for _, _, p in files[k:k + chunk]], stdout=subprocess.DEVNULL)
        for row in json.loads(part.read_text(encoding="utf-8")):
            status[str((M.ROOT / row["source"]).resolve())] = row["status"]
    return status


def build_dir(p: Path, module: str) -> Path:
    return M.ROOT / "build" / module / M.build_key(p)


def value_used(body: str, callee: str) -> bool:
    """True when some call of `callee` is not a whole statement (its result feeds an expression)."""
    for line in body.split("\n"):
        stripped = line.strip()
        if f"{callee}(" in stripped and not stripped.startswith(f"{callee}("):
            return True
    return False


def cast_retry(p: Path, module: str) -> str | None:
    """Rewrite each statement the compiler refused for an implicit pointer/integer conversion with an
    explicit cast to the type the compiler named. Only whole-line assignments and returns are touched."""
    log = build_dir(p, module) / "compile.log"
    if not log.exists():
        return None
    text = log.read_text(errors="replace")
    errors = CONVERSION.findall(text)
    if not errors or text.count("Error:") != len(errors):
        return None
    lines = p.read_text(encoding="utf-8").split("\n")
    for line_no, _, target in errors:
        i = int(line_no) - 1
        if not 0 <= i < len(lines):
            return None
        m = ASSIGN.match(lines[i])
        if m:
            lines[i] = f"{m.group(1)}{m.group(2).rstrip()} = ({target})({m.group(3)});"
            continue
        m = RETURN.match(lines[i])
        if m:
            lines[i] = f"{m.group(1)}return ({target})({m.group(2)});"
            continue
        return None
    return "\n".join(lines)


R_MIPS_HI16, R_MIPS_LO16 = 5, 6


def infer_rodata(p: Path, module: str) -> int | None:
    """The retail address of a candidate's .rodata, read back from retail at the object's %hi/%lo pairs
    against .rodata symbols (jump tables, constants). Every pair must agree. Placement is only a guess:
    lbpmatch.py then requires the placed bytes to equal retail."""
    obj_path = build_dir(p, module) / "obj.o"
    if not obj_path.exists():
        return None
    obj = obj_path.read_bytes()
    sections = M.elf_sections(obj)
    names = [n for n, _ in sections]
    if ".rodata" not in names or ".text" not in names:
        return None
    rodata_idx, text_idx = names.index(".rodata"), names.index(".text")
    symbols = list(M.elf_symbols(obj))
    text = sections[text_idx][1]
    rel = next((s for n, s in sections if s[1] == 9 and s[7] == text_idx), None)  # SHT_REL for .text
    if rel is None:
        return None
    relocs = [struct.unpack_from("<II", obj, rel[4] + i * 8) for i in range(rel[5] // 8)]
    h = M.parse_header(p)
    data, seg = M.module_bytes({}, module)
    base = seg["file_offset"] + h["start"] - seg["start"]

    def word(blob, off):
        return struct.unpack_from("<I", blob, off)[0]

    votes = set()
    pending = {}
    for off, info in relocs:
        sym, rtype = info >> 8, info & 0xFF
        name, value, size, sinfo, shndx = symbols[sym]
        if shndx != rodata_idx or rtype not in (R_MIPS_HI16, R_MIPS_LO16):
            continue
        if off + 4 > h["end"] - h["start"]:
            return None
        ours, theirs = word(obj, text[4] + off), word(data, base + off)
        if rtype == R_MIPS_HI16:
            if theirs >> 26 != 0x0F:  # retail must have a lui here
                return None
            pending[sym] = (ours & 0xFFFF, theirs & 0xFFFF)
        elif sym in pending:
            (ohi, thi), olo, tlo = pending.pop(sym), ours & 0xFFFF, theirs & 0xFFFF
            sext = lambda v: v - 0x10000 if v & 0x8000 else v  # noqa: E731
            addend = (ohi << 16) + sext(olo)
            votes.add(((thi << 16) + sext(tlo)) - (value + addend))
    return votes.pop() if len(votes) == 1 else None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--tag", required=True)
    ap.add_argument("--module", default=P.default_module())
    ap.add_argument("--min-size", type=int, default=4)
    ap.add_argument("--max-size", type=int, default=64)
    ap.add_argument("--min-start", type=lambda s: int(s, 0), default=0x88000)
    ap.add_argument("--max-start", type=lambda s: int(s, 0), default=0xFFFFFFFF)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--parallel", type=int, default=8)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--pseudo", type=Path, help="alternative pseudocode export (functions.jsonl + pseudocode/)")
    ap.add_argument("--variants", default="", help="comma-separated pass-2 variant numbers (default: all)")
    ap.add_argument("--only", default="", help="comma-separated function addresses, or @file with one per line")
    ap.add_argument("--cc", default=M.PRIMARY_PROFILE, help="compiler profile for the headers (default: the primary profile, M.PRIMARY_PROFILE)")
    args = ap.parse_args()
    only_text = Path(args.only[1:]).read_text(encoding="utf-8") if args.only.startswith("@") else args.only
    only = {int(a, 16) for a in re.split(r"[\s,]+", only_text) if a.strip()}
    pass2_variants = ([int(v) for v in args.variants.split(",") if v] if args.variants
                      else list(range(1, len(VARIANTS))))

    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    mod = next(m for m in ledger["modules"] if m["name"] == args.module)
    global ADDR_RANGE, BARE_MIN
    ADDR_RANGE = (0x10000, max(s["memory_end"] for s in mod["load_segments"]))
    RETAIL["module"] = args.module
    RETAIL["ends"] = {f["start"]: f["end"] for f in mod["functions"]}
    BARE_MIN = max(0x10000, args.min_start)
    module_dir = args.module.replace(".", "_")
    pseudo, ghidra_sizes = load_pseudocode(M.CAMPAIGN, module_dir, args.pseudo)
    protos = {}
    for addr, (name, path) in pseudo.items():
        sig = signature(path.read_text(encoding="utf-8"))
        if sig:
            protos[name] = sig
    have = existing_starts()
    targets = []
    extent_differs = 0
    for f in mod["functions"]:
        if f.get("boundary_status") not in P.USABLE_BOUNDARY:
            continue
        size = f["end"] - f["start"]
        if not (args.min_size <= size <= args.max_size and args.min_start <= f["start"] <= args.max_start):
            continue
        if (args.module, f["start"]) in have or f["start"] not in pseudo:
            continue
        if only and f["start"] not in only:
            continue
        if ghidra_sizes.get(f["start"]) != size:
            # Ghidra decompiled a different extent (merged neighbours, padding): its pseudocode
            # describes other bytes, so a translation cannot match.
            extent_differs += 1
            continue
        targets.append(f)
    print(f"skipped_extent_differs={extent_differs}")
    if args.limit:
        targets = targets[:args.limit]

    gen = M.ROOT / "try" / f"gen-{args.tag}"
    if gen.exists():
        raise SystemExit(f"{gen} exists; use a new tag")
    if args.dry_run:
        n = sum(translate(pseudo[f["start"]][0], pseudo[f["start"]][1].read_text(encoding="utf-8"), protos,
                          VARIANTS[0]) is not None for f in targets)
        print(f"targets={len(targets)} untranslatable={len(targets) - n} first-pass candidates={n} (dry run)")
        return 0
    def write_variant(f, i):
        name, path = pseudo[f["start"]]
        src = translate(name, path.read_text(encoding="utf-8"), protos, VARIANTS[i])
        if src is None:
            return None
        lang = VARIANTS[i].get("lang", "c99")
        cc = f" cc={args.cc}" if args.cc else ""
        header = (f"/* LBPMATCH module={args.module} start=0x{f['start']:08x} end=0x{f['end']:08x} "
                  f"symbol={name} lang={lang}{cc} */\n")
        p = gen / f"v{i}" / (name + (".cpp" if lang == "c++" else ".c"))
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(header + src, encoding="utf-8", newline="\n")
        return p

    def cast_retries(batch):
        out = []
        for f, v, p in batch:
            if f["start"] in exact_starts:
                continue
            if status.get(str(p.resolve())) == "UNPLACED_SECTION" and "rodata=" not in p.read_text(encoding="utf-8")[:300]:
                addr = infer_rodata(p, args.module)
                if addr is not None:
                    head, rest = p.read_text(encoding="utf-8").split("\n", 1)
                    q = gen / f"{v}r" / p.name
                    q.parent.mkdir(parents=True, exist_ok=True)
                    q.write_text(head.replace(" */", f" rodata=0x{addr:08x} */", 1) + "\n" + rest,
                                 encoding="utf-8", newline="\n")
                    out.append((f, f"{v}r", q))
                continue
            if status.get(str(p.resolve())) != "COMPILE_FAILED":
                continue
            fixed = cast_retry(p, args.module)
            if fixed:
                q = gen / f"{v}c" / p.name
                q.parent.mkdir(parents=True, exist_ok=True)
                q.write_text(fixed, encoding="utf-8", newline="\n")
                out.append((f, f"{v}c", q))
        return out

    files = []
    first = []
    for f in targets:
        p = write_variant(f, 0)
        if p is not None:
            first.append((f, "v0", p))
    skipped = len(targets) - len(first)
    print(f"targets={len(targets)} untranslatable={skipped} first-pass candidates={len(first)}")
    if args.dry_run or not first:
        return 0
    status = run_batch(gen, first, args.parallel, "pass1")
    files += first
    exact_starts = {f["start"] for f, _, p in files if status.get(str(p.resolve())) == "EXACT"}
    # pass 2: the other variants and the cast retries of pass 1, only for unmatched targets
    second = cast_retries(first)
    v0_text = {f["start"]: p.read_text(encoding="utf-8") for f, _, p in first}
    for f, _, _ in first:
        if f["start"] in exact_starts:
            continue
        for i in pass2_variants:
            p = write_variant(f, i)
            if p is not None and p.read_text(encoding="utf-8") != v0_text[f["start"]]:
                second.append((f, f"v{i}", p))
    if second:
        status.update(run_batch(gen, second, args.parallel, "pass2"))
        files += second
        exact_starts |= {f["start"] for f, _, p in second if status.get(str(p.resolve())) == "EXACT"}
    # pass 3: cast retries of the pass 2 variants
    third = cast_retries([row for row in second if not row[1].endswith("c")])
    if third:
        status.update(run_batch(gen, third, args.parallel, "pass3"))
        files += third
    retry = [row for row in files if row[1].endswith("c")]
    accepted = {}
    for f, v, p in files:
        if status.get(str(p.resolve())) == "EXACT" and f["start"] not in accepted:
            accepted[f["start"]] = (f, p)
    dest = M.ROOT / "src" / SRC_DIRS[args.module]
    dest.mkdir(parents=True, exist_ok=True)
    covered = existing_starts()  # another run may have matched some targets meanwhile
    written = 0
    for start, (f, p) in sorted(accepted.items()):
        if (args.module, start) in covered:
            print(f"skip 0x{start:08x}: already covered by another source")
            continue
        text = p.read_text(encoding="utf-8")
        first, rest = text.split("\n", 1)
        note = ("/* Mechanical translation of Ghidra pseudocode (lbpgen.py tag " + args.tag + "); exact match "
                "verified; names and types await human review. */\n")
        out = dest / p.name
        if out.exists():
            print(f"skip {out.name}: exists")
            continue
        out.write_text(first + "\n" + note + rest, encoding="utf-8", newline="\n")
        written += 1
    counts = {}
    for f, v, p in files:
        key = status.get(str(p.resolve()), "NOT_RUN")
        counts[key] = counts.get(key, 0) + 1
    by_variant = {}
    for start, (f, p) in accepted.items():
        by_variant[p.parent.name] = by_variant.get(p.parent.name, 0) + 1
    report = {"tag": args.tag, "module": args.module, "pseudo": str(args.pseudo or "campaign candidate export"),
              "targets": len(targets), "untranslatable": skipped,
              "candidates": len(files), "cast_retries": len(retry), "accepted": len(accepted), "written": written,
              "accepted_bytes": sum(f["end"] - f["start"] for f, _ in accepted.values()),
              "accepted_by_variant": by_variant, "status_counts": counts}
    (gen / "report.json").write_text(json.dumps(report, indent=1) + "\n", encoding="utf-8")
    print(json.dumps(report))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
