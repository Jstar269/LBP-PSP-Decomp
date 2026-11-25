#!/usr/bin/env python3
"""Tests for the cross-title SDK fingerprint catalogue (lbpfingerprint.py). Synthetic code only, no game data."""
from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpclone as C  # noqa: E402
import lbpfingerprint as F  # noqa: E402


def i_type(op: int, rs: int, rt: int, imm: int) -> int:
    return (op << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)


def jal(target: int) -> int:
    return (3 << 26) | ((target >> 2) & 0x03FFFFFF)


SP, RA, A0, A1, V0 = 29, 31, 4, 5, 2
JR_RA = (RA << 21) | 0x08
NOP = 0


def function(hi: int, lo: int, callee: int, frame: int = 16) -> list[int]:
    """A small SDK-style leaf caller: frame, a global's address built with lui/addiu, a call, a load, a branch."""
    return [
        i_type(0x09, SP, SP, -frame),          # addiu sp, sp, -frame
        i_type(0x2B, SP, RA, frame - 4),       # sw    ra, frame-4(sp)
        i_type(0x0F, 0, A0, hi),               # lui   a0, %hi(global)
        i_type(0x09, A0, A0, lo),              # addiu a0, a0, %lo(global)
        jal(callee),                           # jal   callee
        NOP,
        i_type(0x23, V0, A1, 0x10),            # lw    a1, 16(v0)
        i_type(0x04, A1, 0, 2),                # beq   a1, zero, +2
        NOP,
        i_type(0x23, SP, RA, frame - 4),       # lw    ra, frame-4(sp)
        JR_RA,                                 # jr    ra
        i_type(0x09, SP, SP, frame),           # addiu sp, sp, frame
    ]


def code(ws: list[int]) -> bytes:
    return b"".join(w.to_bytes(4, "little") for w in ws)


# Two "objects" holding the same function linked at different addresses: only the relocated fields differ.
OBJECT_A = function(hi=0x0012, lo=0x3450, callee=0x00089ABC)
OBJECT_B = function(hi=0x0034, lo=-0x10, callee=0x0001F000)
# A third with one changed instruction (a larger frame slot for ra).
OBJECT_C = list(OBJECT_A)
OBJECT_C[1] = i_type(0x2B, SP, RA, 8)
# The same fingerprint is pinned in hst-decomp's tools/test_hstfingerprint.py: both projects mask alike.
GOLDEN_A = (12, "103002ac21bca9384839a164bfc4369914e9697de6e5a4e58a5b5ce566f45207")


class AcceptanceTests(unittest.TestCase):
    """Issue #7: equal fingerprints for code that differs only in relocated addresses, none for a changed insn."""

    def test_relocated_copies_have_equal_fingerprints(self) -> None:
        self.assertNotEqual(code(OBJECT_A), code(OBJECT_B))
        self.assertEqual(F.fingerprint(code(OBJECT_A)), F.fingerprint(code(OBJECT_B)))

    def test_one_changed_instruction_does_not_match(self) -> None:
        self.assertEqual(F.fingerprint(code(OBJECT_A))[0], F.fingerprint(code(OBJECT_C))[0])
        self.assertNotEqual(F.fingerprint(code(OBJECT_A)), F.fingerprint(code(OBJECT_C)))

    def test_fingerprint_is_the_in_title_clone_signature(self) -> None:
        sig, _refs = C.analyse(OBJECT_A)
        self.assertEqual(F.fingerprint(code(OBJECT_A)), (len(sig), F.signature_sha256(sig)))

    def test_golden_fingerprint_shared_with_lbp(self) -> None:
        self.assertEqual(F.fingerprint(code(OBJECT_A)), GOLDEN_A)


def images() -> dict[str, tuple[int, bytes]]:
    """Synthetic module images: name -> (base address, bytes)."""
    return {"EBOOT.BIN": (0x00010000, code([NOP] * 4 + OBJECT_A + OBJECT_C))}


def code_of(module: str, start: int, end: int) -> bytes:
    base, data = images()[module]
    return data[start - base:end - base]


PROOFS = [
    {"module": "EBOOT.BIN", "start": "0x00010010", "end": "0x00010040", "symbol": "FUN_00010010",
     "source": "src/eboot/sdk_thing_00010010.c", "cc": "gcc171-O2", "status": "EXACT"},
    {"module": "EBOOT.BIN", "start": "0x00010040", "end": "0x00010070", "symbol": "FUN_00010040",
     "source": "src/eboot/FUN_00010040.cpp", "cc": "mwcc219", "status": "EXACT"},
]


