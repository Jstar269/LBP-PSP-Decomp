#!/usr/bin/env python3
"""Tests for tools/lbpsigscan.py: which functions of a compiled object it compares. The object is a synthetic ELF
relocatable built here byte by byte (no game data, no compiler)."""
from __future__ import annotations

import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpsigscan as S  # noqa: E402

JR_RA, NOP = 0x03E00008, 0


def relocatable(code_sections: list[tuple[str, list[tuple[str, list[int]]]]]) -> bytes:
    """An ELF32 little-endian MIPS relocatable: one SHT_PROGBITS section per entry of `code_sections` (name, then
    its functions as (symbol, words)), each function an STT_FUNC symbol of that section; then .symtab, .strtab and
    .shstrtab."""
    shstr, strtab = bytearray(b"\0"), bytearray(b"\0")
    blobs, shdrs, syms = [], [bytes(40)], [bytes(16)]
    pos = 52

    def name_in(table: bytearray, name: str) -> int:
        off = len(table)
        table += name.encode() + b"\0"
        return off
    for index, (sec_name, funcs) in enumerate(code_sections, 1):
        data = b""
        for sym, ws in funcs:
            body = b"".join(struct.pack("<I", w) for w in ws)
            syms.append(struct.pack("<IIIBBH", name_in(strtab, sym), len(data), len(body), 0x12, 0, index))
            data += body
        shdrs.append(struct.pack("<10I", name_in(shstr, sec_name), 1, 6, 0, pos, len(data), 0, 0, 4, 0))
        blobs.append(data)
        pos += len(data)
    n = len(code_sections)
    symtab = b"".join(syms)
    for sec_name, kind, blob, link in ((".symtab", 2, symtab, n + 2), (".strtab", 3, bytes(strtab), 0)):
        shdrs.append(struct.pack("<10I", name_in(shstr, sec_name), kind, 0, 0, pos, len(blob), link,
                                 1 if kind == 2 else 0, 4, 16 if kind == 2 else 0))
        blobs.append(blob)
        pos += len(blob)
    shstr_name = name_in(shstr, ".shstrtab")
    shdrs.append(struct.pack("<10I", shstr_name, 3, 0, 0, pos, len(shstr), 0, 0, 1, 0))
    blobs.append(bytes(shstr))
    pos += len(shstr)
    pos += -pos % 4
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9), 1, 8, 1, 0, 0, pos, 0x10A23000,
                       52, 0, 0, 40, len(shdrs), len(shdrs) - 1)
    out = ehdr + b"".join(blobs)
    return out + bytes(pos - len(out)) + b"".join(shdrs)


class ObjectFunctions(unittest.TestCase):
    def functions(self, obj: bytes) -> list[str]:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "a.o"
            path.write_bytes(obj)
            return [name for name, _ws in S.object_functions(path)]

    def test_text_and_linkonce_functions(self):
        obj = relocatable([(".text", [("plain", [0x27BDFFF0, JR_RA, 0x27BD0010])]),
                           (".gnu.linkonce.t._ZN1A3getEv", [("_ZN1A3getEv", [0x8C820000, JR_RA, NOP])]),
                           (".text._Z4coldv", [("_Z4coldv", [0x24020001, JR_RA, NOP])])])
        self.assertEqual(sorted(self.functions(obj)), ["_Z4coldv", "_ZN1A3getEv", "plain"])

    def test_words_of_a_linkonce_function(self):
        obj = relocatable([(".text", [("plain", [0x27BDFFF0, JR_RA, 0x27BD0010])]),
                           (".gnu.linkonce.t._ZN1A3getEv", [("_ZN1A3getEv", [0x8C820000, JR_RA, NOP])])])
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "a.o"
            path.write_bytes(obj)
            got = dict(S.object_functions(path))
        self.assertEqual(got["_ZN1A3getEv"], [0x8C820000, JR_RA, NOP])


if __name__ == "__main__":
    unittest.main()
