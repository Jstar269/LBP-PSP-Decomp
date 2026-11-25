#!/usr/bin/env python3
"""Synthetic tests for lbpnids.py (import stub names from NIDs): a hand-built PRX, no game data."""
from __future__ import annotations

import contextlib
import hashlib
import io
import json
import re
import struct
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402
import lbpnids as N  # noqa: E402


def nid(name: str) -> int:
    return int.from_bytes(hashlib.sha1(name.encode("ascii")).digest()[:4], "little")


def prx(stub_entries: list[tuple[int, list[int]]]) -> bytes:
    """A section-less PRX (one PT_LOAD at 0) whose .lib.stub holds `stub_entries` [(stub table address, [NIDs])], all
    of the library "TestLib"."""
    seg = bytearray(0x200)
    seg[0x100:0x108] = b"TestLib\0"
    stub_top = 0x40
    a = stub_top
    nid_at = 0x140
    for stub_table, nids in stub_entries:
        struct.pack_into("<IHHBBHII", seg, a, 0x100, 0x11, 0x9, 5, 0, len(nids), nid_at, stub_table)
        for i, n in enumerate(nids):
            struct.pack_into("<I", seg, nid_at + 4 * i, n)
        nid_at += 4 * len(nids)
        a += 20
    struct.pack_into("<HBB28s5I", seg, 0x180, 0, 1, 1, b"synthetic", 0, 0x30, 0x30, stub_top, a)
    off = 0x60
    eh = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9), 0xFFA0, 8, 1, 0, 52, 0, 0x10A23001, 52,
                     32, 1, 40, 0, 0)
    ph = struct.pack("<8I", 1, off, 0, off + 0x180, len(seg), len(seg), 7, 16)
    out = bytearray(off + len(seg))
    out[:52] = eh
    out[52:84] = ph
    out[off:] = seg
    return bytes(out)


class AppendTests(unittest.TestCase):
    def run_append(self, image: bytes, names: list[str], existing: str = "") -> str:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "symbols").mkdir()
            sym = root / "symbols" / "TEST.PRX.txt"
            sym.write_text(existing, encoding="utf-8")
            corpus = root / "corpus.json"
            corpus.write_text(json.dumps({"derivable_classes": ["d"],
                                          "entries": [{"name": n, "class": "d"} for n in names]}), encoding="utf-8")
            with mock.patch.object(M, "ROOT", root), \
                    mock.patch.object(M, "module_bytes", lambda _c, _m: (image, {})), \
                    mock.patch.object(sys, "argv", ["lbpnids.py", "--module", "TEST.PRX", "--corpus", str(corpus),
                                                    "--append"]), \
                    contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(N.main(), 0)
                M.load_symbols("TEST.PRX")          # the map must still load: no name bound to two addresses
            return sym.read_text(encoding="utf-8")

    def test_stubs_are_named_at_their_addresses(self) -> None:
        text = self.run_append(prx([(0x20, [nid("sceFoo"), nid("sceBar"), 0x12345678])]), ["sceFoo", "sceBar"])
        self.assertIn("sceFoo = 0x00000020;", text)
        self.assertIn("sceBar = 0x00000028;", text)
        self.assertEqual(len(text.splitlines()), 2)

    def test_a_function_imported_by_two_stub_entries_is_named_once(self) -> None:
        # the same library imported by two stub entries (two stub objects linked), both with sceFoo: a second line
        # `sceFoo = <other stub>` would make symbols/<module>.txt bind one name to two addresses, which every tool
        # that loads the map refuses
        text = self.run_append(prx([(0x20, [nid("sceFoo")]), (0x30, [nid("sceFoo"), nid("sceBar")])]),
                               ["sceFoo", "sceBar"])
        bound = re.findall(r"^(\S+) = (0x[0-9a-f]+);", text, re.M)
        self.assertEqual(sorted(bound), [("sceBar", "0x00000038"), ("sceFoo", "0x00000020")])

    def test_names_already_in_the_map_are_skipped(self) -> None:
        text = self.run_append(prx([(0x20, [nid("sceFoo")])]), ["sceFoo"], "sceFoo = 0x00000020;\n")
        self.assertEqual(text, "sceFoo = 0x00000020;\n")


if __name__ == "__main__":
    unittest.main()
