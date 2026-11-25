#!/usr/bin/env python3
"""Tests for the second, independent verifier (lbpxverify.py) on synthetic ELF images and a synthetic public tree:
that it shares no code with the tools it checks, that its byte comparison agrees with the matcher's rules, and that
it finds every kind of disagreement in the ledger and the published figures. No WSL, no retail bytes."""
from __future__ import annotations

import ast
import hashlib
import json
import os
import struct
import sys
import tempfile
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402
import lbpprogress as G  # noqa: E402
import lbpxverify as X  # noqa: E402
import test_lbpprogress as T  # noqa: E402

BASE = 0x08804000
CODE = bytes((7 * i + 3) & 0xFF for i in range(0x200))          # synthetic stand-in for retail code at BASE


def image(sections: list[tuple[str, int, int, bytes, int]], symbols: list[tuple[str, int, int, int]],
          load: tuple[int, bytes] | None = None) -> bytes:
    """An ELF32 little-endian MIPS image. sections: (name, type, flags, contents or b"" with a size for NOBITS,
    address) given as (name, type, flags, contents, addr); symbols: (name, value, size, section index from 1);
    load: (vaddr, bytes) of one RWX PT_LOAD."""
    names = bytearray(b"\0")
    shdrs = [bytes(40)]
    data_off = 52 + (32 if load else 0)
    body = bytearray()
    if load:
        body += load[1]
    for name, typ, flags, contents, addr in sections:
        off = data_off + len(body)
        size = len(contents)
        if typ != X.SHT_NOBITS:
            body += contents
        shdrs.append(struct.pack("<IIIIIIIIII", len(names), typ, flags, addr, off, size, 0, 0, 4, 0))
        names += name.encode() + b"\0"
    strtab = bytearray(b"\0")
    symtab = bytearray(16)
    for name, value, size, shndx in symbols:
        symtab += struct.pack("<IIIBBH", len(strtab), value, size, 0x12, 0, shndx)
        strtab += name.encode() + b"\0"
    n = len(sections)
    for name, typ, contents, link, entsize in ((".symtab", 2, symtab, n + 2, 16), (".strtab", 3, strtab, 0, 0),
                                               (".shstrtab", 3, None, 0, 0)):
        if contents is None:
            names += name.encode() + b"\0"
            contents = names
            shdrs.append(struct.pack("<IIIIIIIIII", len(names) - len(name) - 1, typ, 0, 0, data_off + len(body),
                                     len(contents), link, 0, 1, entsize))
        else:
            shdrs.append(struct.pack("<IIIIIIIIII", len(names), typ, 0, 0, data_off + len(body), len(contents), link,
                                     1 if typ == 2 else 0, 4, entsize))
            names += name.encode() + b"\0"
        body += contents
    shoff = data_off + len(body)
    eh = b"\x7fELF" + bytes([1, 1, 1]) + bytes(9) + struct.pack(
        "<HHIIIIIHHHHHH", 2, 8, 1, BASE, 52 if load else 0, shoff, 0, 52, 32, 1 if load else 0, 40, len(shdrs),
        len(shdrs) - 1)
    ph = struct.pack("<IIIIIIII", 1, data_off, load[0], load[0], len(load[1]), len(load[1]), 7, 16) if load else b""
    return eh + ph + bytes(body) + b"".join(shdrs)


def retail_image() -> X.Elf:
    return X.Elf(image([], [], load=(BASE, CODE)), "retail")


def linked(code: bytes, start: int, symbol: str = "f", size: int | None = None,
           extra: list[tuple[str, int, int, bytes, int]] = ()) -> X.Elf:
    secs = [(".text", 1, 6, code, start), *extra]
    return X.Elf(image(secs, [(symbol, start, len(code) if size is None else size, 1)]), "linked")


def hdr(start: int, end: int, symbol: str = "f", **placed: int) -> dict:
    return {"module": "EBOOT.BIN", "start": start, "end": end, "symbol": symbol, "extra": None, "placed": placed,
            "functions": [(symbol, start, end)]}


