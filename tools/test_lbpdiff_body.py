#!/usr/bin/env python3
"""Unit tests for reading our function's bytes out of the linked .text (lbpdiff.function_bytes, lbpcat, lbpdiff.show)."""
from __future__ import annotations

import io
import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpcat as C  # noqa: E402
import lbpdiff as D  # noqa: E402


def linked_elf(addr: int, code: bytes, symbols: list[tuple[str, int, int]]) -> bytes:
    """A little-endian ELF32 executable with one `.text` section at `addr` and FUNC symbols (name, value, size)."""
    names = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    strtab = b"\0"
    syms = [struct.pack("<IIIBBH", 0, 0, 0, 0, 0, 0)]
    for name, value, size in symbols:
        syms.append(struct.pack("<IIIBBH", len(strtab), value, size, 0x12, 0, 1))
        strtab += name.encode() + b"\0"
    symdata = b"".join(syms)
    blobs, sections, off = [], [(0,) * 10], 52
    for data, sec in ((code, (1, 1, 6, addr, 0, len(code), 0, 0, 4, 0)),
                      (symdata, (7, 2, 0, 0, 0, len(symdata), 3, 1, 4, 16)),
                      (strtab, (15, 3, 0, 0, 0, len(strtab), 0, 0, 1, 0)),
                      (names, (23, 3, 0, 0, 0, len(names), 0, 0, 1, 0))):
        blobs.append(data)
        sections.append(sec[:4] + (off,) + sec[5:])
        off += len(data)
    off += -off % 4
    body = b"".join(blobs)
    body += b"\0" * (off - 52 - len(body))
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + b"\0" * 9, 2, 8, 1, addr, 0, off, 0, 52, 0, 0, 40,
                       len(sections), len(sections) - 1)
    return ehdr + body + b"".join(struct.pack("<10I", *s) for s in sections)


def le(*ws: int) -> bytes:
    return b"".join(w.to_bytes(4, "little") for w in ws)


HELPER = le(0x03E00008, 0x24020001)                 # jr ra; li v0,1 (a static helper the object defines first)
TARGET = le(0x27BDFFF0, 0x03E00008, 0x27BD0010)     # addiu sp,sp,-16; jr ra; addiu sp,sp,16
AFTER = le(0x03E00008, 0x00000000)                  # the next function of the object
START = 0x08804008                                  # the target's retail address; .text starts 8 bytes before it


class FunctionBytesTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.out = Path(self.tmp.name)
        self.addCleanup(self.tmp.cleanup)

    def write(self, code: bytes, symbols: list[tuple[str, int, int]]) -> Path:
        (self.out / "linked.elf").write_bytes(linked_elf(START - 8, code, symbols))
        (self.out / "text.bin").write_bytes(code)
        return self.out / "text.bin"

    def test_the_function_is_read_from_its_own_address_not_from_the_start_of_text(self) -> None:
        text = self.write(HELPER + TARGET + AFTER, [("helper", START - 8, 8), ("target", START, 12), ("after", START + 12, 8)])
        self.assertEqual(D.function_bytes(text, START, len(TARGET)), TARGET)

    def test_a_function_longer_than_the_header_span_keeps_its_own_size(self) -> None:
        text = self.write(HELPER + TARGET + AFTER, [("helper", START - 8, 8), ("target", START, 12)])
        self.assertEqual(D.function_bytes(text, START, 8), TARGET)

    def test_a_shorter_function_is_compared_over_the_header_span(self) -> None:
        text = self.write(HELPER + TARGET + AFTER, [("target", START, 12)])
        self.assertEqual(D.function_bytes(text, START, 16), TARGET + AFTER[:4])

    def test_a_file_that_is_not_the_linked_text_is_left_alone(self) -> None:
        self.write(HELPER + TARGET, [("target", START, 12)])
        helper = self.out / "helper-x.bin"
        helper.write_bytes(TARGET)
        self.assertEqual(D.function_bytes(helper, START, 12), TARGET)

    def test_without_a_linked_elf_the_file_is_returned_as_it_is(self) -> None:
        (self.out / "text.bin").write_bytes(HELPER + TARGET)
        self.assertEqual(D.function_bytes(self.out / "text.bin", START, 12), HELPER + TARGET)


