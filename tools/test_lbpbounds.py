#!/usr/bin/env python3
"""Tests for the code ranges of tools/lbpbounds.py (and tools/lbpfuncs.py, which uses them) on a synthetic module
without section headers, as a PRX often is. The module is built here byte by byte; it holds no game data."""
from __future__ import annotations

import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpbootstrap  # noqa: E402
import lbpbounds as B  # noqa: E402
import lbpfuncs as F  # noqa: E402
import lbpmodinfo as MI  # noqa: E402


def words(*ws: int) -> bytes:
    return b"".join(struct.pack("<I", w) for w in ws)


def sectionless_prx() -> bytes:
    """A PRX laid out like the SDK's: .text, .sceStub.text, then the read-only tables in the same R+X segment
    (.lib.ent, .lib.stub, .rodata.sceResident, .rodata.sceNid, the library name, .rodata.sceModuleInfo), and a data
    segment with .bss. No section headers.

      0x00 one function (calls the first stub), 0x1c alignment padding
      0x20 two import stubs (jr ra; nop)
      0x30 .lib.ent (the module's own export: module_start and the variable module_info)
      0x40 .lib.stub (one library, two functions, stubs at 0x20)
      0x60 entry table: NIDs module_start, module_info; addresses 0x0, 0x88
      0x70 the two imported NIDs, 0x80 the library name, 0x88 SceModuleInfo"""
    seg0 = bytearray(0xBC)
    seg0[0x00:0x20] = words(0x27BDFFF0, 0xAFBF0000, 0x0C000008, 0, 0x8FBF0000, 0x03E00008, 0x27BD0010, 0)
    seg0[0x20:0x30] = words(0x03E00008, 0, 0x03E00008, 0)
    seg0[0x30:0x40] = struct.pack("<IHHBBHI", 0, 0, 0x8000, 4, 1, 1, 0x60)
    seg0[0x40:0x54] = struct.pack("<IHHBBHII", 0x80, 0x0011, 0x0009, 5, 0, 2, 0x70, 0x20)
    seg0[0x60:0x70] = words(0xD632ACDB, 0xF01D73A7, 0x0, 0x88)
    seg0[0x70:0x78] = words(0x12345678, 0x9ABCDEF0)
    seg0[0x80:0x88] = b"TestLib\0"
    seg0[0x88:0xBC] = struct.pack("<HBB28s5I", 0, 1, 1, b"synthetic", 0, 0x30, 0x40, 0x40, 0x54)
    off0, off1 = 0xA0, 0x160
    phdrs = struct.pack("<8I", 1, off0, 0, off0 + 0x88, len(seg0), len(seg0), 5, 16)    # p_paddr: modinfo offset
    phdrs += struct.pack("<8I", 1, off1, 0xC0, 0, 4, 0x20, 6, 16)                       # .data + .bss
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9), 0xFFA0, 8, 1, 0, 52, 0, 0x10A23001,
                       52, 32, 2, 40, 0, 0)
    out = bytearray(off1 + 4)
    out[:52] = ehdr
    out[52:52 + len(phdrs)] = phdrs
    out[off0:off0 + len(seg0)] = seg0
    out[off1:off1 + 4] = words(0x11111111)
    return bytes(out)


class SectionlessModule(unittest.TestCase):
    def setUp(self):
        self.data = sectionless_prx()
        self.elf = MI.Elf(self.data)

    def test_fixture_reads_as_a_module(self):
        self.assertEqual(self.elf.sections, [])
        mi = MI.module_info(self.elf)
        self.assertEqual(mi["name"], "synthetic")
        self.assertEqual([r["stub_table"] for r in MI.imports(self.elf, mi, False)], ["0x00000020"])

    def test_code_ends_before_the_module_tables(self):
        # the read-only tables share the R+X segment with the code; they are not code
        self.assertEqual(B.exec_ranges(self.elf), [(0x0, 0x20, "segment"), (0x20, 0x30, ".sceStub.text")])

    def test_function_list(self):
        with tempfile.TemporaryDirectory() as tmp:
            elf_path, out = Path(tmp) / "t.prx", Path(tmp) / "functions.txt"
            elf_path.write_bytes(self.data)
            argv = sys.argv
            sys.argv = ["lbpfuncs.py", "--elf", str(elf_path), "--out", str(out)]
            try:
                with open(Path(tmp) / "stdout", "w", encoding="utf-8") as sink:
                    stdout, sys.stdout = sys.stdout, sink
                    try:
                        F.main()
                    finally:
                        sys.stdout = stdout
            finally:
                sys.argv = argv
            # no function at the export table (0x34) or at the exported variable module_info (0x88); the stubs are
            # stubs, not C functions
            self.assertEqual(out.read_text(encoding="utf-8").splitlines(), ["0 1c auto", "20 28 stub", "28 30 stub"])

    def test_bootstrap_finds_the_stubs(self):
        self.assertEqual(lbpbootstrap.stub_ranges(self.data), [(0x20, 0x30)])

    def test_no_imports(self):
        # without stubs the code still ends at the first table the module info leads to
        data = bytearray(self.data)
        struct.pack_into("<5I", data, 0xA0 + 0x88 + 32, 0, 0x30, 0x40, 0x40, 0x40)   # stub_end = stub_top
        self.assertEqual(B.exec_ranges(MI.Elf(bytes(data))), [(0x0, 0x30, "segment")])


if __name__ == "__main__":
    unittest.main()