class IndependenceTests(unittest.TestCase):
    def test_it_imports_no_tool_but_the_configuration(self) -> None:
        tree = ast.parse((Path(X.__file__)).read_text(encoding="utf-8"))
        names = {a.name.split(".")[0] for n in ast.walk(tree) if isinstance(n, ast.Import) for a in n.names}
        names |= {n.module.split(".")[0] for n in ast.walk(tree) if isinstance(n, ast.ImportFrom) and n.module}
        ours = {p.stem for p in Path(X.__file__).parent.glob("*.py")}
        self.assertEqual(names & ours, {"lbpconfig"})

    def test_its_build_key_is_the_matchers(self) -> None:
        for rel in ("src/eboot/FUN_08804000.c", "wip/x y.cpp", "try/gen-a1/v0/FUN_x.c"):
            self.assertEqual(X.build_key(M.ROOT / rel), M.build_key(M.ROOT / rel))

    def test_its_header_parser_agrees_with_the_matchers_on_every_source(self) -> None:
        paths = sorted(p for t in ("src", "wip") for p in (M.ROOT / t).rglob("*") if p.suffix in (".c", ".cpp"))
        for p in paths[:: max(1, len(paths) // 400)]:
            mine = X.parse_header(p.read_text(encoding="utf-8", errors="replace"), p.name)
            theirs = M.parse_header(p)
            self.assertEqual((mine["module"], mine["start"], mine["end"], mine["symbol"]),
                             (theirs["module"], theirs["start"], theirs["end"], theirs["symbol"]), p)

    def test_header_refusals(self) -> None:
        good = "/* LBPMATCH module=M start=0x10 end=0x20 symbol=f lang=c99 rodata=0x100 */"
        self.assertEqual(X.parse_header(good)["placed"], {"rodata": 0x100})
        for bad in ("int x;", good.replace("lang", "colour"), good.replace("end=0x20", "end=0x10"),
                    good.replace(" */", " start=0x10 */"), good.replace("symbol=f", "")):
            with self.assertRaises(X.Refusal, msg=bad):
                X.parse_header(bad)
        two = X.parse_header(good.replace(" */", " helpers=g@0x30-0x40 */"))
        self.assertEqual(two["functions"], [("f", 0x10, 0x20), ("g", 0x30, 0x40)])
        with self.assertRaises(X.Refusal):
            X.parse_header(good.replace(" */", " helpers=g@0x18-0x40 */"))


class BytesTests(unittest.TestCase):
    def test_equal_code_matches_and_one_word_differs(self) -> None:
        s = BASE + 0x40
        self.assertEqual(X.compare_linked(linked(CODE[0x40:0x60], s), retail_image(), hdr(s, s + 0x20))["status"],
                         "MATCH")
        bad = bytearray(CODE[0x40:0x60])
        bad[9] ^= 1
        r = X.compare_linked(linked(bytes(bad), s), retail_image(), hdr(s, s + 0x20))
        self.assertEqual((r["status"], r["differing_words"], r["first_difference"]), ("DIFFER", 1, f"0x{s + 8:08x}"))

    def test_a_function_longer_than_its_range_differs(self) -> None:
        s = BASE + 0x40
        r = X.compare_linked(linked(CODE[0x40:0x68], s), retail_image(), hdr(s, s + 0x20))
        self.assertEqual(r["status"], "DIFFER")                         # as tools/lbpmatch.py: the longer body counts

    def test_placement_is_refused_when_wrong(self) -> None:
        s = BASE + 0x40
        with self.assertRaises(X.Refusal):
            X.compare_linked(linked(CODE[0x40:0x60], s + 4), retail_image(), hdr(s, s + 0x20))
        rodata = (".rodata", 1, 2, CODE[0x100:0x110], BASE + 0x100)
        ok = X.compare_linked(linked(CODE[0x40:0x60], s, extra=[rodata]), retail_image(),
                              hdr(s, s + 0x20, rodata=BASE + 0x100))
        self.assertEqual((ok["status"], ok["compared_bytes"]), ("MATCH", 0x30))
        wrong = (".rodata", 1, 2, b"\0" * 16, BASE + 0x100)
        self.assertEqual(X.compare_linked(linked(CODE[0x40:0x60], s, extra=[wrong]), retail_image(),
                                          hdr(s, s + 0x20, rodata=BASE + 0x100))["status"], "DIFFER")
        with self.assertRaises(X.Refusal):
            X.compare_linked(linked(CODE[0x40:0x60], s, extra=[rodata]), retail_image(),
                             hdr(s, s + 0x20, rodata=BASE + 0x104))

    def test_helpers_are_each_checked_at_their_own_address_with_zero_fill(self) -> None:
        code = bytearray(CODE)
        code[0x18:0x20] = bytes(8)                                      # retail: the helper's range ends in zero fill
        retail = X.Elf(image([], [], load=(BASE, bytes(code))), "retail")
        secs = [(".lbp_fn0", 1, 6, bytes(code[0:0x10]), BASE), (".lbp_fn1", 1, 6, bytes(code[0x10:0x18]), BASE + 0x10)]
        ln = X.Elf(image(secs, [("f", BASE, 0x10, 1), ("g", BASE + 0x10, 8, 2)]), "linked")
        h = {**hdr(BASE, BASE + 0x10), "functions": [("f", BASE, BASE + 0x10), ("g", BASE + 0x10, BASE + 0x20)]}
        self.assertEqual(X.compare_linked(ln, retail, h)["status"], "MATCH")
        self.assertEqual(X.compare_linked(ln, retail_image(), h)["status"], "DIFFER")

    def test_a_source_end_to_end(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "src" / "eboot").mkdir(parents=True)
            src = root / "src" / "eboot" / "f.c"
            s = BASE + 0x40
            src.write_text(f"/* LBPMATCH module=EBOOT.BIN start=0x{s:08x} end=0x{s + 0x20:08x} symbol=f */\n",
                           encoding="utf-8")
            bounds = {"EBOOT.BIN": ({s, s + 0x10}, {s + 0x10, s + 0x20}, {})}
            retail = {"EBOOT.BIN": retail_image()}
            self.assertEqual(X.verify_source(src, retail, bounds, root)["status"], "REFUSED")     # no artifact
            out = root / "build" / "EBOOT.BIN" / X.build_key(src, root)
            out.mkdir(parents=True)
            (out / "linked.elf").write_bytes(image([(".text", 1, 6, CODE[0x40:0x60], s)], [("f", s, 0x20, 1)]))
            self.assertEqual(X.verify_source(src, retail, bounds, root)["status"], "MATCH")
            later = time.time() + 5
            os.utime(src, (later, later))
            self.assertIn("older", X.verify_source(src, retail, bounds, root)["reason"])
            os.utime(out / "linked.elf", (later + 1, later + 1))
            self.assertEqual(X.verify_source(src, retail, {"EBOOT.BIN": ({s}, {s + 0x10}, {})}, root)["status"],
                             "REFUSED")                                                  # not whole functions
            src.write_text(src.read_text(encoding="utf-8").replace(" */", ' extra="-O0" */'), encoding="utf-8")
            os.utime(out / "linked.elf", (later + 9, later + 9))
            self.assertIn("extra=", X.verify_source(src, retail, bounds, root)["reason"])


class LedgerTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "src").mkdir()
        self.src = self.root / "src" / "f.c"
        self.src.write_text("/* LBPMATCH module=EBOOT.BIN start=0x08804040 end=0x08804060 symbol=f */\n",
                            encoding="utf-8")
        s = BASE + 0x40
        self.proof = {"module": "EBOOT.BIN", "start": f"0x{s:08x}", "end": f"0x{s + 0x20:08x}", "source": "src/f.c",
                      "source_sha256": hashlib.sha256(self.src.read_bytes()).hexdigest(), "status": "EXACT",
                      "compared": {".text": {"addr": f"0x{s:08x}", "size": 0x20,
                                             "sha256": hashlib.sha256(CODE[0x40:0x60]).hexdigest()}}}
        self.bounds = {"EBOOT.BIN": ({s, s + 0x20}, {s + 0x20, s + 0x40}, {})}
        self.retail = {"EBOOT.BIN": retail_image()}

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def problems(self, proofs: list[dict]) -> list[str]:
        return X.ledger_problems(proofs, self.root, self.retail, self.bounds)

    def test_a_proof_that_holds(self) -> None:
        self.assertEqual(self.problems([self.proof]), [])

    def test_every_kind_of_broken_proof(self) -> None:
        self.assertTrue(any("digest" in p for p in self.problems([
            {**self.proof, "compared": {".text": {**self.proof["compared"][".text"], "sha256": "0" * 64}}}])))
        self.assertTrue(any("differs" in p for p in self.problems([{**self.proof, "source_sha256": "0" * 64}])))
        self.assertTrue(any("overlaps" in p for p in self.problems([self.proof, {**self.proof, "start": "0x08804050"}])))
        self.assertTrue(any("whole functions" in p for p in self.problems([{**self.proof, "end": "0x08804050"}])))
        self.assertTrue(any("status" in p for p in self.problems([{**self.proof, "status": "MISMATCH"}])))

    def test_the_matchers_artifact_must_have_the_proofs_digest(self) -> None:
        out = self.root / "build" / "EBOOT.BIN" / X.build_key(self.src, self.root)
        out.mkdir(parents=True)
        s = BASE + 0x40
        (out / "linked.elf").write_bytes(image([(".text", 1, 6, CODE[0x40:0x60], s)], [("f", s, 0x20, 1)]))
        self.assertEqual(self.problems([self.proof]), [])
        (out / "linked.elf").write_bytes(image([(".text", 1, 6, bytes(0x20), s)], [("f", s, 0x20, 1)]))
        self.assertTrue(any("matcher's linked code" in p for p in self.problems([self.proof])))

    def test_a_ledger_edited_by_hand_is_refused(self) -> None:
        body = json.dumps(self.proof, sort_keys=True).encode() + b"\n"
        path = self.root / "ledger.jsonl"
        path.write_bytes(json.dumps({"count": 1, "body_sha256": hashlib.sha256(body).hexdigest()}).encode() + b"\n"
                         + body)
        self.assertEqual(X.read_ledger(path)[1], [self.proof])
        path.write_bytes(path.read_bytes().replace(b"EXACT", b"EXACX"))
        with self.assertRaises(X.FormatError):
            X.read_ledger(path)


class FigureTests(unittest.TestCase):
    """The same tree as test_lbpprogress.py: the first verifier (lbpprogress, lbpreport, lbppublish) and this one
    must agree that it is consistent, and each must find every disagreement planted in it."""

    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.tree = T.make_tree(Path(self.tmp.name))

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def edit(self, rel: str, old: str, new: str) -> None:
        p = self.tree / rel
        text = p.read_text(encoding="utf-8")
        self.assertIn(old, text)
        p.write_text(text.replace(old, new, 1), encoding="utf-8")

    def both(self) -> tuple[list[str], list[str]]:
        first = T.run("lbpprogress.py", "--check", "--tree", str(self.tree))
        return X.figure_problems(self.tree), ([] if first.returncode == 0 else [first.stdout])

    def test_both_verifiers_accept_a_consistent_tree(self) -> None:
        self.assertEqual(self.both(), ([], []))
        self.assertEqual(X.figure_problems(self.tree, T.PROOFS), [])
        self.assertEqual(X.main(["figures", "--tree", str(self.tree)]), 0)

    def test_both_verifiers_find_figures_that_disagree_with_the_rows(self) -> None:
        self.edit("data/public-figures.json", '"public_t1_bytes": 128', '"public_t1_bytes": 64')
        self.edit("data/public-figures.json", '"public_t1_percent": 28.571', '"public_t1_percent": 14.286')
        mine, first = self.both()
        self.assertTrue(mine and first)

    def test_both_verifiers_find_a_stale_badge(self) -> None:
        self.edit("progress.json", "tier 1: 0.2%", "tier 1: 0.3%")
        mine, first = self.both()
        self.assertTrue(mine and first, (mine, first))

    def test_a_readme_figure_or_a_report_total_that_was_edited(self) -> None:
        self.edit("README.md", "128 of the 448 code bytes", "129 of the 448 code bytes")
        self.assertTrue(any("README.md" in p for p in X.figure_problems(self.tree)))
        self.edit("README.md", "129 of the 448 code bytes", "128 of the 448 code bytes")
        self.edit("docs/objdiff-report.json", '"matched_code":"128"', '"matched_code":"127"')
        self.assertTrue(any("objdiff-report" in p for p in X.figure_problems(self.tree)))

    def test_a_record_or_a_ladder_that_was_edited(self) -> None:
        record = json.loads((self.tree / "docs/milestones.json").read_text(encoding="utf-8"))
        record["milestones"][1]["regressed"] = True
        (self.tree / "docs/milestones.json").write_text(json.dumps(record), encoding="utf-8")
        self.assertTrue(any("REGRESSED" in p for p in X.figure_problems(self.tree)))
        record["milestones"][1].update(regressed=False, progress=30.0)
        (self.tree / "docs/milestones.json").write_text(json.dumps(record), encoding="utf-8")
        self.assertTrue(any("recounted 28.571" in p for p in X.figure_problems(self.tree)))
        G.write_tree(self.tree)
        self.edit(G.COMPLETION_REL, "| T4: strict (reviewed) | 0 B | 0.000% |", "| T4: strict (reviewed) | 9 B | 0.014% |")
        self.assertTrue(any("T4" in p for p in X.figure_problems(self.tree)))

    def test_rows_whose_sources_do_not_back_them(self) -> None:
        self.edit("src/eboot/a_00001000.c", "symbol=do_a", "symbol=do_b")
        self.assertTrue(any("header says" in p for p in X.figure_problems(self.tree)))
        self.edit("src/eboot/a_00001000.c", "symbol=do_b", "symbol=do_a")
        self.edit("ORIGINS.json", '"game"', '"sony-sdk"')
        self.assertTrue(any("not a published source" in p for p in X.figure_problems(self.tree)))
        self.edit("ORIGINS.json", '"sony-sdk"', '"game"')
        moved = [dict(T.PROOFS[0], start="0x00001040", end="0x00001080"), T.PROOFS[1]]
        found = X.figure_problems(self.tree, moved)
        self.assertTrue(any("0x00001000" in p and "not an accepted proof" in p for p in found), found)
        self.assertTrue(any("0x00001040" in p and "do not count it" in p for p in found), found)
        uncounted = T.PROOFS + [dict(T.PROOFS[0], start="0x00002000", end="0x00002100")]
        self.assertTrue(any("do not count it" in p for p in X.figure_problems(self.tree, uncounted)))


@unittest.skipUnless((X.ROOT / "data" / "public-figures.json").is_file(), "this tree has no public figures")
class CommittedTests(unittest.TestCase):
    def test_every_committed_figure_agrees_with_the_recount(self) -> None:
        self.assertEqual(X.figure_problems(X.ROOT), [])


if __name__ == "__main__":
    unittest.main()
