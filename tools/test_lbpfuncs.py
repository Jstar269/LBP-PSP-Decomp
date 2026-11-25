#!/usr/bin/env python3
"""Tests for tools/lbpfuncs.py on synthetic modules built here byte by byte (no game data)."""
from __future__ import annotations

import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpfuncs as F  # noqa: E402
import lbpmodinfo as MI  # noqa: E402


def words(*ws: int) -> bytes:
    return b"".join(struct.pack("<I", w) for w in ws)


def prx_with_data_relocations() -> bytes:
    """A PRX with section headers and two load segments: .text (one function, 0x00..0x18, with a `nop` load delay
    slot at 0x08) and .rodata.sceModuleInfo in segment 0 at address 0, .data in segment 1 at address 0x100, and a
    PSP relocation section (0x700000A0) for .data with two R_MIPS_32 entries:
      0x100  address segment 1: the word 0x0c means 0x100 + 0x0c, a pointer into .data itself
      0x108  address segment 0: the word 0x00 means 0x0, a pointer to the function
    (two separate pointers: consecutive ones would be read as a jump table)"""
    text = words(0x27BDFFF0, 0x8C820000, 0, 0x00421021, 0x03E00008, 0x27BD0010)
    rel = struct.pack("<IIII", 0x0, 2 | 1 << 8 | 1 << 16, 0x8, 2 | 1 << 8 | 0 << 16)
    names = b"\0.text\0.data\0.rel.data\0.shstrtab\0.rodata.sceModuleInfo\0"

    def n(name: str) -> int:
        return names.index(name.encode() + b"\0")
    off0, off1, rel_off, str_off = 0x80, 0x100, 0x110, 0x120
    sh_off = (str_off + len(names) + 3) & ~3
    shdrs = [words(*[0] * 10),
             words(n(".text"), 1, 6, 0, off0, len(text), 0, 0, 16, 0),
             words(n(".rodata.sceModuleInfo"), 1, 2, 0x20, off0 + 0x20, 52, 0, 0, 4, 0),
             words(n(".data"), 1, 3, 0x100, off1, 12, 0, 0, 16, 0),
             words(n(".rel.data"), MI.SHT_PRXREL, 0, 0, rel_off, len(rel), 0, 3, 0, 8),
             words(n(".shstrtab"), 3, 0, 0, str_off, len(names), 0, 0, 1, 0)]
    phdrs = words(1, off0, 0, off0 + 0x20, 0x54, 0x54, 5, 16) + words(1, off1, 0x100, 0, 12, 12, 6, 16)
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9), 0xFFA0, 8, 1, 0, 52, sh_off, 0,
                       52, 32, 2, 40, len(shdrs), 5)
    out = bytearray(sh_off + 40 * len(shdrs))
    out[:52] = ehdr
    out[52:52 + len(phdrs)] = phdrs
    out[off0:off0 + len(text)] = text
    out[off1:off1 + 12] = words(0x0C, 0x0, 0x0)
    out[rel_off:rel_off + len(rel)] = rel
    out[str_off:str_off + len(names)] = names
    out[sh_off:] = b"".join(shdrs)
    return bytes(out)


def run_main(elf_bytes: bytes) -> list[str]:
    with tempfile.TemporaryDirectory() as tmp:
        elf_path, out = Path(tmp) / "t.prx", Path(tmp) / "functions.txt"
        elf_path.write_bytes(elf_bytes)
        argv, stdout = sys.argv, sys.stdout
        sys.argv = ["lbpfuncs.py", "--elf", str(elf_path), "--out", str(out)]
        try:
            with open(Path(tmp) / "stdout", "w", encoding="utf-8") as sink:
                sys.stdout = sink
                F.main()
        finally:
            sys.argv, sys.stdout = argv, stdout
        return out.read_text(encoding="utf-8").splitlines()


class PrxRelocations(unittest.TestCase):
    def test_word_is_relative_to_its_address_segment(self):
        elf = MI.Elf(prx_with_data_relocations())
        # 0x100 points at 0x10c (data), not at the code address 0x0c; 0x108 points at the function
        self.assertEqual(F.data_code_pointers(elf, F.code_ranges(elf)), {0x108: 0x0})

    def test_no_function_starts_at_a_data_pointer_value(self):
        self.assertEqual(run_main(prx_with_data_relocations()), ["0 18 auto"])


if __name__ == "__main__":
    unittest.main()
