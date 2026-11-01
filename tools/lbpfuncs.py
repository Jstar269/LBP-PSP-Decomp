#!/usr/bin/env python3
"""Decomp-oriented function boundaries for a decrypted PSP module (ELF/PRX), from the code and relocations alone.

    python tools/lbpfuncs.py --elf FILE --out data/functions-NAME.txt [--ref reviewed.txt] [--report try/x.json]

A matching decompilation needs every function's exact [start, end): one source per function, linked at its retail
address. Nakagawa Recomp's analyzer (tools/analyze.py) is tuned for recompilation, where an extra entry point is
harmless, so it over-splits (delay slots and basic blocks become "functions"). This tool finds starts the way the
compilers lay code out:

  1. the module entry, the export-table targets, every `jal` target inside the code and every static constructor and
     destructor `.ctors`/`.dtors` list;
  2. code pointers in data (32-bit relocations whose word points into the code) that sit at a function boundary
     (the word before is the delay slot of a `jr ra`/`j`, or padding, or the target opens a frame) -- jump tables point
     into the middle of functions and are left out;
  3. code that follows a function's last return: after `jr ra` (or an unconditional `j` tail call) and its delay slot
     and any zero padding, a new function starts unless a branch from the current function reaches past that point
     (functions with several epilogues); import stubs (.sceStub.text) are 8-byte functions each.

Each function ends at the next start; trailing zero words that are not a delay slot are alignment padding and are
cut off. Output lines are `start end status` in hex: `auto` (one tool; tools/lbpbounds.py marks agreement with a second
tool) or `stub` (an import stub in .sceStub.text: generated from the import table, never matched by a C source). `--ref` compares with a known list (for calibration on hst-decomp's reviewed boundaries).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpbounds as B  # noqa: E402
import lbpmodinfo as MI  # noqa: E402

R_MIPS_32 = 2
COND_BRANCH = {4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17}


def code_ranges(elf: MI.Elf) -> list[tuple[int, int, str]]:
    return [r for r in B.exec_ranges(elf)]


def words_at(elf: MI.Elf, lo: int, hi: int) -> dict[int, int]:
    o = elf.v2o(lo)
    n = (hi - lo) // 4
    vals = struct.unpack_from(f"<{n}I", elf.data, o)
    return {lo + 4 * i: v for i, v in enumerate(vals)}


def branch_target(pc: int, w: int) -> int | None:
    op = w >> 26
    rt = (w >> 16) & 31
    imm = w & 0xFFFF
    off = (imm - 0x10000 if imm & 0x8000 else imm) * 4
    if op in COND_BRANCH or (op == 1 and rt in (0, 1, 2, 3, 0x10, 0x11, 0x12, 0x13)):
        return pc + 4 + off
    if op == 0x11 and (w >> 21) & 31 == 8:  # bc1f/bc1t(l)
        return pc + 4 + off
    if op == 0x12 and (w >> 21) & 31 == 8:  # VFPU bvf/bvt
        return pc + 4 + off
    return None


def is_jr_ra(w: int) -> bool:
    return w == 0x03E00008


def is_j(w: int) -> bool:
    return w >> 26 == 2


def opens_frame(w: int) -> bool:
    return w >> 16 == 0x27BD and bool(w & 0x8000)  # addiu sp, sp, -N


def data_code_pointers(elf: MI.Elf, ranges) -> dict[int, int]:
    """{data address: code address} for every word that a 32-bit relocation in a non-code section makes point into
    the code (PRX relocation format A0 and standard SHT_REL)."""
    out: dict[int, int] = {}
    phdrs = elf.phdrs
    for s in elf.sections:
        if s["type"] not in (9, MI.SHT_PRXREL):
            continue
        target = elf.sections[s["info"]] if s["type"] == 9 and s["info"] < len(elf.sections) else None
        if target is not None and target["flags"] & 4:
            continue  # relocations of code
        for k in range(s["size"] // 8):
            r_off, r_info = struct.unpack_from("<II", elf.data, s["offset"] + 8 * k)
            if r_info & 0xFF != R_MIPS_32:
                continue
            if s["type"] == MI.SHT_PRXREL:
                ofs_base = (r_info >> 8) & 0xFF
                if ofs_base >= len(phdrs):
                    continue
                addr = phdrs[ofs_base]["vaddr"] + r_off
            else:
                addr = r_off
            o = elf.v2o(addr)
            if o is None or any(lo <= addr < hi for lo, hi, _n in ranges):
                continue
            v = MI.u32(elf.data, o)
            if any(lo <= v < hi for lo, hi, _n in ranges):
                out[addr] = v
    if not any(s["type"] in (9, MI.SHT_PRXREL) for s in elf.sections):
        # A static executable (ET_EXEC, no relocations): every aligned word of the allocated data sections whose value
        # is a code address is taken as a pointer. Exception and unwind tables encode addresses differently and are
        # read separately (eh_frame_ranges).
        for s in elf.sections:
            if not s["flags"] & 2 or s["flags"] & 4 or s["type"] == 8 or not s["size"]:
                continue
            if s["name"] in (".eh_frame", ".gcc_except_table"):
                continue
            n = s["size"] // 4
            vals = struct.unpack_from(f"<{n}I", elf.data, s["offset"])
            for i, v in enumerate(vals):
                if v % 4 == 0 and any(lo <= v < hi for lo, hi, _n in ranges):
                    out[s["addr"] + 4 * i] = v
    return out


def _uleb(b: bytes, i: int) -> tuple[int, int]:
    r = sh = 0
    while True:
        c = b[i]
        i += 1
        r |= (c & 0x7F) << sh
        sh += 7
        if not c & 0x80:
            return r, i


def eh_frame_ranges(elf: MI.Elf) -> list[tuple[int, int]]:
    """[pc_begin, pc_end) of every FDE in .eh_frame (absolute or pc-relative 4-byte pointer encodings): the exact
    extent of each function compiled with unwind tables."""
    sec = elf.section(".eh_frame")
    if not sec:
        return []
    d, o, end, base = elf.data, sec["offset"], sec["offset"] + sec["size"], sec["addr"]
    encodings: dict[int, int] = {}
    out = []
    p = o
    while p + 8 <= end:
        length, cid = struct.unpack_from("<II", d, p)
        if length == 0:
            break
        body = p + 8
        if cid == 0:
            j = d.index(b"\0", body + 1)
            aug = d[body + 1:j].decode("latin-1")
            j += 1
            _caf, j = _uleb(d, j)
            j += 1  # data alignment factor (one-byte sleb on MIPS)
            j += 1  # return address register
            enc = 0
            if aug.startswith("z"):
                _alen, j = _uleb(d, j)
                for ch in aug[1:]:
                    if ch == "R":
                        enc = d[j]
                        j += 1
                    elif ch == "P":
                        penc = d[j]
                        j += 1 + (4 if penc & 0x0F in (0x00, 0x03, 0x0B) else 8)
                    elif ch == "L":
                        j += 1
            encodings[p] = enc
        else:
            enc = encodings.get(p + 4 - cid, 0)
            raw = struct.unpack_from("<I", d, body)[0]
            begin = (base + body - o + raw) & 0xFFFFFFFF if enc & 0x70 == 0x10 else raw
            rng = struct.unpack_from("<I", d, body + 4)[0]
            if rng:
                out.append((begin, begin + rng))
        p += 4 + length
    return out


def init_fini(elf: MI.Elf) -> list[int]:
    """The static constructors and destructors .ctors and .dtors list (without the -1 and 0 terminators). Each is a
    function of its own: GCC emits a translation unit's `_GLOBAL__I_*` and `_GLOBAL__D_*` as its last functions."""
    out = []
    for name in (".ctors", ".dtors"):
        sec = elf.section(name)
        if sec and sec["type"] == 1:
            out += [a for a in struct.unpack_from(f"<{sec['size'] // 4}I", elf.data, sec["offset"])
                    if a not in (0, 0xFFFFFFFF)]
    return out


