#!/usr/bin/env python3
"""Function boundaries of a decrypted module, from Nakagawa Recomp's analyzer and (optionally) Ghidra.

    python tools/lbpbounds.py --elf FILE --toml analyzer.toml [--ghidra functions.csv] --out data/functions-NAME.txt

Inputs:
  --toml    the function inventory of Nakagawa Recomp's `tools/analyze.py <elf> --toml=OUT` (entry points found by
            recursive descent from the entry, exports, jal targets, ctor/dtor arrays and code pointers in data).
  --ghidra  optional: a CSV with a hexadecimal start address in the first column and, if present, the end (exclusive)
            or size in a column named `end`/`size` (Ghidra's ExportFunctionsCSV.java, or any export in that form).

Every start inside an executable section (section headers; when there are none, the executable load segment up to
the module's read-only tables, the import stubs counted as `.sceStub.text`: segment_code_ranges) becomes
a function that runs to the next start or the end of its section. Trailing alignment padding (zero words that are not
the delay slot of a final jump or branch) is cut off, so `end` is the end of the function's own code: the padding is
inter-function fill the linker inserts, accounted for by the whole-module build (docs/PLAN.md), not by a source.

Output: one line per function, `start end status` in hex, where status is `agreed` (Ghidra has a function with the
same start and, when it reports one, the same end), `auto` (analyzer only, or no Ghidra input) or `disputed` (Ghidra
reports a different end). Starts that only Ghidra finds are listed as comments (`# ghidra-only 0xADDR`) for review.
"""
from __future__ import annotations

import argparse
import csv
import struct
import sys
import tomllib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmodinfo as MI  # noqa: E402

BRANCH_OPS = {2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17}


def is_control_transfer(w: int) -> bool:
    op, funct = w >> 26, w & 63
    if op in BRANCH_OPS or op == 1:
        return True
    if op == 0 and funct in (8, 9):  # jr, jalr
        return True
    if op == 0x11 and (w >> 21) & 31 == 8:  # bc1x
        return True
    if op == 0x12 and (w >> 21) & 31 == 8:  # bvf/bvt (VFPU branches, COP2)
        return True
    return False


def exec_ranges(elf: MI.Elf) -> list[tuple[int, int, str]]:
    out = [(s["addr"], s["addr"] + s["size"], s["name"]) for s in elf.sections
           if s["flags"] & 4 and s["size"] and s["type"] != 8]
    if not out:
        out = segment_code_ranges(elf)
    return sorted(out)


def _module_tables(elf: MI.Elf) -> tuple[list[tuple[int, int]], list[int]]:
    """([lo, hi) of each import stub table], [address of each table the module info leads to]) of a module: the
    stub tables are code (8 bytes per imported function); the module info itself, the export and import tables and
    the NID, entry and library-name tables they point at are read-only data."""
    mi = MI.module_info(elf)
    if not mi:
        return [], []
    data_at = [mi["ent_top"], mi["stub_top"]]
    seg = next((p for p in elf.load() if p["offset"] <= mi["file_offset"] < p["offset"] + p["filesz"]), None)
    if seg is not None:
        data_at.append(seg["vaddr"] + mi["file_offset"] - seg["offset"])
    stubs = []
    walks = ((mi["stub_top"], mi["stub_end"], "<IHHBBHII", 5), (mi["ent_top"], mi["ent_end"], "<IHHBBHI", 4))
    for top, end, fmt, default in walks:
        a = top
        while a < end:
            o = elf.v2o(a)
            if o is None or o + struct.calcsize(fmt) > len(elf.data):
                break
            row = struct.unpack_from(fmt, elf.data, o)
            name_p, fcount, table_p = row[0], row[5], row[6]
            data_at += [x for x in (name_p, table_p) if x]
            if fmt == "<IHHBBHII" and fcount:
                stubs.append((row[7], row[7] + 8 * fcount))
            a += 4 * (row[3] or default)
    return stubs, data_at


def segment_code_ranges(elf: MI.Elf) -> list[tuple[int, int, str]]:
    """The code of a module without section headers (a PRX often has none): each executable load segment up to the
    read-only tables that follow the code in a PSP module (the module info, the export and import tables and what
    they point at), with the import stubs as `.sceStub.text`. The segment's file size is not the end of the code:
    those tables share the executable segment with it."""
    stubs, data_at = _module_tables(elf)
    out = []
    for p in elf.load():
        if not p["flags"] & 1:
            continue
        lo, hi = p["vaddr"], p["vaddr"] + p["filesz"]
        mine = sorted((a, b) for a, b in stubs if lo <= a < b <= hi)
        end = min((a for a in data_at if lo < a < hi), default=hi)
        end = max([end] + [b for _a, b in mine])
        pos = lo
        for a, b in mine:
            if a < pos:
                a = pos
            if pos < a:
                out.append((pos, a, "segment"))
            if a < b:
                out.append((a, b, ".sceStub.text"))
            pos = max(pos, b)
        if pos < end:
            out.append((pos, end, "segment"))
    return out


