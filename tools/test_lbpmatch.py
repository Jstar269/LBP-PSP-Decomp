#!/usr/bin/env python3
"""Unit checks of tools/lbpmatch.py's same-file helper support (helpers=) that need no toolchain: the header syntax,
the section rename of the placed object copy, and the function-to-section mapping. The end-to-end placement (compile,
link, compare, promotion, refusal of a mismatching helper) is in tools/test_lbppromote.py (HelperPlacement).

    python tools/test_lbpmatch.py [-v]
"""
from __future__ import annotations

import struct
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402


def tiny_object(funcs: list[tuple[str, bytes]], extra_in_first: str | None = None) -> bytes:
    """A little-endian ELF32 relocatable object shaped like mwccpsp output: one `.text` section per function, a symtab
    with one FUNC symbol at offset 0 of each (and optionally a second FUNC symbol in the first section)."""
    names = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    n_text, n_sym, n_str, n_shs = 1, 7, 15, 23
    strtab = b"\0"
    syms = [struct.pack("<IIIBBH", 0, 0, 0, 0, 0, 0)]
    blobs, sections = [], [(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)]
    off = 52
    for i, (name, code) in enumerate(funcs):
        blobs.append(code)
        sections.append((n_text, 1, 6, 0, off, len(code), 0, 0, 4, 0))
        off += len(code)
        syms.append(struct.pack("<IIIBBH", len(strtab), 0, len(code), 0x12, 0, i + 1))
        strtab += name.encode() + b"\0"
    if extra_in_first:
        syms.append(struct.pack("<IIIBBH", len(strtab), 4, 4, 0x12, 0, 1))
        strtab += extra_in_first.encode() + b"\0"
    symdata = b"".join(syms)
    for data, sec in ((symdata, (n_sym, 2, 0, 0, 0, len(symdata), len(funcs) + 2, 1, 4, 16)),
                      (strtab, (n_str, 3, 0, 0, 0, len(strtab), 0, 0, 1, 0)),
                      (names, (n_shs, 3, 0, 0, 0, len(names), 0, 0, 1, 0))):
        blobs.append(data)
        sections.append(sec[:4] + (off,) + sec[5:])
        off += len(data)
    off += -off % 4
    body = b"".join(blobs)
    body += b"\0" * (off - 52 - len(body))
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + b"\0" * 9, 1, 8, 1, 0, 0, off, 0, 52, 0, 0, 40,
                       len(sections), len(sections) - 1)
    return ehdr + body + b"".join(struct.pack("<10I", *s) for s in sections)


class HelperHeader(unittest.TestCase):
    def header(self, extra: str) -> dict:
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "f.c"
            p.write_text(f"/* LBPMATCH module=EBOOT.BIN start=0x00291434 end=0x002914c0 symbol=FUN_00291434 "
                         f"lang=c99 {extra} */\n", encoding="utf-8")
            return M.parse_header(p)

    def test_parsed_and_ordered_target_first(self) -> None:
        h = self.header("helpers=Log@0x0028fb44-0x0028fb70,FUN_0028fb70@0x0028fb70-0x0028fb80")
        self.assertEqual(h["helpers"], [("Log", 0x28fb44, 0x28fb70), ("FUN_0028fb70", 0x28fb70, 0x28fb80)])
        self.assertEqual(M.placed_functions(h)[0], ("FUN_00291434", 0x291434, 0x2914c0))
        self.assertEqual(M.placed_functions(self.header("")), [("FUN_00291434", 0x291434, 0x2914c0)])

    def test_bad_entries_are_rejected(self) -> None:
        for bad in ("helpers=Log", "helpers=Log@0x0028fb44", "helpers=Log@0x0028fb46-0x0028fb70",
                    "helpers=Log@0x0028fb70-0x0028fb44",
                    "helpers=Log@0x00291430-0x00291440",                                   # overlaps the target
                    "helpers=A@0x0028fb44-0x0028fb70,B@0x0028fb60-0x0028fb80",              # helpers overlap
                    "helpers=FUN_00291434@0x0028fb44-0x0028fb70",                           # the target's own name
                    "helpers=A@0x0028fb44-0x0028fb70,A@0x0028fb70-0x0028fb80"):             # a name twice
            with self.subTest(bad=bad), self.assertRaises(ValueError):
                self.header(bad)