class CatalogueTests(unittest.TestCase):
    def catalogue(self) -> dict:
        return F.build_catalogue(PROOFS, code_of, lambda path: "CC0-1.0", project="title-a")

    def test_export_lists_identities_only(self) -> None:
        cat = self.catalogue()
        self.assertEqual((cat["format"], cat["masking"], cat["project"]), (F.FORMAT, F.MASKING, "title-a"))
        self.assertEqual(len(cat["entries"]), 2)
        for e in cat["entries"]:
            self.assertEqual(set(e), set(F.ENTRY_KEYS))
            self.assertEqual(e["instructions"], 12)
        by_source = {e["source"]: e for e in cat["entries"]}
        self.assertEqual(by_source["src/eboot/sdk_thing_00010010.c"]["masked_sha256"], F.fingerprint(code(OBJECT_A))[1])
        self.assertEqual(by_source["src/eboot/sdk_thing_00010010.c"]["start"], "0x00010010")
        # no code, signature words or relocation targets are carried
        text = json.dumps(cat)
        self.assertNotIn("sig", text)
        self.assertNotIn(code(OBJECT_A).hex()[:16], text)

    def test_round_trip_and_validation(self) -> None:
        cat = self.catalogue()
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "cat.json"
            path.write_text(json.dumps(cat), encoding="utf-8")
            self.assertEqual(F.read_catalogue(path), cat)
        for broken in ({**cat, "format": "other"}, {**cat, "masking": "other-v9"},
                       {**cat, "entries": [{**cat["entries"][0], "masked_sha256": "xyz"}]},
                       {**cat, "entries": [{k: v for k, v in cat["entries"][0].items() if k != "licence"}]}):
            with self.assertRaises(ValueError):
                F.validate_catalogue(broken)

    def test_lookup_proposes_the_relocated_copy_and_not_the_changed_one(self) -> None:
        cat = F.build_catalogue(PROOFS[:1], code_of, lambda path: "CC0-1.0", project="title-a")
        targets = [("EBOOT.BIN", 0x00200000, 0x00200030, C.analyse(OBJECT_B)[0]),
                   ("EBOOT.BIN", 0x00300000, 0x00300030, C.analyse(OBJECT_C)[0])]
        rows = F.propose(targets, [cat])
        self.assertEqual([r["start"] for r in rows], ["0x00200000"])
        self.assertEqual(rows[0]["exemplar"], {"project": "title-a", "source": "src/eboot/sdk_thing_00010010.c",
                                               "licence": "CC0-1.0", "module": "EBOOT.BIN", "start": "0x00010010",
                                               "cc": "gcc171-O2"})
        self.assertFalse(rows[0]["covered"])
        rows = F.propose(targets, [cat], covered={("EBOOT.BIN", 0x00200000)})
        self.assertTrue(rows[0]["covered"])

    def test_short_functions_are_not_proposed(self) -> None:
        stub = [JR_RA, NOP]
        cat = F.build_catalogue([{**PROOFS[0], "end": "0x00010018"}],
                                lambda m, s, e: code(stub), lambda path: "CC0-1.0")
        targets = [("EBOOT.BIN", 0x00200000, 0x00200008, C.analyse(stub)[0])]
        self.assertEqual(F.propose(targets, [cat]), [])
        self.assertEqual(len(F.propose(targets, [cat], min_instructions=2)), 1)


class LicenceTests(unittest.TestCase):
    def test_last_matching_annotation_is_effective(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            toml = Path(tmp) / "REUSE.toml"
            toml.write_text('version = 1\n'
                            '[[annotations]]\npath = ["src/**"]\nSPDX-FileCopyrightText = "x"\n'
                            'SPDX-License-Identifier = "CC0-1.0"\n'
                            '[[annotations]]\npath = "src/eboot/strlen_*.c"\nprecedence = "override"\n'
                            'SPDX-FileCopyrightText = "y"\nSPDX-License-Identifier = "LicenseRef-Newlib"\n',
                            encoding="utf-8")
            licence = F.licence_lookup(toml)
        self.assertEqual(licence("src/eboot/a_00010000.c"), "CC0-1.0")
        self.assertEqual(licence("src/eboot/strlen_00010000.c"), "LicenseRef-Newlib")
        self.assertEqual(licence("elsewhere.c"), "NOASSERTION")


class PrivatePathTests(unittest.TestCase):
    def test_catalogue_never_lands_in_the_versioned_tree(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "repo"
            root.mkdir()
            subprocess.run(["git", "init", "-q"], cwd=root, check=True)
            (root / ".gitignore").write_text("/try/\n", encoding="utf-8")
            self.assertTrue(F.private_path(root / "try" / "cat.json", root))
            self.assertTrue(F.private_path(Path(tmp) / "outside.json", root))
            self.assertFalse(F.private_path(root / "data" / "cat.json", root))
            with self.assertRaises(SystemExit):
                F.write_private(root / "data" / "cat.json", {}, root)
            self.assertFalse((root / "data" / "cat.json").exists())
            F.write_private(root / "try" / "cat.json", {"ok": 1}, root)
            self.assertEqual(json.loads((root / "try" / "cat.json").read_text(encoding="utf-8")), {"ok": 1})

    def test_repository_ignores_catalogue_files(self) -> None:
        root = F.ROOT
        if not (root / ".git").exists():
            self.skipTest("not a git checkout")
        self.assertTrue(F.private_path(F.DEFAULT_OUT, root))
        self.assertTrue(F.private_path(root / "data" / "sdk-fingerprints-hst-decomp.json", root))


if __name__ == "__main__":
    unittest.main()