class ShowTests(unittest.TestCase):
    def test_the_listing_shows_our_function_beside_the_retail_one(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp)
            (out / "linked.elf").write_bytes(linked_elf(START - 8, HELPER + TARGET, [("helper", START - 8, 8),
                                                                                      ("target", START, 12)]))
            (out / "text.bin").write_bytes(HELPER + TARGET)
            (out / "target.bin").write_bytes(TARGET)
            seen = {}

            def fake_disasm(path: Path, vma: int) -> list[str]:
                data = path.read_bytes()
                seen[path.name] = data
                return [f"{int.from_bytes(data[i:i + 4], 'little'):08x} op" for i in range(0, len(data), 4)]

            with mock.patch.object(D, "disasm", side_effect=fake_disasm), mock.patch("sys.stdout", new_callable=io.StringIO) as so:
                D.show("t.c", out / "text.bin", out / "target.bin", START)
            self.assertNotIn("**", so.getvalue())
            self.assertEqual(len(so.getvalue().splitlines()), 4)
            self.assertIn(TARGET, seen.values())


class ClassifyLinkedTextTests(unittest.TestCase):
    def test_a_function_after_a_helper_is_classified_from_its_own_bytes(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            gen = root / "try" / "gen-x"
            gen.mkdir(parents=True)
            src = "try/gen-x/v0/f.c"
            (gen / "summary-0.json").write_text(json.dumps(
                [{"source": src, "start": f"0x{START:08x}", "status": "MISMATCH"}]), encoding="utf-8")
            out = root / "build" / "MOD" / C.M.build_key(root / src)
            out.mkdir(parents=True)
            ours = le(0x27BDFFF0, 0x03E00008, 0x27BD0010)
            theirs = le(0x27BDFFE0, 0x03E00008, 0x27BD0020)  # a different frame size: class `frame`
            (out / "linked.elf").write_bytes(linked_elf(START - 8, HELPER + ours, [("target", START, 12)]))
            (out / "text.bin").write_bytes(HELPER + ours)
            (out / "target.bin").write_bytes(theirs)
            argv = ["lbpcat.py", str(gen), "--module", "MOD"]
            with mock.patch.object(C, "ROOT", root), mock.patch.object(sys, "argv", argv), \
                    mock.patch("sys.stdout", new_callable=io.StringIO) as so:
                self.assertEqual(C.main(), 0)
            self.assertRegex(so.getvalue(), r"frame\s+1 ")


def w(op: int, rs: int = 0, rt: int = 0, rd: int = 0, sa: int = 0, fn: int = 0) -> int:
    return op << 26 | rs << 21 | rt << 16 | rd << 11 | sa << 6 | fn


class AllegrexOpcodeTests(unittest.TestCase):
    """Two different operations that share a major opcode are not a register-allocation difference."""

    PAIRS = {
        "add.s vs mul.s": (w(0x11, 0x10, 2, 1, 0, 0x00), w(0x11, 0x10, 2, 1, 0, 0x02)),
        "c.lt.s vs c.le.s": (w(0x11, 0x10, 2, 1, 0, 0x3C), w(0x11, 0x10, 2, 1, 0, 0x3E)),
        "mfc1 vs mtc1": (w(0x11, 0x00, 4, 12), w(0x11, 0x04, 4, 12)),
        "srl vs rotr": (w(0, 0, 4, 2, 3, 0x02), w(0, 1, 4, 2, 3, 0x02)),
        "srlv vs rotrv": (w(0, 5, 4, 2, 0, 0x06), w(0, 5, 4, 2, 1, 0x06)),
        "seb vs seh": (w(0x1F, 0, 4, 2, 0x10, 0x20), w(0x1F, 0, 4, 2, 0x18, 0x20)),
        "wsbh vs bitrev": (w(0x1F, 0, 4, 2, 0x02, 0x20), w(0x1F, 0, 4, 2, 0x14, 0x20)),
        "ext vs ins": (w(0x1F, 4, 2, 7, 3, 0x00), w(0x1F, 4, 2, 7, 3, 0x04)),
        "mfic vs mtic": (w(0x1C, 0, 4, 0, 0, 0x24), w(0x1C, 0, 4, 0, 0, 0x26)),
        "cache 0x14 vs 0x1b": (w(0x2F, 4, 0x14), w(0x2F, 4, 0x1B)),
        "vadd.q vs vsub.q": (0x60008080 | 0x020100, 0x60808080 | 0x020100),
        "vadd.s vs vadd.q": (0x60000000 | 0x020100, 0x60008080 | 0x020100),
        "vcmp.s EQ vs LT": (0x6C000000 | 0x020100 | 1, 0x6C000000 | 0x020100 | 4),
        "vmov.q vs vabs.q": (0xD0008080 | 0x0100, 0xD0018080 | 0x0100),
        "vcst 1 vs 2": (0xD0610000, 0xD0620000),
        "vi2f vs vf2in": (0xD2800000 | 0x0100, 0xD2000000 | 0x0100),
        "vpfxs xyzw vs wzyx": (0xDC0000E4, 0xDC00001B),
        "vmmul vs vmmov": (0xF0008080 | 0x0100, 0xF3808080 | 0x0100),
        "mfv vs mfvc": (0x48600000 | 4 << 16 | 0x01, 0x48600000 | 4 << 16 | 0x81),
        "lvl.q vs lvr.q": (0xD4000000 | 4 << 21, 0xD4000002 | 4 << 21),
    }

    def test_each_pair_has_different_keys_and_is_not_regalloc(self) -> None:
        for name, (a, b) in self.PAIRS.items():
            with self.subTest(name):
                self.assertNotEqual(C.opcode_key(a), C.opcode_key(b))
                self.assertNotEqual(C.classify([a], [b]), "regalloc")

    def test_register_only_differences_are_still_regalloc(self) -> None:
        same_op = {
            "add.s fd": (w(0x11, 0x10, 2, 1, 0, 0x00), w(0x11, 0x10, 2, 1, 3, 0x00)),
            "seb rd": (w(0x1F, 0, 4, 2, 0x10, 0x20), w(0x1F, 0, 4, 3, 0x10, 0x20)),
            "rotr rd": (w(0, 1, 4, 2, 3, 0x02), w(0, 1, 4, 5, 3, 0x02)),
            "vadd.q vd": (0x60008080 | 0x020100, 0x60008080 | 0x020104),
            "lv.q vt": (0xD8000000 | 29 << 21 | 1 << 16, 0xD8000000 | 29 << 21 | 2 << 16),
        }
        for name, (a, b) in same_op.items():
            with self.subTest(name):
                self.assertEqual(C.classify([a], [b]), "regalloc")

    def test_regimm_and_fpu_branch_likely_forms_are_likely(self) -> None:
        pairs = {
            "bltz vs bltzl": (w(1, 4, 0x00) | 3, w(1, 4, 0x02) | 3),
            "bgezal vs bgezall": (w(1, 4, 0x11) | 3, w(1, 4, 0x13) | 3),
            "bc1t vs bc1tl": (w(0x11, 8, 1) | 3, w(0x11, 8, 3) | 3),
            "bvf vs bvfl": (w(0x12, 8, 0) | 3, w(0x12, 8, 2) | 3),
        }
        for name, (a, b) in pairs.items():
            with self.subTest(name):
                self.assertEqual(C.classify([a, 0], [b, 0]), "likely")

    def test_bc1f_vs_bc1t_is_not_a_likely_pair(self) -> None:
        self.assertNotEqual(C.classify([w(0x11, 8, 0) | 3], [w(0x11, 8, 1) | 3]), "likely")


if __name__ == "__main__":
    unittest.main()