def find_starts(elf: MI.Elf) -> tuple[list[int], dict]:
    ranges = code_ranges(elf)
    words: dict[int, int] = {}
    for lo, hi, _n in ranges:
        words.update(words_at(elf, lo, hi))
    inside = lambda a: any(lo <= a < hi for lo, hi, _n in ranges)  # noqa: E731
    starts: set[int] = set()
    why: dict[str, int] = {}

    def add(a: int, reason: str) -> None:
        if inside(a) and a % 4 == 0 and a not in starts:
            starts.add(a)
            why[reason] = why.get(reason, 0) + 1

    for lo, hi, name in ranges:
        add(lo, "section_start")
        if name == ".sceStub.text":
            for a in range(lo, hi, 8):
                add(a, "import_stub")
    add(elf.entry & 0xFFFFFFFF, "entry")
    mi = MI.module_info(elf)
    if mi:
        for row in MI.exports(elf, mi, True):
            for t in row.get("targets", []):
                add(int(t, 16), "export")
    for pc, w in words.items():
        if w >> 26 == 3:
            add((pc & 0xF0000000) | ((w & 0x03FFFFFF) << 2), "jal")
    for a, _b in eh_frame_ranges(elf):
        add(a, "eh_frame")
    for a in init_fini(elf):
        add(a, "ctors_dtors")

    def boundary_before(t: int) -> bool:
        p1, p2 = words.get(t - 4), words.get(t - 8)
        if p1 is None:
            return True
        if p1 == 0 and (p2 is None or not (B.is_control_transfer(p2))):
            return True  # padding
        return p2 is not None and (is_jr_ra(p2) or is_j(p2))

    strong = sorted(starts)

    def strong_interval(a: int) -> int:
        """Index of the strong-seed interval holding a (strong seeds: entry, exports, jal targets, stubs)."""
        import bisect
        return bisect.bisect_right(strong, a)

    # Jump tables: within a run of consecutive code pointers in data, targets that share a strong-seed interval with
    # another entry of the run (one function's case labels). Their targets are not function starts. Vtables and callback tables point into
    # different intervals and are kept.
    pointers = data_code_pointers(elf, ranges)
    by_site: dict[int, int] = pointers
    sites = sorted(by_site)
    case_labels: set[int] = set()
    run: list[int] = []
    for i, site in enumerate(sites + [None]):
        if site is not None and run and site == run[-1] + 4:
            run.append(site)
            continue
        if len(run) >= 2:
            per_interval: dict[int, set[int]] = {}
            for x in run:
                per_interval.setdefault(strong_interval(by_site[x]), set()).add(by_site[x])
            for targets in per_interval.values():
                if len(targets) >= 2 or sum(1 for x in run if strong_interval(by_site[x]) == strong_interval(next(iter(targets)))) >= 2:
                    case_labels.update(targets)
        run = [site] if site is not None else []
    for t in sorted(set(by_site.values())):
        if t in case_labels:
            continue
        if boundary_before(t) or opens_frame(words.get(t, 0)):
            add(t, "data_pointer")
    strong_set = set(strong)

    def table_targets(site: int) -> list[int]:
        """Targets of the run of consecutive code pointers that starts at `site` (a jump table)."""
        out = []
        while site in by_site:
            out.append(by_site[site])
            site += 4
        return out

    def dispatch_table(pc: int) -> list[int]:
        """Case labels of the switch whose `jr <reg>` is at pc: the table address is built by a lui/addiu (or a lui
        and the low half of a load) in the few instructions before the jump."""
        his: dict[int, int] = {}
        for q in range(pc - 48, pc, 4):
            w = words.get(q)
            if w is None:
                continue
            op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
            if op == 0x0F:
                his[rt] = (w & 0xFFFF) << 16
            elif op in (0x09, 0x23) and rs in his:  # addiu / lw with a %lo
                imm = w & 0xFFFF
                addr = (his[rs] + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
                if addr in by_site:
                    return table_targets(addr)
        return []
    # code after the last return of a function
    for lo, hi, name in ranges:
        if name == ".sceStub.text":
            continue
        reach = 0           # highest branch target seen since the current function's start
        cur = lo
        pc = lo
        while pc < hi:
            if pc in starts and pc != cur:
                cur, reach = pc, 0
            w = words[pc]
            t = branch_target(pc, w)
            if t is None and is_j(w):
                # `j` inside a function (a case break or loop exit) extends its reach; a `j` to a known start is a
                # tail call and does not
                jt = (pc & 0xF0000000) | ((w & 0x03FFFFFF) << 2)
                slot = words.get(pc + 4, 0)
                tail = slot >> 16 == 0x27BD and not slot & 0x8000  # delay slot pops the frame: a tail call
                nxt_strong = strong[strong_interval(pc)] if strong_interval(pc) < len(strong) else hi
                if jt not in strong_set and pc < jt < nxt_strong and not tail:
                    t = jt
            jr_reg = w & 0xFC1FFFFF == 0x00000008 and (w >> 21) & 31 != 31
            if t is None and jr_reg:
                # jr <reg>: a switch reaches its case labels from here; otherwise (a call through a register in tail
                # position) the function ends like at `jr ra`
                labels = [x for x in dispatch_table(pc) if x > pc]
                if labels:
                    t = max(labels)
            if t is not None and t > reach and cur <= t < hi:
                reach = t
            if (is_jr_ra(w) or is_j(w) or jr_reg) and pc + 8 <= hi:
                nxt = pc + 8
                while nxt < hi and words[nxt] == 0 and nxt not in starts:
                    nxt += 4
                if nxt < hi and reach < nxt:
                    if nxt not in starts:
                        add(nxt, "after_return")
                pc += 8
                continue
            pc += 4
    # Unwind tables give exact extents: nothing starts strictly inside an FDE range (exception landing pads follow
    # the return and are reached only through .gcc_except_table), and the code right after an FDE range starts a
    # function unless it is padding.
    fdes = eh_frame_ranges(elf)
    if fdes:
        inner = {a for a in starts for b, e in fdes if b < a < e}
        if inner:
            starts -= inner
            why["dropped_inside_fde"] = len(inner)
        for _b, e in fdes:
            nxt = e
            while nxt in words and words[nxt] == 0:
                nxt += 4
            if nxt in words and not any(b < nxt < e2 for b, e2 in fdes):
                add(nxt, "eh_frame_end")
    return sorted(starts), why


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--elf", required=True, type=Path)
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--ref", type=Path, help="known `start end` list to compare with (calibration)")
    ap.add_argument("--report", type=Path)
    args = ap.parse_args()
    elf = MI.Elf(args.elf.read_bytes())
    starts, why = find_starts(elf)
    ranges = code_ranges(elf)
    lines, funcs = [], {}
    for lo, hi, name in ranges:
        sec = [a for a in starts if lo <= a < hi]
        for i, a in enumerate(sec):
            nxt = sec[i + 1] if i + 1 < len(sec) else hi
            end = B.trim_padding(elf, a, nxt)
            funcs[a] = end
            # import stubs are generated from the import table (the SDK's stub assembly), not C functions
            lines.append(f"{a:x} {end:x} {'stub' if name == '.sceStub.text' else 'auto'}")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text("".join(line + "\n" for line in lines), encoding="utf-8", newline="\n")
    rep = {"functions": len(funcs), "code_bytes": sum(e - s for s, e in funcs.items()), "reasons": why}
    if args.ref:
        ref = {}
        for line in args.ref.read_text(encoding="utf-8").splitlines():
            if line.strip() and not line.startswith("#"):
                a, b = line.split()[:2]
                ref[int(a, 16)] = int(b, 16)
        both = set(ref) & set(funcs)
        rep["ref"] = {"ref": len(ref), "common_starts": len(both),
                      "same_span": sum(1 for a in both if ref[a] == funcs[a]),
                      "ref_only": len(set(ref) - set(funcs)), "ours_only": len(set(funcs) - set(ref)),
                      "ref_only_sample": [f"0x{a:x}" for a in sorted(set(ref) - set(funcs))[:15]],
                      "ours_only_sample": [f"0x{a:x}" for a in sorted(set(funcs) - set(ref))[:15]],
                      "end_diff_sample": [f"0x{a:x}:0x{funcs[a]:x}/0x{ref[a]:x}" for a in sorted(both)
                                          if ref[a] != funcs[a]][:15]}
    print(json.dumps(rep, indent=1))
    if args.report:
        args.report.write_text(json.dumps(rep, indent=1) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
