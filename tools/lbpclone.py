#!/usr/bin/env python3
"""Propagate exact sources to their clones: functions whose retail code is identical apart from call
targets and absolute (hi/lo) addresses. Template instances and copied code are common in this game.

    python tools/lbpclone.py --tag c1 [--module EBOOT.BIN] [--parallel 12] [--dry-run]
    python tools/lbpclone.py --tag r1 --refresh <exemplar> [...]     re-derive the existing clones of rewritten exemplars

For each exact source in src/<module dir> (the exemplar) and each unmatched reviewed function with the
same masked signature (jal targets and lui-based immediates masked), the tool derives the address
mapping from the two retail byte streams. It then rewrites the exemplar's address-named symbols
(FUN_/DAT_/LAB_/PTR_/jtbl_), with data references allowed a constant symbol+offset shift. Every
occurrence of the exemplar's own address (function and class names) is retargeted, and the
header's start, end and symbol are set to the clone's. Only candidates that lbpmatch.py reports
EXACT are written to src/, with a provenance note naming the exemplar.
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M
import lbpproject as P

SRC_DIRS = P.src_dirs()  # {module: src/ subdirectory}, from data/modules.json
IDENT_HEX = re.compile(r"\b([A-Za-z]\w*?)_([0-9a-f]{8})(?![0-9A-Za-z])")
ADDRESS_KINDS = {"FUN", "DAT", "LAB", "PTR", "jtbl"}
NOTE = re.compile(r"^/\*\s*(Mechanical translation|Clone of).*?\*/\n", re.M)
MAX_DATA_SHIFT = 0x10000
# I-type opcodes that write rt: ALU immediates and integer loads (lui is handled separately)
GPR_WRITE_RT = set(range(0x08, 0x0F)) | set(range(0x20, 0x27))


def words(data: bytes) -> list[int]:
    return [int.from_bytes(data[i:i + 4], "little") for i in range(0, len(data) - len(data) % 4, 4)]


def analyse(ws: list[int]) -> tuple[tuple[int, ...], list[tuple[str, int, int]]]:
    """Masked signature, plus the references in order: ("call", index, target) / ("data", index, address)."""
    sig, refs = [], []
    hi: dict[int, int] = {}
    for i, w in enumerate(ws):
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        if op in (2, 3):
            sig.append(op << 26)
            refs.append(("call", i, (w & 0x03FFFFFF) << 2))
        elif op == 0x0F:
            sig.append(w & 0xFFFF0000)
            hi[rt] = (w & 0xFFFF) << 16
            refs.append(("hi", i, (w & 0xFFFF) << 16))
            continue
        elif op >= 8 and rs in hi:
            sig.append(w & 0xFFFF0000)
            imm = w & 0xFFFF
            if op not in (0x0C, 0x0D, 0x0E) and imm & 0x8000:  # andi/ori/xori zero-extend
                imm -= 0x10000
            refs.append(("data", i, (hi[rs] + imm) & 0xFFFFFFFF if op != 0x0D else hi[rs] | imm))
        else:
            sig.append(w)
        # a register written here no longer holds a lui high half
        if op in (0x00, 0x1C, 0x1F):
            hi.pop((w >> 11) & 31, None)
        elif op in GPR_WRITE_RT or (op == 0x11 and rs == 0):
            hi.pop(rt, None)
    return tuple(sig), refs


def mapping(e_refs, t_refs) -> tuple[dict[int, int], dict[int, int], dict[int, int]] | None:
    calls, data, his = {}, {}, {}
    if len(e_refs) != len(t_refs):
        return None
    bad_hi = set()
    for (ek, ei, ev), (tk, ti, tv) in zip(e_refs, t_refs):
        if ek != tk or ei != ti:
            return None
        if ek == "hi":
            # two exemplar addresses in one 64 KiB page can land in two target pages; the hi table only feeds
            # literal rewriting, so an ambiguous high half is dropped (its literal is left alone), never fatal
            # (as hst-decomp's hstclone.mapping)
            if his.setdefault(ev, tv) != tv:
                bad_hi.add(ev)
            continue
        table = calls if ek == "call" else data
        if table.setdefault(ev, tv) != tv:
            return None
    for key in bad_hi:
        his.pop(key, None)
    return calls, data, his


def map_value(kind: str, value: int, e_start: int, t_start: int, calls: dict[int, int], data: dict[int, int]) -> int | None:
    """The clone's value for an address carried in an identifier; None leaves the identifier unchanged."""
    if value == e_start:
        return t_start
    if value in calls:
        return calls[value]
    if value in data:
        return data[value]
    if kind in ADDRESS_KINDS:
        shifts = {tv - ev for ev, tv in data.items() if 0 <= ev - value < MAX_DATA_SHIFT}
        if len(shifts) == 1:
            return (value + shifts.pop()) & 0xFFFFFFFF
        raise KeyError(value)  # an address-named external the retail code does not reveal
    return None