def trim_padding(elf: MI.Elf, start: int, end: int) -> int:
    while end - start > 4:
        o = elf.v2o(end - 4)
        if o is None:
            break
        last = MI.u32(elf.data, o)
        prev = MI.u32(elf.data, o - 4)
        if last != 0 or is_control_transfer(prev):
            break
        end -= 4
    return end


def load_ghidra(path: Path) -> dict[int, int | None]:
    out: dict[int, int | None] = {}
    with path.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.reader(stream))
    if not rows:
        return out
    head = [c.strip().lower() for c in rows[0]]
    has_header = not all(ch in "0123456789abcdefx" for ch in head[0])
    body = rows[1:] if has_header else rows
    end_col = head.index("end") if has_header and "end" in head else None
    size_col = head.index("size") if has_header and "size" in head else None
    for r in body:
        if not r or not r[0].strip():
            continue
        try:
            start = int(r[0].strip(), 16)
        except ValueError:
            continue
        end = None
        if end_col is not None and r[end_col].strip():
            end = int(r[end_col].strip(), 16)
        elif size_col is not None and r[size_col].strip():
            end = start + int(r[size_col].strip(), 0)
        out[start & 0xFFFFFFFF] = end
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--elf", required=True, type=Path)
    ap.add_argument("--toml", required=True, type=Path)
    ap.add_argument("--ghidra", type=Path)
    ap.add_argument("--ghidra-base", default="0", help="image base Ghidra used (subtracted from its addresses)")
    ap.add_argument("--out", required=True, type=Path)
    args = ap.parse_args()
    elf = MI.Elf(args.elf.read_bytes())
    ranges = exec_ranges(elf)
    doc = tomllib.loads(args.toml.read_text(encoding="utf-8"))
    starts = sorted({int(f["addr"]) & 0xFFFFFFFF for f in doc.get("function", [])})
    inside = [a for a in starts if any(lo <= a < hi for lo, hi, _n in ranges)]
    # A function cannot begin in the delay slot of a jump or branch: such "starts" are the analyzer's view of a
    # delay slot reached from elsewhere (recompiler entry points), not function boundaries.
    in_slot = []
    for a in inside:
        o = elf.v2o(a - 4) if a >= 4 else None
        if o is not None and any(lo <= a - 4 < hi for lo, hi, _n in ranges) and is_control_transfer(MI.u32(elf.data, o)):
            in_slot.append(a)
    inside = [a for a in inside if a not in set(in_slot)]
    ghidra = {}
    if args.ghidra:
        base = int(args.ghidra_base, 16)
        ghidra = {(s - base) & 0xFFFFFFFF: (e - base if e is not None else None) for s, e in load_ghidra(args.ghidra).items()}
    lines, counts = [], {"agreed": 0, "auto": 0, "disputed": 0}
    code_bytes = 0
    for lo, hi, name in ranges:
        sec_starts = [a for a in inside if lo <= a < hi]
        if sec_starts and sec_starts[0] != lo:
            lines.append(f"# {name}: 0x{lo:x}..0x{sec_starts[0]:x} precedes the first found start")
        for i, a in enumerate(sec_starts):
            nxt = sec_starts[i + 1] if i + 1 < len(sec_starts) else hi
            end = trim_padding(elf, a, nxt)
            status = "auto"
            if ghidra:
                if a in ghidra:
                    g_end = ghidra[a]
                    status = "agreed" if g_end is None or g_end in (end, nxt) else "disputed"
            counts[status] += 1
            code_bytes += end - a
            lines.append(f"{a:x} {end:x} {status}")
    only_g = sorted(a for a in ghidra if a not in set(inside) and any(lo <= a < hi for lo, hi, _n in ranges))
    lines += [f"# ghidra-only 0x{a:08x}" for a in only_g]
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text("".join(line + "\n" for line in lines), encoding="utf-8", newline="\n")
    print(f"{args.out}: {sum(counts.values())} functions, {code_bytes} code bytes; {counts}; "
          f"analyzer starts outside code {len(starts) - len(inside) - len(in_slot)}; in delay slots {len(in_slot)}; "
          f"ghidra-only starts {len(only_g)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