class SectionPlacement(unittest.TestCase):
    def test_rename_changes_only_section_names(self) -> None:
        obj = tiny_object([("Log", b"\x08\x00\xe0\x03" b"\0\0\0\0"), ("Target", b"\x01\x02\x03\x04")])
        out = M.rename_sections(obj, {1: ".lbp_fn0", 2: ".lbp_fn1"})
        before, after = M.elf_sections(obj), M.elf_sections(out)
        self.assertEqual([n for n, _ in before][1:3], [".text", ".text"])
        self.assertEqual([n for n, _ in after][1:3], [".lbp_fn0", ".lbp_fn1"])
        self.assertEqual([n for n, _ in after][3:], [n for n, _ in before][3:])
        for (_, a), (_, b) in zip(before, after):
            if a[1] != 3:   # every section but the string tables: same header fields apart from the name
                self.assertEqual(a[1:], b[1:])
                self.assertEqual(obj[a[4]:a[4] + a[5]], out[b[4]:b[4] + b[5]])
        self.assertEqual(list(M.elf_symbols(obj)), list(M.elf_symbols(out)))
        shoff = struct.unpack_from("<I", obj, 0x20)[0]
        self.assertEqual(out[:shoff], obj[:shoff])   # every byte before the section headers is untouched (the new name
                                                     # table is appended after them)

    def test_function_sections(self) -> None:
        obj = tiny_object([("Log", b"\0" * 8), ("Target", b"\0" * 4)])
        got, why = M.function_sections(obj, [("Target", 0x100, 0x104), ("Log", 0x40, 0x48)])
        self.assertEqual((got, why), ({2: ("Target", 0x100, 0x104), 1: ("Log", 0x40, 0x48)}, ""))

    def test_function_sections_refusals(self) -> None:
        obj = tiny_object([("Log", b"\0" * 8), ("Target", b"\0" * 4)])
        _, why = M.function_sections(obj, [("Target", 0x100, 0x104)])
        self.assertIn("Log defined but not placed", why)                 # every function must be placed
        _, why = M.function_sections(obj, [("Target", 0x100, 0x104), ("Missing", 0x40, 0x48)])
        self.assertIn("Missing is not defined", why)
        shared = tiny_object([("Log", b"\0" * 8), ("Target", b"\0" * 4)], extra_in_first="Other")
        _, why = M.function_sections(shared, [("Target", 0x100, 0x104), ("Log", 0x40, 0x48)])
        self.assertIn("shares its .text section with Other", why)        # one function per section only

    def test_word_diffs(self) -> None:
        self.assertEqual(M.word_diffs(b"\0" * 8, b"\0" * 8), [])
        self.assertEqual(M.word_diffs(b"\0" * 8, b"\0\0\0\0\1\0\0\0"), [1])
        self.assertEqual(M.word_diffs(b"\0" * 4, b"\0" * 8), [1])          # a shorter body differs in the rest


class LinkonceData(unittest.TestCase):
    @staticmethod
    def sec(sh_type: int, size: int) -> tuple:
        return (0, sh_type, 3, 0, 0, size, 0, 0, 4, 0)

    def test_placed_at_symbol_address(self) -> None:
        sections = {".text": self.sec(1, 8), ".bss": self.sec(8, 0),
                    ".gnu.linkonce.b._ZZ9game_heapvE4heap": self.sec(8, 4),
                    ".gnu.linkonce.d._ZN1AILi0EE1vE": self.sec(1, 4),
                    ".gnu.linkonce.t._Z9game_heapv": self.sec(1, 12)}
        syms = {"_ZZ9game_heapvE4heap": 0x08be84a0, "_ZN1AILi0EE1vE": 0x08bd0000}
        got, why = M.linkonce_placements(sections, None, syms)
        self.assertEqual(why, "")
        self.assertEqual(got, [(".gnu.linkonce.b._ZZ9game_heapvE4heap", 0x08be84a0, False),
                               (".gnu.linkonce.d._ZN1AILi0EE1vE", 0x08bd0000, True)])

    def test_only_referenced_and_nonempty(self) -> None:
        sections = {".gnu.linkonce.b.x": self.sec(8, 4), ".gnu.linkonce.b.y": self.sec(8, 4),
                    ".gnu.linkonce.b.z": self.sec(8, 0)}
        got, why = M.linkonce_placements(sections, {".gnu.linkonce.b.x", ".gnu.linkonce.b.z"}, {"x": 16})
        self.assertEqual((got, why), ([(".gnu.linkonce.b.x", 16, False)], ""))

    def test_missing_symbol_refused(self) -> None:
        got, why = M.linkonce_placements({".gnu.linkonce.sb.q": self.sec(8, 4)}, None, {})
        self.assertEqual(got, [])
        self.assertIn("'q = 0x...;'", why)