def retarget_names(text: str, e_start: int, t_start: int, calls: dict[int, int], data: dict[int, int]) -> str:
    """Rewrite every identifier ending in _<8 hex> whose address maps (function, class and vtable names)."""
    def repl(m: re.Match) -> str:
        new = map_value(m.group(1), int(m.group(2), 16), e_start, t_start, calls, data)
        return m.group(0) if new is None else f"{m.group(1)}_{new:08x}"
    return IDENT_HEX.sub(repl, text)


def symbol_entries(base: dict[str, int], e_start: int, t_start: int, calls: dict[int, int],
                   data: dict[int, int], his: dict[int, int] | None = None, keep_names: bool = False) -> dict[str, int]:
    """Symbol-map entries a clone needs: each committed entry whose name and address both map
    (e.g. __vt__10C_<exemplar> = its vtable -> __vt__10C_<clone> = the clone's vtable)."""
    out = {}
    for name, address in base.items():
        if address == e_start:
            new_address = t_start
        else:
            new_address = calls.get(address, data.get(address))
        if new_address is None:
            continue
        new_name = re.sub(r"[0-9a-f]{8}", lambda m: f"{map_value('', int(m.group(0), 16), e_start, t_start, calls, data) or int(m.group(0), 16):08x}", name)
        if new_name != name or keep_names:  # across modules even runtime names (__dl__FPv) move
            out[new_name] = new_address
    return out


