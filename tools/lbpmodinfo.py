#!/usr/bin/env python3
"""Describe decrypted PSP modules (ELF/PRX): identity, layout, toolchain strings, imports and exports.

    python tools/lbpmodinfo.py FILE [FILE ...] [--json out.json] [--nids]

For each module: size and SHA-256, ELF type, entry, program headers (load segments), section headers (names, sizes,
flags) when present, the `.comment` strings (compiler and assembler identification), the SceModuleInfo record (module
name, version, attributes, gp, entry/stub table bounds), the imported libraries (name, version, function and variable
counts; with --nids every NID) and the exported libraries. Everything is read locally; nothing is written except the
optional --json report (keep it under try/ or inputs/: it names retail details). The exit status is 1 when a FILE
cannot be described (not an ELF, truncated or unreadable).

`--modules-entry NAME:SRC_DIR:KIND:ORIGIN` prints a data/modules.json entry for the first FILE (load segments from its
program headers; functions_file data/functions-NAME.txt).
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ET_NAMES = {1: "REL", 2: "EXEC", 3: "DYN", 0xFFA0: "SCE_PRX", 0xFE04: "SCE_EXEC?"}
SHT_PRXREL = 0x700000A0
SHT_PRXREL2 = 0x700000A1


def u32(b: bytes, o: int) -> int:
    return struct.unpack_from("<I", b, o)[0]


def cstr(b: bytes, o: int, limit: int = 256) -> str:
    if o < 0 or o >= len(b):
        return ""
    end = b.find(b"\0", o, o + limit)
    return b[o:end if end >= 0 else o + limit].decode("latin-1")


class Elf:
    def __init__(self, data: bytes):
        if data[:4] != b"\x7fELF":
            raise ValueError("not an ELF file (still encrypted or compressed?)")
        if len(data) < 52:
            raise ValueError(f"truncated: {len(data)} bytes, shorter than an ELF header")
        if data[4] != 1 or data[5] != 1:
            raise ValueError("not a 32-bit little-endian ELF")
        self.data = data
        (self.type, self.machine, _v, self.entry, self.phoff, self.shoff, self.flags, _eh, self.phentsize, self.phnum,
         self.shentsize, self.shnum, self.shstrndx) = struct.unpack_from("<HHIIIIIHHHHHH", data, 16)
        if self.phnum and (self.phentsize < 32 or self.phoff + self.phnum * self.phentsize > len(data)):
            raise ValueError(f"truncated: the program header table ends past the file's {len(data)} bytes")
        self.phdrs = [dict(zip(("type", "offset", "vaddr", "paddr", "filesz", "memsz", "flags", "align"),
                               struct.unpack_from("<8I", data, self.phoff + i * self.phentsize)))
                      for i in range(self.phnum)]
        self.sections = []
        if self.shoff and self.shnum and self.shoff + self.shnum * self.shentsize <= len(data):
            raw = [struct.unpack_from("<10I", data, self.shoff + i * self.shentsize) for i in range(self.shnum)]
            strtab = raw[self.shstrndx] if self.shstrndx < len(raw) else None
            for s in raw:
                name = cstr(data, strtab[4] + s[0]) if strtab else ""
                self.sections.append(dict(zip(("name_off", "type", "flags", "addr", "offset", "size", "link", "info",
                                               "addralign", "entsize"), s), name=name))

    def load(self) -> list[dict]:
        return [p for p in self.phdrs if p["type"] == 1]

    def v2o(self, addr: int) -> int | None:
        for p in self.load():
            if p["vaddr"] <= addr < p["vaddr"] + p["filesz"]:
                return p["offset"] + addr - p["vaddr"]
        return None

    def vstr(self, addr: int) -> str:
        o = self.v2o(addr)
        return cstr(self.data, o) if o is not None else f"<0x{addr:x}>"

    def section(self, name: str) -> dict | None:
        return next((s for s in self.sections if s["name"] == name), None)


def module_info(elf: Elf) -> dict | None:
    sec = elf.section(".rodata.sceModuleInfo")
    off = sec["offset"] if sec else None
    if off is None and elf.load():
        first = elf.load()[0]
        if elf.type == 0xFFA0:
            off = first["paddr"] & 0x7FFFFFFF  # PRX: file offset of SceModuleInfo
        else:
            off = elf.v2o(first["paddr"]) if first["paddr"] else None
    if off is None or off + 52 > len(elf.data):
        return None
    attr, ver_lo, ver_hi = struct.unpack_from("<HBB", elf.data, off)
    name = cstr(elf.data, off + 4, 28)
    gp, ent_top, ent_end, stub_top, stub_end = struct.unpack_from("<5I", elf.data, off + 32)
    return {"file_offset": off, "attribute": attr, "version": f"{ver_hi}.{ver_lo}", "name": name, "gp": gp,
            "ent_top": ent_top, "ent_end": ent_end, "stub_top": stub_top, "stub_end": stub_end}


def imports(elf: Elf, mi: dict, nids: bool) -> list[dict]:
    out = []
    a = mi["stub_top"]
    while a < mi["stub_end"]:
        o = elf.v2o(a)
        if o is None:
            break
        name_p, ver, attr, size, vcount, fcount, nid_p, stub_p = struct.unpack_from("<IHHBBHII", elf.data, o)
        size = size or 5
        row = {"library": elf.vstr(name_p) if name_p else "", "version": f"0x{ver:04x}", "attr": f"0x{attr:04x}",
               "functions": fcount, "variables": vcount, "stub_table": f"0x{stub_p:08x}"}
        if nids:
            no = elf.v2o(nid_p)
            row["nids"] = [f"0x{u32(elf.data, no + 4 * i):08x}" for i in range(fcount)] if no is not None else []
        out.append(row)
        a += 4 * size
    return out


def exports(elf: Elf, mi: dict, nids: bool) -> list[dict]:
    out = []
    a = mi["ent_top"]
    while a < mi["ent_end"]:
        o = elf.v2o(a)
        if o is None:
            break
        name_p, ver, attr, size, vcount, fcount, table_p = struct.unpack_from("<IHHBBHI", elf.data, o)
        size = size or 4
        row = {"library": elf.vstr(name_p) if name_p else "(module)", "version": f"0x{ver:04x}", "attr": f"0x{attr:04x}",
               "functions": fcount, "variables": vcount}
        if nids:
            to = elf.v2o(table_p)
            n = fcount + vcount
            if to is not None:
                row["nids"] = [f"0x{u32(elf.data, to + 4 * i):08x}" for i in range(n)]
                row["targets"] = [f"0x{u32(elf.data, to + 4 * (n + i)):08x}" for i in range(n)]
        out.append(row)
        a += 4 * size
    return out


def describe(path: Path, nids: bool) -> dict:
    data = path.read_bytes()
    elf = Elf(data)
    rep = {"file": path.name, "size": len(data), "sha256": hashlib.sha256(data).hexdigest(),
           "elf_type": ET_NAMES.get(elf.type, hex(elf.type)), "machine": elf.machine, "flags": f"0x{elf.flags:08x}",
           "entry": f"0x{elf.entry:08x}",
           "program_headers": [{k: (f"0x{v:x}" if k != "type" else v) for k, v in p.items()} for p in elf.phdrs],
           "sections": [{"name": s["name"], "type": f"0x{s['type']:x}", "flags": f"0x{s['flags']:x}",
                         "addr": f"0x{s['addr']:08x}", "size": s["size"]} for s in elf.sections]}
    comment = elf.section(".comment")
    if comment:
        blob = data[comment["offset"]:comment["offset"] + comment["size"]]
        rep["comment"] = sorted({s.decode("latin-1") for s in blob.split(b"\0") if s.strip()})
    mi = module_info(elf)
    if mi:
        rep["module_info"] = {k: (f"0x{v:08x}" if isinstance(v, int) and k not in ("attribute",) else v)
                              for k, v in mi.items()}
        rep["imports"] = imports(elf, mi, nids)
        rep["exports"] = exports(elf, mi, nids)
    code = sum(s["size"] for s in elf.sections if s["flags"] & 4)
    rep["code_bytes"] = code
    return rep


def modules_entry(path: Path, spec: str) -> dict:
    parts = spec.split(":", 3)
    if len(parts) != 4:
        raise ValueError(f"--modules-entry wants NAME:SRC_DIR:KIND:ORIGIN, got {spec!r}")
    name, src_dir, kind, origin = parts
    data = path.read_bytes()
    elf = Elf(data)
    segs = [{"flags": p["flags"], "start": p["vaddr"], "file_offset": p["offset"], "file_end": p["offset"] + p["filesz"],
             "memory_end": p["vaddr"] + p["memsz"]} for p in elf.load()]
    return {"name": name, "src_dir": src_dir, "kind": kind, "origin": origin,
            "description": "the decrypted (and, if compressed, inflated) module", "sha256": hashlib.sha256(data).hexdigest(),
            "size": len(data), "load_segments": segs, "functions_file": f"data/functions-{name}.txt", "function_count": 0}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+", type=Path)
    ap.add_argument("--json", type=Path)
    ap.add_argument("--nids", action="store_true")
    ap.add_argument("--modules-entry")
    args = ap.parse_args()
    if args.modules_entry:
        try:
            entry = modules_entry(args.files[0], args.modules_entry)
        except (ValueError, OSError) as e:
            raise SystemExit(f"{args.files[0]}: {e}")
        print(json.dumps(entry, indent=2))
        return 0
    reports, failed = [], 0
    for path in args.files:
        try:
            rep = describe(path, args.nids)
        except (ValueError, OSError) as e:
            print(f"== {path.name}: {e}")
            failed += 1
            continue
        reports.append(rep)
        print(f"== {rep['file']}: {rep['size']} B sha256 {rep['sha256']} {rep['elf_type']} entry {rep['entry']} "
              f"flags {rep['flags']}")
        for p in rep["program_headers"]:
            print(f"   phdr type {p['type']} off {p['offset']} vaddr {p['vaddr']} paddr {p['paddr']} filesz {p['filesz']} "
                  f"memsz {p['memsz']} flags {p['flags']}")
        if rep["sections"]:
            print("   sections: " + ", ".join(f"{s['name']}({s['size']})" for s in rep["sections"] if s["name"]))
        for c in rep.get("comment", []):
            print(f"   .comment: {c}")
        mi = rep.get("module_info")
        if mi:
            print(f"   module {mi['name']!r} v{mi['version']} attr 0x{mi['attribute']:04x} gp {mi['gp']}")
            for row in rep["imports"]:
                print(f"   import {row['library']:28} {row['version']} funcs {row['functions']:4} vars {row['variables']}")
            for row in rep["exports"]:
                print(f"   export {row['library']:28} {row['version']} funcs {row['functions']:4} vars {row['variables']}")
    if args.json:
        args.json.write_text(json.dumps(reports, indent=1) + "\n", encoding="utf-8")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