def named_object(secs: list[tuple[str, int, int, int]], syms: list[tuple[str, int, int]]) -> bytes:
    """A little-endian ELF32 relocatable object with the given sections (name, sh_type, size, align; contents zero)
    and FUNC symbols (name, 1-based section number, value). Shaped like GCC 4.0 C++ output for the layout checks."""
    shstr = b"\0"
    name_off = []
    for name, *_ in secs:
        name_off.append(len(shstr))
        shstr += name.encode() + b"\0"
    tail = {}
    for n in (".symtab", ".strtab", ".shstrtab"):
        tail[n] = len(shstr)
        shstr += n.encode() + b"\0"
    strtab = b"\0"
    symdata = struct.pack("<IIIBBH", 0, 0, 0, 0, 0, 0)
    for name, shndx, value in syms:
        symdata += struct.pack("<IIIBBH", len(strtab), value, 4, 0x12, 0, shndx)
        strtab += name.encode() + b"\0"
    headers, blobs, off = [(0,) * 10], [], 52
    for (name, sh_type, size, align), no in zip(secs, name_off):
        data = b"" if sh_type == 8 else b"\0" * size
        headers.append((no, sh_type, 6, 0, off, size, 0, 0, align, 0))
        blobs.append(data)
        off += len(data)
    for name, data, sh_type, link, info, ent in ((".symtab", symdata, 2, len(secs) + 2, 1, 16),
                                                 (".strtab", strtab, 3, 0, 0, 0), (".shstrtab", shstr, 3, 0, 0, 0)):
        headers.append((tail[name], sh_type, 0, 0, off, len(data), link, info, 4 if ent else 1, ent))
        blobs.append(data)
        off += len(data)
    pad = -off % 4
    ehdr = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + b"\0" * 9, 1, 8, 1, 0, 0, off + pad, 0, 52, 0, 0,
                       40, len(headers), len(headers) - 1)
    return ehdr + b"".join(blobs) + b"\0" * pad + b"".join(struct.pack("<10I", *h) for h in headers)


class LinkonceText(unittest.TestCase):
    """A GCC C++ thunk with its inline destructor in .gnu.linkonce.t.<D1>: where ld's default script lays it out."""
    THUNK = "_ZThn12_N3ObjD1Ev"

    def obj(self, *, early: bool = False, twice: bool = False) -> bytes:
        secs = [(".text", 1, 0x14, 4), (".data", 1, 0, 1), (".bss", 8, 0, 1),
                (".gnu.linkonce.r._ZTV3Obj", 1, 0x58, 8),            # data: not a code input
                (".gnu.linkonce.t._ZN3MemD1Ev", 1, 0x10, 4),
                (".gnu.linkonce.t._ZN3ObjD1Ev", 1, 0x9c, 16),        # aligned: 0x14 + 0x10 -> 0x30
                (".gnu.linkonce.t._ZN3ObjD0Ev", 1, 0xa4, 4)]
        if early:
            secs.append((".text.unlikely", 1, 8, 4))
        syms = [("_ZN3Obj2s2Ev", 1, 0), ("_ZN3MemD1Ev", 5, 0), (self.THUNK, 6, 0), ("_ZN3ObjD1Ev", 6, 12),
                ("_ZThn12_N3ObjD0Ev", 7, 0)]
        if twice:
            syms.append((self.THUNK, 7, 4))
        return named_object(secs, syms)

    def test_offset_follows_file_order_and_alignment(self) -> None:
        o = self.obj()
        self.assertEqual(M.linkonce_text_offset(o, self.THUNK), 0x30)
        self.assertEqual(M.linkonce_text_offset(o, "_ZN3ObjD1Ev"), 0x3c)
        self.assertEqual(M.linkonce_text_offset(o, "_ZThn12_N3ObjD0Ev"), 0x30 + 0x9c)

    def test_refusals(self) -> None:
        o = self.obj()
        self.assertIsNone(M.linkonce_text_offset(o, "_ZN3Obj2s2Ev"))         # in .text: the old path places it
        self.assertIsNone(M.linkonce_text_offset(o, "_ZN3Missing"))
        self.assertIsNone(M.linkonce_text_offset(self.obj(twice=True), self.THUNK))     # defined twice
        self.assertIsNone(M.linkonce_text_offset(self.obj(early=True), self.THUNK))     # a layout it does not model