def retarget(text: str, e_start: int, t_start: int, t_end: int, calls: dict[int, int], data: dict[int, int],
             his: dict[int, int] | None = None) -> str | None:
    try:
        body = retarget_names(text, e_start, t_start, calls, data)
    except KeyError:
        return None
    body = body.replace(f"{e_start:08x}", f"{t_start:08x}")  # mangled names such as __dt__10C_<addr>Fv
    # Literal constants the code builds with lui/ori (or lui/addiu) differ between template instances
    # (e.g. max_size for each element size): rewrite a literal whose value is such a mapped constant.
    head0, rest0 = body.split("\n", 1)

    def literal(m: re.Match) -> str:
        value = int(m.group(0), 16)
        if value >= 0x10000 and value in data and data[value] != value:
            return f"0x{data[value]:x}"
        upper = value & 0xFFFF0000
        if his and value >= 0x10000 and upper in his and his[upper] != upper:
            return f"0x{his[upper] | (value & 0xFFFF):x}"  # high half built by a lui whose low half is elsewhere
        return m.group(0)
    body = head0 + "\n" + re.sub(r"\b0x[0-9a-fA-F]+\b", literal, rest0)
    # Template instances over element size: an exemplar element `int w[K]` whose max_size constant
    # (0xffffffff / (4K)) maps to another max_size gets the clone's element size.
    def element(m: re.Match) -> str:
        words_e = int(m.group(1))
        max_e = 0xFFFFFFFF // (4 * words_e)
        max_t = None
        # the code may hold max_size itself or max_size + 1 (`n < max + 1`), as a lui/ori pair or a lone lui
        for key, adjust in ((max_e, 0), (max_e + 1, 1)):
            if key in data:
                max_t = data[key] - adjust
            elif his and key & 0xFFFF == 0 and key in his:
                max_t = his[key] - adjust
            elif his and (key & 0xFFFF0000) in his:
                max_t = (his[key & 0xFFFF0000] | (key & 0xFFFF)) - adjust
            if max_t:
                break
        if not max_t or max_t == max_e:
            return m.group(0)
        size_t = 0xFFFFFFFF // max_t
        if size_t % 4 or 0xFFFFFFFF // size_t != max_t:
            return m.group(0)
        return f"int w[{size_t // 4}]"
    body = re.sub(r"int w\[(\d+)\]", element, body)
    head, rest = body.split("\n", 1)
    head = re.sub(r"start=0x[0-9a-fA-F]+", f"start=0x{t_start:08x}", head)
    head = re.sub(r"end=0x[0-9a-fA-F]+", f"end=0x{t_end:08x}", head)
    # helpers=NAME@0xSTART-0xEND: a local helper compiled into the same object. The exemplar calls it, so the
    # call mapping gives the clone's helper; its size is the exemplar helper's (lbpmatch then checks its bytes).
    unmapped = []

    def helper(m: re.Match) -> str:
        e_lo, e_hi = int(m.group(2), 16), int(m.group(3), 16)
        t_lo = calls.get(e_lo)
        if t_lo is None:
            unmapped.append(e_lo)
            return m.group(0)
        return f"{m.group(1)}@0x{t_lo:08x}-0x{t_lo + (e_hi - e_lo):08x}"
    head = re.sub(r"(\w+)@0x([0-9a-fA-F]{8})-0x([0-9a-fA-F]{8})", helper, head)
    if unmapped:
        return None  # a helper the exemplar never calls directly cannot be placed in the clone
    # a jump table's base is built with lui/addiu, so the placed .rodata moves with the data mapping; the other
    # placed data sections (data=, sdata=, bss=, sbss=) move with it as well (as hst-decomp's hstclone.retarget)
    def placed(m: re.Match) -> str:
        try:
            new = map_value("DAT", int(m.group(2), 16), e_start, t_start, calls, data)
        except KeyError:
            return m.group(0)  # no data reference reveals the clone's placement: keep the exemplar's (lbpmatch decides)
        return m.group(0) if new is None else f"{m.group(1)}=0x{new:08x}"
    head = re.sub(r"\b(rodata|data|sdata|bss|sbss)=0x([0-9a-fA-F]+)", placed, head)
    return head + "\n" + rest


def load_functions(ledger: dict, name: str) -> dict[int, tuple[int, tuple]]:
    module = next(m for m in ledger["modules"] if m["name"] == name)
    data, seg = M.module_bytes({}, name)
    out = {}
    for f in module["functions"]:
        if f.get("boundary_status") not in P.USABLE_BOUNDARY:
            continue
        off = seg["file_offset"] + f["start"] - seg["start"]
        out[f["start"]] = (f["end"], analyse(words(data[off:off + f["end"] - f["start"]])))
    return out


CLONE_OF = re.compile(r"^/\* Clone of (src/\S+) \(lbpclone\.py tag [\w./-]+\)")


