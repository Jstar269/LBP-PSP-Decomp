#!/usr/bin/env python3
"""m2c drafts for small unmatched functions: retail bytes -> psp-objdump -> m2c -> plain-C draft with an LBPMATCH header.

    LBP_M2C=<path to m2c.py> python tools/lbpm2c.py --tag m1 [--module EBOOT.BIN] [--count 200] [--min-size 12]
                                                      [--max-size N] [--skip N] [--calls] [--cc PROFILE]
                                                      [--publishable] [--exclude-tried]

m2c (matt-kempster/m2c, GPL-3.0) is not shipped and not installed by this tool: point LBP_M2C at a checkout's m2c.py
(the trial's pin is recorded in toolchain/gcc4/PROVENANCE.json, user_supplied.m2c). Writes try/m2c-<tag>/:
  <start>.s        the m2c input made from the retail words (GNU as syntax, `glabel`, .L<addr> branch labels)
  <start>.m2c.c    m2c's raw output (target mipsel-gcc-c, --valid-syntax --globals used)
  drafts/m2c_<start>.c   the draft: header, the tool's provenance note, m2c's C with its macros and fixed-width type
                   names rewritten to plain C (no M2C_* name is left; a draft m2c marked with M2C_ERROR is not written)
  m2c.json         one row per function: start, size, leaf, Allegrex-only instructions, m2c errors, draft written
A draft is only a candidate: run it through lbpmatch.py, and only an EXACT one may go further (lbppromote.py, as any
source). The provenance note is "Mechanical translation of the disassembly (m2c <commit>, lbpm2c.py tag <tag>)".

Selection: functions of the module's inventory that no source covers and the ledger does not hold, outside
try/snc-functions.txt, without VFPU, madd/msub or div instructions (pools no draft can fix), not an empty `jr ra; nop`
and not a one-jump thunk; leaves (no jal/jalr) first unless --calls, then by size.
Rewrites of the m2c input: a tail call (`j FUN` outside the function, `jr $reg` other than $ra) becomes delay slot,
call, return (m2c reads `jr $reg` as a jump table and drops a `j` tail call's delay-slot instruction); `jal` targets
become FUN_<addr>; a lui whose register has exactly one %lo-style use becomes %hi/%lo of DAT_<addr>.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M

VFPU_OPS = {0x18, 0x19, 0x1b, 0x34, 0x35, 0x36, 0x37, 0x3c, 0x3d, 0x3e, 0x3f, 0x32, 0x3a}
MULDIV_FUNCTS = {0x1c, 0x1d, 0x2e, 0x2f, 0x1a, 0x1b}          # Allegrex madd/maddu/msub/msubu, div/divu
ALLEGREX = {"ext", "ins", "seb", "seh", "wsbh", "wsbw", "clz", "clo", "min", "max", "bitrev", "rotr", "rotrv", "rorv",
            "movz", "movn", "mfic", "mtic", "halt", "cache", "sync"}
REGS = set("zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 t8 t9 s0 s1 s2 s3 s4 s5 s6 s7 k0 k1 gp sp fp s8 ra".split())
BRANCHES = {"b", "beq", "bne", "beqz", "bnez", "bgez", "bgtz", "blez", "bltz", "beql", "bnel", "beqzl", "bnezl",
            "bgezl", "bgtzl", "blezl", "bltzl", "bc1t", "bc1f", "bc1tl", "bc1fl", "bal", "bgezal", "bltzal", "j"}
MEM = re.compile(r"^(-?(?:0x[0-9a-f]+|\d+))\((\$\w+)\)$")
TYPES = {"s8": "signed char", "u8": "unsigned char", "s16": "short", "u16": "unsigned short", "s32": "int",
         "u32": "unsigned int", "s64": "long long", "u64": "unsigned long long", "f32": "float", "f64": "double",
         "M2C_UNK8": "signed char", "M2C_UNK16": "short", "M2C_UNK32": "int", "M2C_UNK64": "long long",
         "M2C_UNK": "int"}
M2C_COMMIT = "c632946b"


def inventory(module: str) -> dict[int, int]:
    inv = {}
    for line in (M.ROOT / "data" / f"functions-{module}.txt").read_text(encoding="utf-8").splitlines():
        p = line.split()
        if len(p) >= 2 and not line.startswith("#"):
            inv[int(p[0], 16)] = int(p[1], 16)
    return inv


def covered(module: str) -> set[int]:
    out = set()
    for line in (M.ROOT / "data" / "accepted-ledger.jsonl").read_text(encoding="utf-8").splitlines():
        r = json.loads(line)
        if r.get("module") == module:
            s = r["start"]
            out.add(int(s, 16) if isinstance(s, str) else s)
    for p in list((M.ROOT / "src").rglob("*.c")) + list((M.ROOT / "src").rglob("*.cpp")):
        try:
            h = M.parse_header(p)
        except Exception:                      # a file without a header is no cover
            continue
        if h.get("module") == module:
            out.add(int(h["start"], 16) if isinstance(h["start"], str) else h["start"])
    return out


def publishable_filter(module: str):
    """A predicate: True when the lbporigin region of a start address is one lbppublish publishes."""
    import lbporigin
    from lbppublish import EXCLUDED_CLASSES
    regions = lbporigin.load_regions()
    return lambda a: lbporigin.region_class(regions, module, a) not in EXCLUDED_CLASSES


def select(module: str, count: int, min_size: int, max_size: int, skip: int, calls: bool,
           publishable: bool = False, exclude: set[int] = frozenset()) -> list[dict]:
    data, seg = M.module_bytes({}, module)
    base, off = seg["start"], seg["file_offset"]
    done = covered(module)
    snc_file = M.ROOT / "try" / "snc-functions.txt"
    snc = {int(x, 16) for x in snc_file.read_text().split()} if snc_file.exists() else set()
    pub = publishable_filter(module) if publishable else (lambda a: True)
    rows = []
    for a, b in inventory(module).items():
        n = b - a
        if a in done or a in snc or a in exclude or n < min_size or (max_size and n > max_size) or not pub(a):
            continue
        raw = data[off + a - base: off + b - base]
        if len(raw) != n:
            continue
        ws = list(struct.unpack(f"<{n // 4}I", raw))
        op = [w >> 26 for w in ws]
        if ws[:2] == [0x03e00008, 0] or op[0] == 2 or any(o in VFPU_OPS for o in op):
            continue
        if any(o == 0 and (w & 0x3f) in MULDIV_FUNCTS for o, w in zip(op, ws)):
            continue
        leaf = not any(o == 3 or (o == 0 and (w & 0x3f) == 9) for o, w in zip(op, ws))
        rows.append(((0 if leaf or calls else 1), n, a, b, leaf, raw))
    rows.sort()
    return [{"start": a, "end": b, "size": n, "leaf": leaf, "raw": raw} for _, n, a, b, leaf, raw in rows[skip:skip + count]]


def _reg(tok: str) -> str:
    return "$" + ("fp" if tok == "s8" else tok) if tok in REGS else tok


def parse_objdump(text: str) -> list[tuple[int, str, list[str]]]:
    out = []
    for line in text.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f]{8} \t(\S+)\s*(.*)$", line)
        if not m:
            continue
        ops = []
        for o in (m.group(3).split(",") if m.group(3) else []):
            o = re.sub(r"\((\w+)\)", lambda q: f"({_reg(q.group(1))})", _reg(o.strip()))
            ops.append(o)
        out.append((int(m.group(1), 16), m.group(2), ops))
    return out


def to_m2c(start: int, end: int, ins: list[tuple[int, str, list[str]]]) -> str:
    """The m2c input for one function (see the module docstring for the rewrites)."""
    targets = {int(i[2][-1], 16) for i in ins if i[1] in BRANCHES and i[2] and i[2][-1].startswith("0x")}
    ops = [list(i[2]) for i in ins]
    for k, (_a, mn, o) in enumerate(ins):
        if mn != "lui":
            continue
        r, hi = o[0], int(o[1], 16)
        uses = []
        for j in range(k + 1, len(ins)):
            mj, oj = ins[j][1], ins[j][2]
            if mj == "addiu" and len(oj) == 3 and oj[1] == r:
                uses.append((j, 2, int(oj[2], 0)))
            else:
                for n, x in enumerate(oj):
                    mm = MEM.match(x)
                    if mm and mm.group(2) == r:
                        uses.append((j, n, int(mm.group(1), 0)))
            if oj and oj[0] == r and mj not in ("sw", "sh", "sb", "swc1"):
                break                                   # the register is redefined
        if len(uses) != 1:
            continue
        j, n, lo = uses[0]
        sym = f"DAT_{((hi << 16) + lo) & 0xffffffff:08x}"
        ops[k][1] = f"%hi({sym})"
        ops[j][n] = f"%lo({sym})" if ins[j][1] == "addiu" else f"%lo({sym})({r})"
    lines = [".set noat", ".set noreorder", ".section .text", f"glabel FUN_{start:08x}"]
    k = 0
    while k < len(ins):
        a, mn, _o = ins[k]
        if a in targets and a != start:
            lines.append(f".L{a:08x}:")
        oo = ops[k]
        if mn in BRANCHES and oo and oo[-1].startswith("0x"):
            t = int(oo[-1], 16)
            oo[-1] = f"FUN_{t:08x}" if (mn == "j" and not start <= t < end) else f".L{t:08x}"
        if mn == "jal" and oo and oo[-1].startswith("0x"):
            oo[-1] = f"FUN_{int(oo[-1], 16):08x}"
        tail = (mn == "j" and oo[-1].startswith("FUN_")) or (mn == "jr" and oo and oo[0] != "$ra")
        if tail and k + 1 < len(ins):
            d = ins[k + 1]
            lines.append(f"/* {d[0]:08X} */ {d[1]} {', '.join(ops[k + 1])}".rstrip())
            lines.append(f"/* {a:08X} */ {'jal ' + oo[-1] if mn == 'j' else 'jalr ' + oo[0]}")
            lines += ["nop", "jr $ra", "nop"]
            k += 2
            continue
        lines.append(f"/* {a:08X} */ {mn} {', '.join(oo)}".rstrip())
        k += 1
    return "\n".join(lines) + "\n"


def _macro_args(text: str, i: int) -> tuple[list[str], int]:
    """Top-level comma-separated arguments of the call whose '(' is at text[i]; (args, index after ')')."""
    depth, cur, args = 0, [], []
    for j in range(i, len(text)):
        c = text[j]
        if c == "(":
            depth += 1
            if depth == 1:
                continue
        elif c == ")":
            depth -= 1
            if depth == 0:
                args.append("".join(cur).strip())
                return args, j + 1
        elif c == "," and depth == 1:
            args.append("".join(cur).strip())
            cur = []
            continue
        cur.append(c)
    raise ValueError("unbalanced macro call")


def plain_c(text: str) -> str:
    """m2c --valid-syntax output without its macros and fixed-width type names."""
    for name, fmt in (("M2C_FIELD", "(*({1})((char *)({0}) + {2}))"), ("M2C_BITWISE", "(({0})({1}))")):
        while True:
            m = re.search(rf"\b{name}\(", text)
            if not m:
                break
            args, end = _macro_args(text, m.end() - 1)
            text = text[:m.start()] + fmt.format(*args) + text[end:]
    for k in sorted(TYPES, key=len, reverse=True):
        text = re.sub(rf"\b{k}\b", TYPES[k], text)
    return re.sub(r"\bNULL\b", "0", text)         # a draft includes no header that defines NULL


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--tag", required=True)
    ap.add_argument("--module", default="EBOOT.BIN")
    ap.add_argument("--count", type=int, default=200)
    ap.add_argument("--min-size", type=int, default=12)
    ap.add_argument("--max-size", type=int, default=0)
    ap.add_argument("--skip", type=int, default=0, help="skip the first N selected functions")
    ap.add_argument("--calls", action="store_true", help="do not put leaves first")
    ap.add_argument("--cc", default="", help="cc= for the drafts (default: none, the primary profile)")
    ap.add_argument("--publishable", action="store_true",
                    help="select only functions in lbporigin regions that lbppublish publishes (not sony-sdk, startup, ...)")
    ap.add_argument("--exclude-tried", action="store_true", help="skip functions already in any try/m2c-*/m2c.json")
    args = ap.parse_args()
    m2c = os.environ.get("LBP_M2C")
    if not m2c or not Path(m2c).is_file():
        sys.exit("set LBP_M2C to the path of m2c.py (not shipped; see the module docstring)")
    out = M.ROOT / "try" / f"m2c-{args.tag}"
    (out / "drafts").mkdir(parents=True, exist_ok=True)
    (out / "bin").mkdir(exist_ok=True)
    tried = set()
    if args.exclude_tried:
        for f in (M.ROOT / "try").glob("m2c-*/m2c.json"):
            tried |= {int(r["start"], 16) for r in json.loads(f.read_text(encoding="utf-8"))}
    sel = select(args.module, args.count, args.min_size, args.max_size, args.skip, args.calls, args.publishable, tried)
    for r in sel:
        (out / "bin" / f"{r['start']:08x}.bin").write_bytes(r["raw"])
    script = (f'cd "{M.to_wsl(out / "bin")}" && for f in *.bin; do a=${{f%.bin}}; '
              f'{M.PSPDEV_BIN}/psp-objdump -D -z -EL -b binary -m mips:allegrex --adjust-vma=0x$a "$f" > "$a.dis"; done')
    subprocess.run(["wsl.exe", "-e", "sh", "-c", script], check=True)
    note = f"/* Mechanical translation of the disassembly (m2c {M2C_COMMIT}, lbpm2c.py tag {args.tag}); names and types await human review. */"
    rows = []
    for r in sel:
        a, e = r["start"], r["end"]
        ins = parse_objdump((out / "bin" / f"{a:08x}.dis").read_text())
        (out / f"{a:08x}.s").write_text(to_m2c(a, e, ins))
        p = subprocess.run([sys.executable, "-E", "-s", m2c, "-t", "mipsel-gcc-c", "--valid-syntax", "--globals", "used",
                            str(out / f"{a:08x}.s")], capture_output=True, text=True, timeout=300)
        (out / f"{a:08x}.m2c.c").write_text(p.stdout + (("\n/* stderr:\n" + p.stderr + "*/\n") if p.stderr else ""))
        errors = sorted(set(re.findall(r"M2C_ERROR\(/\* (.*?) \*/\)", p.stdout)))
        ok = p.returncode == 0 and f"FUN_{a:08x}(" in p.stdout and not errors
        if ok:
            hdr = (f"/* LBPMATCH module={args.module} start=0x{a:08x} end=0x{e:08x} symbol=FUN_{a:08x} lang=c99"
                   f"{' cc=' + args.cc if args.cc else ''} */")
            body = plain_c(p.stdout).strip()
            body = re.sub(r"[ \t]+/\* extern \*/", "", body)
            (out / "drafts" / f"m2c_{a:08x}.c").write_text(f"{hdr}\n{note}\n\n{body}\n")
        rows.append({"start": f"0x{a:08x}", "end": f"0x{e:08x}", "size": r["size"], "leaf": r["leaf"],
                     "allegrex": sorted({mn for _, mn, _ in ins} & ALLEGREX), "m2c_rc": p.returncode,
                     "m2c_errors": errors, "draft": ok})
    (out / "m2c.json").write_text(json.dumps(rows, indent=0) + "\n")
    print(json.dumps({"tag": args.tag, "selected": len(rows), "drafts": sum(x["draft"] for x in rows),
                      "m2c_failed": sum(x["m2c_rc"] != 0 for x in rows),
                      "m2c_error_marked": sum(bool(x["m2c_errors"]) for x in rows), "out": str(out)}))
    return 0


if __name__ == "__main__":
    sys.exit(main())