class CxxVtable(unittest.TestCase):
    def test_only_vtables_of_gcc_cxx_sources(self) -> None:
        gcc = {"lang": "c++", "cc": "gcc404xsg-Os"}
        self.assertTrue(M.is_cxx_vtable_section(".gnu.linkonce.r._ZTV12Obj_0886c09c", gcc))
        self.assertTrue(M.is_cxx_vtable_section(".gnu.linkonce.r._ZTV3Obj", {"lang": "c++"}))     # primary profile
        for name in (".gnu.linkonce.r._ZTI3Obj", ".gnu.linkonce.r._ZTS3Obj", ".gnu.linkonce.d._ZTV3Obj",
                     ".gnu.linkonce.b._ZZ9game_heapvE4heap", ".rodata", ".gnu.linkonce.r._ZTV"):
            self.assertFalse(M.is_cxx_vtable_section(name, gcc), name)
        for h in ({"lang": "c99", "cc": "gcc404xs-Os"}, {"lang": "c++", "cc": "mwcc219"}, {"lang": "c++", "cc": "snc127-O2"}):
            self.assertFalse(M.is_cxx_vtable_section(".gnu.linkonce.r._ZTV3Obj", h), h)

    def test_profile_gcc404xsg_is_gcc404xs_with_assembler_g(self) -> None:
        with patch.object(M, "_gcc_profiles", None), patch.object(M, "gcc_home", return_value="/fixture/gcc"), \
             patch.object(M, "gcc4_home", return_value="/fixture/gcc4"):
            prof = M.gcc_profiles()
        for o in ("O0", "O1", "O2", "O3", "Os"):
            self.assertEqual(prof[f"gcc404xsg-{o}"]["flags"], prof[f"gcc404xs-{o}"]["flags"] + " -Wa,-g")
            self.assertEqual(prof[f"gcc404xsg-{o}"]["driver"], prof[f"gcc404xs-{o}"]["driver"])


class BuildKey(unittest.TestCase):
    def test_same_name_in_different_trees_gets_different_keys(self) -> None:
        # the collision: try/gen-a1/v0/FUN_x.c and try/gen-a2/v0/FUN_x.c shared build/<module>/v0__FUN_x
        a = M.ROOT / "try" / "gen-a1" / "v0" / "FUN_08804000.c"
        b = M.ROOT / "try" / "gen-a2" / "v0" / "FUN_08804000.c"
        self.assertNotEqual(M.build_key(a), M.build_key(b))
        self.assertTrue(M.build_key(a).startswith("v0__FUN_08804000__"))

    def test_key_is_stable_and_relative_to_the_checkout(self) -> None:
        p = M.ROOT / "src" / "eboot" / "FUN_08804000.c"
        self.assertEqual(M.build_key(p), M.build_key(Path(str(p))))
        # inside the checkout the digest is of the relative path, so a lane (its own ROOT) keys the file the same way
        ident = __import__("os").path.normcase("src/eboot/FUN_08804000.c")   # case-folded (and \ on Windows)
        digest = __import__("hashlib").sha1(ident.encode("utf-8")).hexdigest()[:10]
        self.assertEqual(M.build_key(p), f"eboot__FUN_08804000__{digest}")

    def test_outside_the_checkout_the_absolute_path_decides(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            a, b = Path(d) / "x" / "v0" / "f.c", Path(d) / "y" / "v0" / "f.c"
            self.assertNotEqual(M.build_key(a), M.build_key(b))

    def test_no_tool_builds_its_own_key(self) -> None:
        for f in sorted(Path(M.__file__).resolve().parent.glob("lbp*.py")):
            if f.name == "lbpxverify.py":
                continue    # the second verifier must not import the matcher; test_lbpxverify checks its key equals ours
            text = f.read_text(encoding="utf-8")
            if f.name == "lbpmatch.py":
                text = text.replace('return f"{p.parent.name}__{p.stem}__{digest}"', "")
            self.assertNotRegex(text, r"parent\.name\}__\{", f"{f.name} builds a build-dir key; use lbpmatch.build_key")


if __name__ == "__main__":
    unittest.main()