def refresh(tag: str, exemplars: list[Path], parallel: int, dry_run: bool) -> int:
    """Re-derive the existing clones of rewritten exemplars (a readability pass): every source in src/ whose note says
    `Clone of <exemplar>` gets the exemplar's current text with its own addresses substituted, exactly as a new clone
    would. Only a candidate lbpmatch.py reports EXACT replaces the clone (same path, note naming this tag); the others
    are left as they were and listed. The caller then runs tools/lbppromote.py promote on the replaced files."""
    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    gen = M.ROOT / "try" / f"clone-{tag}"
    if gen.exists():
        raise SystemExit(f"{gen} exists; use a new tag")
    wanted = {p.resolve().relative_to(M.ROOT).as_posix(): p.resolve() for p in exemplars}
    clones: dict[str, list[Path]] = defaultdict(list)
    for q in sorted((M.ROOT / "src").rglob("*")):
        if q.suffix in (".c", ".cpp"):
            lines = q.read_text(encoding="utf-8").split("\n", 2)
            m = CLONE_OF.match(lines[1]) if len(lines) > 1 else None
            if m and m.group(1) in wanted:
                clones[m.group(1)].append(q)
    functions_by_module: dict[str, dict] = {}
    base_symbols: dict[str, dict] = {}
    candidates: list[tuple[Path, Path, Path, dict]] = []   # (exemplar, clone, candidate, symbol entries)
    for rel_e, e_path in sorted(wanted.items()):
        eh = M.parse_header(e_path)
        module = eh["module"]
        if module not in functions_by_module:
            functions_by_module[module] = load_functions(ledger, module)
            base_symbols[module] = M.load_symbols(module)
        functions = functions_by_module[module]
        if eh["start"] not in functions:
            print(f"skip {rel_e}: not a function start of the inventory")
            continue
        _e_end, (sig, e_refs) = functions[eh["start"]]
        text = e_path.read_text(encoding="utf-8")
        for clone in clones[rel_e]:
            ch = M.parse_header(clone)
            if ch["start"] not in functions or functions[ch["start"]][1][0] != sig:
                print(f"skip {clone.name}: no longer the exemplar's masked code")
                continue
            t_end, (_sig, t_refs) = functions[ch["start"]]
            maps = mapping(e_refs, t_refs)
            out = retarget(text, eh["start"], ch["start"], t_end, *maps) if maps else None
            if out is None:
                print(f"skip {clone.name}: the exemplar's references do not map")
                continue
            out = NOTE.sub("", out, count=1)
            head, rest = out.split("\n", 1)
            out = head + "\n" + (f"/* Clone of {rel_e} (lbpclone.py tag {tag}): addresses substituted; exact match "
                                 "verified; names and types await human review. */\n") + rest
            old = clone.read_text(encoding="utf-8").split("\n", 2)
            new = out.split("\n", 2)
            if len(old) == 3 and new[0] == old[0] and new[2] == old[2]:
                continue   # only the note (the tag) would change
            dest = gen / "refresh" / clone.name
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_text(out, encoding="utf-8", newline="\n")
            candidates.append((e_path, clone, dest, symbol_entries(base_symbols[module], eh["start"], ch["start"],
                                                                   *maps)))
    print(f"exemplars={len(wanted)} clones={sum(len(v) for v in clones.values())} changed_candidates={len(candidates)}")
    if dry_run or not candidates:
        return 0
    extra: dict[str, int] = {}
    for _e, _c, _d, entries in candidates:
        extra.update(entries)
    extra_path = gen / "symbols-extra.txt"
    extra_path.write_text("".join(f"{n} = 0x{a:08x};\n" for n, a in sorted(extra.items())), encoding="utf-8",
                          newline="\n")
    status: dict[str, str] = {}
    cmd = [sys.executable, str(Path(M.__file__)), "--quiet", "--parallel", str(parallel), "--symbols-extra",
           str(extra_path), "--json"]
    files = [d for _e, _c, d, _x in candidates]
    for k in range(0, len(files), 200):
        part = gen / f"summary-{k // 200}.json"
        subprocess.run(cmd + [str(part)] + [str(p) for p in files[k:k + 200]], stdout=subprocess.DEVNULL)
        for row in json.loads(part.read_text(encoding="utf-8")):
            status[str((M.ROOT / row["source"]).resolve())] = row["status"]
    replaced, kept = [], []
    committed: dict[str, dict[str, int]] = defaultdict(dict)
    for e_path, clone, dest, entries in candidates:
        if status.get(str(dest.resolve())) != "EXACT":
            kept.append(f"{clone.relative_to(M.ROOT).as_posix()}: {status.get(str(dest.resolve()), 'NOT_RUN')}")
            continue
        module = M.parse_header(clone)["module"]
        linked = M.ROOT / "build" / module / M.build_key(dest) / "syms.ld"
        used = set(re.findall(r"^(\S+) = ", linked.read_text(encoding="utf-8"), re.M)) if linked.exists() else set()
        for symbol, address in entries.items():
            if symbol in used and base_symbols[module].get(symbol) != address:
                committed[module][symbol] = address
        clone.write_text(dest.read_text(encoding="utf-8"), encoding="utf-8", newline="\n")
        replaced.append(clone.relative_to(M.ROOT).as_posix())
    for module, entries in committed.items():
        with (M.ROOT / "symbols" / f"{module}.txt").open("a", encoding="utf-8", newline="\n") as stream:
            for symbol, address in sorted(entries.items()):
                stream.write(f"{symbol} = 0x{address:08x};   # lbpclone {tag}\n")
    (gen / "replaced.txt").write_text("".join(f"{p}\n" for p in replaced), encoding="utf-8", newline="\n")
    print(json.dumps({"tag": tag, "replaced": len(replaced), "left_unchanged": len(kept)}))
    for line in kept:
        print(f"left unchanged (candidate not EXACT): {line}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tag", required=True)
    ap.add_argument("--refresh", type=Path, nargs="+", metavar="EXEMPLAR",
                    help="re-derive the existing clones of these (rewritten) exemplars instead of finding new ones")
    ap.add_argument("--module", default=P.default_module(), help="module whose functions are the clone targets")
    ap.add_argument("--from-module", help="module whose exact sources are the exemplars (default: --module)")
    ap.add_argument("--parallel", type=int, default=12)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--max-per-target", type=int, default=3, help="distinct exemplar texts tried per clone")
    args = ap.parse_args()
    if args.refresh:
        return refresh(args.tag, args.refresh, args.parallel, args.dry_run)
    source_module = args.from_module or args.module
    same_module = source_module == args.module
    ledger = json.loads((M.CAMPAIGN / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    functions = load_functions(ledger, args.module)
    e_functions = functions if same_module else load_functions(ledger, source_module)
    sources = defaultdict(dict)
    for p in sorted((M.ROOT / "src").rglob("*")):
        if p.suffix in (".c", ".cpp"):
            h = M.parse_header(p)
            sources[h["module"]][h["start"]] = p
    exemplars: dict[int, Path] = sources[source_module]
    matched_targets = set(sources[args.module])
    by_sig = defaultdict(list)
    for start, (end, (sig, refs)) in functions.items():
        by_sig[sig].append(start)
    gen = M.ROOT / "try" / f"clone-{args.tag}"
    if gen.exists():
        raise SystemExit(f"{gen} exists; use a new tag")
    candidates: dict[int, list[tuple[Path, Path]]] = defaultdict(list)
    seen_text: dict[int, set[str]] = defaultdict(set)
    base_symbols = M.load_symbols(source_module)
    target_symbols = base_symbols if same_module else M.load_symbols(args.module)
    extra: dict[str, int] = {}
    conflicting: set[str] = set()
    owned: dict[Path, dict[str, int]] = {}
    for e_start, path in sorted(exemplars.items()):
        if e_start not in e_functions:
            continue
        e_end, (sig, e_refs) = e_functions[e_start]
        text = path.read_text(encoding="utf-8")
        if not same_module:
            text = text.replace(f"module={source_module}", f"module={args.module}", 1)
        for t_start in by_sig[sig]:
            if t_start in matched_targets or (same_module and t_start == e_start):
                continue
            t_end, (_sig, t_refs) = functions[t_start]
            maps = mapping(e_refs, t_refs)
            if maps is None:
                continue
            if len(candidates[t_start]) >= args.max_per_target:
                continue
            out = retarget(text, e_start, t_start, t_end, *maps)
            if out is None or out in seen_text[t_start]:
                continue
            seen_text[t_start].add(out)
            name = path.name.replace(f"{e_start:08x}", f"{t_start:08x}")
            if name == path.name:
                name = f"{path.stem}_{t_start:08x}{path.suffix}"
            dest = gen / f"e{e_start:08x}" / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_text(out, encoding="utf-8", newline="\n")
            candidates[t_start].append((path, dest))
            entries = symbol_entries(base_symbols, e_start, t_start, *maps, keep_names=not same_module)
            owned[dest] = entries
            for symbol, address in entries.items():
                if target_symbols.get(symbol, address) != address or extra.setdefault(symbol, address) != address:
                    conflicting.add(symbol)
    for symbol in conflicting:
        extra.pop(symbol, None)
    files = [dest for rows in candidates.values() for _src, dest in rows]
    print(f"exemplars={len(exemplars)} clone_targets={len(candidates)} candidates={len(files)} "
          f"symbol_entries={len(extra)} conflicting={len(conflicting)}")
    if args.dry_run or not files:
        return 0
    extra_path = gen / "symbols-extra.txt"
    extra_path.write_text("".join(f"{name} = 0x{address:08x};\n" for name, address in sorted(extra.items())),
                          encoding="utf-8", newline="\n")
    status = {}
    cmd = [sys.executable, str(Path(M.__file__)), "--quiet", "--parallel", str(args.parallel),
           "--symbols-extra", str(extra_path), "--json"]
    for k in range(0, len(files), 200):
        part = gen / f"summary-{k // 200}.json"
        subprocess.run(cmd + [str(part)] + [str(p) for p in files[k:k + 200]], stdout=subprocess.DEVNULL)
        for row in json.loads(part.read_text(encoding="utf-8")):
            status[str((M.ROOT / row["source"]).resolve())] = row["status"]
    dest_dir = M.ROOT / "src" / SRC_DIRS[args.module]
    dest_dir.mkdir(parents=True, exist_ok=True)
    covered = {M.parse_header(q)["start"] for q in (M.ROOT / "src").rglob("*")
               if q.suffix in (".c", ".cpp") and M.parse_header(q)["module"] == args.module}
    accepted = 0
    accepted_bytes = 0
    committed: dict[str, int] = {}
    for t_start, rows in sorted(candidates.items()):
        if t_start in covered:
            continue
        for exemplar, cand in rows:
            if status.get(str(cand.resolve())) != "EXACT":
                continue
            text = NOTE.sub("", cand.read_text(encoding="utf-8"), count=1)
            head, rest = text.split("\n", 1)
            note = (f"/* Clone of src/{exemplar.relative_to(M.ROOT / 'src').as_posix()} (lbpclone.py tag {args.tag}): "
                    "addresses substituted; exact match verified; names and types await human review. */\n")
            out = dest_dir / cand.name
            if out.exists():
                print(f"skip {out.name}: exists")
                continue
            out.write_text(head + "\n" + note + rest, encoding="utf-8", newline="\n")
            accepted += 1
            accepted_bytes += functions[t_start][0] - t_start
            # commit the symbol entries this clone actually linked against (lbpmatch's syms.ld)
            linked = M.ROOT / "build" / args.module / M.build_key(cand) / "syms.ld"
            used = set(re.findall(r"^(\S+) = ", linked.read_text(encoding="utf-8"), re.M)) if linked.exists() else set()
            for symbol, address in owned.get(cand, {}).items():
                # a name the target's symbol file already defines at this address needs no second line
                if symbol in used and symbol in extra and symbol not in committed and target_symbols.get(symbol) != address:
                    committed[symbol] = address
            break
    if committed:
        with (M.ROOT / "symbols" / f"{args.module}.txt").open("a", encoding="utf-8", newline="\n") as stream:
            for symbol, address in sorted(committed.items()):
                stream.write(f"{symbol} = 0x{address:08x};   # lbpclone {args.tag}\n")
    counts: dict[str, int] = {}
    for cand in files:
        key = status.get(str(cand.resolve()), "NOT_RUN")
        counts[key] = counts.get(key, 0) + 1
    report = {"tag": args.tag, "module": args.module, "exemplars": len(exemplars), "clone_targets": len(candidates),
              "candidates": len(files), "accepted": accepted, "accepted_bytes": accepted_bytes, "status_counts": counts}
    (gen / "report.json").write_text(json.dumps(report, indent=1) + "\n", encoding="utf-8")
    print(json.dumps(report))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
