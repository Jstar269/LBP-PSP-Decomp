#!/usr/bin/env python3
"""Tests for the fake-match gate (lbpgate.py) and for the places that must use it."""
from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpgate as G

# idioms a source may carry in src/ (each named by a project rule; anything else is a new idiom to review)
KNOWN_IDIOMS = {"vfpu-inline-asm"}   # owner decision 2026-10-27: VFPU-only inline asm under `asm=vfpu`


def kinds(text: str, header: dict | None = None, tree: str = "src") -> set[str]:
    return {f.kind for f in G.check_text(text, header, tree)}


def refused(text: str, header: dict | None = None, tree: str = "src") -> set[str]:
    return {f.kind for f in G.refusals(G.check_text(text, header, tree))}


class RefusalTests(unittest.TestCase):
    def test_plain_c_has_no_findings(self) -> None:
        self.assertEqual(kinds('#include <string.h>\n#pragma inline_depth(8)\nint f(int a) { return a + 1; }\n'), set())

    def test_asm_statements_are_refused_whatever_the_dialect(self) -> None:
        self.assertEqual(refused('void f(void) { __asm__ volatile ("addiu $2,$2,1"); }'), {"asm-emit"})
        self.assertEqual(refused('void f(void) { asm("nop"); }'), {"asm-emit"})
        self.assertEqual(refused('asm void f(void) {\n  addiu r3, r3, 1\n  blr\n}\n'), {"asm-emit"})
        self.assertEqual(refused('void f(void) { __asm { nop } }'), {"asm-emit"})
        self.assertEqual(refused('void f(void) { _asm { nop } }'), {"asm-emit"})

    def test_a_register_binding_and_an_empty_barrier_are_tagged_but_allowed(self) -> None:
        text = 'void f(const unsigned char *src) {\n    register unsigned char c asm("$2") = *src;\n}\n'
        self.assertEqual(kinds(text), {"register-asm-binding"})
        self.assertEqual(refused(text), set())
        self.assertEqual(kinds('void f(void) { __asm__ __volatile__("" ::: "memory"); }'), {"empty-asm-barrier"})
        self.assertEqual(refused('void f(void) { asm volatile("" : : : "memory"); }'), set())
        # a binding does not excuse real code in the same function
        self.assertEqual(refused('void f(void) { register int c asm("$2") = 1; asm("nop"); }'), {"asm-emit"})

    def test_data_directives_are_refused_but_member_access_and_designators_are_not(self) -> None:
        self.assertEqual(refused('void f(void) { __asm__(".word 0x27bdffe0"); }'), {"asm-emit", "data-directive"})
        self.assertEqual(refused('const char *s = ".byte 1, 2";'), {"data-directive"})
        self.assertEqual(refused('__asm__(".incbin \\"retail.bin\\"");'), {"asm-emit", "data-directive"})
        self.assertEqual(kinds('struct S { int word; } s = { .word = 1 };\nint g(struct S *p) { return p->word + s.word; }\n'), set())

    def test_pragmas(self) -> None:
        self.assertEqual(refused("#pragma asm\n"), {"pragma-code"})
        self.assertEqual(refused("#pragma push_asm\n"), {"pragma-code"})
        self.assertEqual(kinds("#pragma opt_strength_reduction off\n#pragma inline_depth(8)\n#pragma exceptions on\n"), set())
        findings = G.check_text("#pragma mystery on\n", None, "src")
        self.assertEqual([(f.kind, f.severity) for f in findings], [("pragma-unknown", G.WARN)])

    def test_text_section_tricks_and_binary_includes_are_refused(self) -> None:
        self.assertEqual(refused('__attribute__((section(".text"))) const unsigned code[] = { 0x27bdffe0 };'), {"text-section"})
        self.assertEqual(refused('#pragma section ".text"\n'), {"text-section"})
        self.assertEqual(refused('const unsigned char blob[] = {\n#include "retail.bin"\n};\n'), {"binary-include"})
        self.assertEqual(refused('#include "table.inc"\n'), {"binary-include"})
        self.assertEqual(refused('#include <stdio.h>\n#include "local.h"\n'), set())

    def test_extra_is_refused_only_in_src(self) -> None:
        self.assertEqual(refused("int x;", {"extra": "-opt nopeephole"}, "src"), {"extra-header"})
        self.assertEqual(refused("int x;", {"extra": "-opt nopeephole"}, "wip"), set())
        self.assertEqual(refused("int x;", {"module": "EBOOT.BIN"}, "src"), set())

    def test_comments_are_not_scanned_but_strings_are(self) -> None:
        self.assertEqual(kinds("/* asm(\".word 1\") */\n// __asm__ volatile (\"nop\");\nint x;\n"), set())
        self.assertEqual(refused('const char *s = "/* not a comment */ .word 7";'), {"data-directive"})

    def test_findings_carry_line_numbers(self) -> None:
        found = G.check_text("int a;\nint b;\nvoid f(void) { asm(\"nop\"); }\n")
        self.assertEqual([f.line for f in found], [3])


class WiringTests(unittest.TestCase):
    """The gate must be called from the matcher, the harvester and the linter."""

    def source(self, name: str) -> str:
        return (Path(__file__).resolve().parent / name).read_text(encoding="utf-8")

    def test_lbpmatch_reports_a_distinct_status(self) -> None:
        text = self.source("lbpmatch.py")
        self.assertIn("import lbpgate", text)
        self.assertIn('status, detail = "EXACT_FAILS_GATE"', text)

    def test_lbplint_reports(self) -> None:
        self.assertIn('flags["gate"]', self.source("lbplint.py"))

    def test_check_file_uses_the_tree_of_the_path(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "src").mkdir()
            (root / "wip").mkdir()
            text = '/* LBPMATCH module=EBOOT.BIN start=0x0 end=0x4 symbol=f lang=c99 extra="-opt x" */\nint f;\n'
            (root / "src" / "a.c").write_text(text, encoding="utf-8")
            (root / "wip" / "a.c").write_text(text, encoding="utf-8")
            self.assertEqual({f.kind for f in G.refusals(G.check_file(root / "src" / "a.c", root))}, {"extra-header"})
            self.assertEqual(G.refusals(G.check_file(root / "wip" / "a.c", root)), [])

    def test_the_tree_has_no_refused_source_and_only_the_known_idioms(self) -> None:
        root = G.ROOT
        refused_files, idioms = [], []
        for p in sorted((root / "src").rglob("*")):
            if p.suffix in (".c", ".cpp"):
                found = G.check_file(p, root)
                if G.refusals(found):
                    refused_files.append(p.name)
                if any(f.kind not in KNOWN_IDIOMS for f in found):
                    idioms.append(p.name)
        self.assertEqual(refused_files, [])
        self.assertEqual(sorted(idioms), [])


    def test_vfpu_inline_asm_rule(self) -> None:
        """LBP rule (owner 2026-10-27): VFPU-only inline asm with header asm=vfpu and a stated reason is an idiom;
        a non-VFPU instruction, a missing key or a missing reason keeps it a refusal."""
        b = chr(92)
        ok = ('/* LBPMATCH module=EBOOT.BIN start=0x0 end=0x4 symbol=f lang=c99 asm=vfpu */' + chr(10) +
              '/* Original inline VFPU asm: the vector math header used lv.q/vdot. */' + chr(10) +
              'void f(float *v){ __asm__ volatile("lv.q C000, 0(%0)' + b + 'n" "vdot.q S010, C000, C000' + b + 'n" '
              '"sv.s S010, 0(%0)" : : "r"(v) : "memory"); }' + chr(10))
        kinds = lambda t, h: {(f.kind, f.severity) for f in G.check_text(t, h, "src")}  # noqa: E731
        self.assertEqual(kinds(ok, {"asm": "vfpu"}), {("vfpu-inline-asm", G.IDIOM)})
        self.assertEqual(kinds(ok.replace("vdot.q S010, C000, C000", "addiu $t0, $t0, 1"), {"asm": "vfpu"}),
                         {("asm-emit", G.REFUSE)})
        self.assertEqual(kinds(ok, {}), {("asm-emit", G.REFUSE)})
        self.assertEqual(kinds(ok.replace("Original inline VFPU asm:", "VFPU:"), {"asm": "vfpu"}),
                         {("asm-emit", G.REFUSE)})


class EmptyBarrierIsThisStatementTests(unittest.TestCase):
    def test_an_emitting_asm_followed_by_a_barrier_is_refused(self) -> None:
        # the barrier idiom must describe the asm statement itself, not a barrier written after it
        text = 'void f(void) { asm("addiu $2,$2,1"); asm("" ::: "memory"); }\n'
        self.assertEqual(refused(text), {"asm-emit"})
        text = 'void f(void) {\n    __asm__ volatile ("addiu $2,$2,1");\n    __asm__ volatile ("" : : : "memory");\n}\n'
        self.assertEqual(refused(text), {"asm-emit"})
        self.assertEqual(kinds('void f(void) { __asm__ __volatile__ ("" ::: "memory"); }'), {"empty-asm-barrier"})


class VfpuTemplateEscapeTests(unittest.TestCase):
    """An octal or hex escape is a newline or a `;` to the assembler: it must not hide a second instruction."""
    HEADER = {"module": "EBOOT.BIN", "start": "0x0", "end": "0x4", "symbol": "f", "asm": "vfpu"}
    REASON = "/* Original inline VFPU asm: the SDK vector helper. */\n"

    def found(self, template: str) -> set[tuple[str, str]]:
        text = self.REASON + 'void f(float *p) { __asm__ volatile("' + template + '" : : "r"(p)); }\n'
        return {(f.kind, f.severity) for f in G.check_text(text, self.HEADER, "src")}

    def test_escaped_separators_do_not_hide_an_instruction(self) -> None:
        b = chr(92)
        for sep in (b + "012", b + "12", b + "x0a", b + "x0A", b + "073", b + "x3b", b + "r", b + "v", b + "f"):
            with self.subTest(sep=sep):
                self.assertEqual(self.found("vmov.s S000, S001" + sep + "addiu $2, $2, 1"), {("asm-emit", G.REFUSE)})

    def test_plain_escapes_are_still_read(self) -> None:
        b = chr(92)
        self.assertEqual(self.found("lv.s S000, 0(%0)" + b + "n" + b + "tvmov.s S001, S000" + b + "n"),
                         {("vfpu-inline-asm", G.IDIOM)})
        self.assertEqual(self.found("lv.s S000, 0(%0)" + b + "012vmov.s S001, S000"), {("vfpu-inline-asm", G.IDIOM)})


class TextSectionSpellingTests(unittest.TestCase):
    """Every spelling of a section attribute GCC accepts, and a section name the gate cannot read, is refused."""

    def test_other_spellings_of_a_text_section_are_refused(self) -> None:
        for text in ('const unsigned code[] __attribute__((__section__(".text"))) = { 0x27bdffe0 };',
                     'const unsigned code[] __attribute__((section(" .text.f"))) = { 0x27bdffe0 };',
                     '#define SEC ".text"\nconst unsigned code[] __attribute__((section(SEC))) = { 0x27bdffe0 };',
                     'const unsigned code[] __attribute__((section("." "text"))) = { 0x27bdffe0 };',
                     '#define IN(s) __attribute__((__section__(s)))\nconst unsigned code[] IN(".text") = { 1 };',
                     '#pragma define_section mytext ".text" ".text"\n',
                     '%:pragma section ".text"\n',
                     '_Pragma("section \\".text\\"")\n'):
            with self.subTest(text=text):
                self.assertEqual(refused(text), {"text-section"})

    def test_a_named_data_section_is_still_fine(self) -> None:
        self.assertEqual(kinds('extern int *p __attribute__((__section__(".sdata")));\n'
                               'int q __attribute__((aligned(4), section(".sbss")));\n'), set())


class LineSpliceTests(unittest.TestCase):
    """A backslash-newline joins two lines before the compiler reads a token: the gate reads the joined line too."""

    def test_a_keyword_split_by_a_line_splice_is_refused(self) -> None:
        b = chr(92)
        self.assertEqual(refused('void f(void) { as' + b + '\nm("addiu $2,$2,1"); }\n'), {"asm-emit"})
        self.assertEqual(refused('void f(void) { __as' + b + '\nm__ volatile ("addiu $2,$2,1"); }\n'), {"asm-emit"})
        self.assertEqual(refused('#pra' + b + '\ngma asm\n'), {"pragma-code"})
        self.assertEqual(refused('const char *s = ".wo' + b + '\nrd 1";\n'), {"data-directive"})

    def test_a_backslash_with_trailing_blanks_is_refused(self) -> None:
        self.assertEqual(refused('// note ' + chr(92) + '  \nint x;\n'), {"loose-line-splice"})

    def test_multi_line_macros_keep_their_line_numbers(self) -> None:
        b = chr(92)
        text = '#define ADD(a, b) ' + b + '\n    ((a) + ' + b + '\n     (b))\nint f(int x) { return ADD(x, 1); }\nasm("nop");\n'
        self.assertEqual(G.splice_lines(text).count("\n"), text.count("\n"))
        self.assertEqual([(f.kind, f.line) for f in G.check_text(text, None, "src")], [("asm-emit", 5)])


class TokenPasteTests(unittest.TestCase):
    """`##` can build the asm keyword out of tokens the gate never sees as one."""

    def test_a_paste_that_can_spell_asm_is_refused(self) -> None:
        for text in ('#define CAT(a, b) a##b\nvoid f(void) { CAT(as, m)("addiu $2,$2,1"); }\n',
                     '#define K(x) as ## x\nvoid f(void) { K(m)("addiu $2,$2,1"); }\n',
                     '#define K(x) x##m__\nvoid f(void) { K(__as)("addiu $2,$2,1"); }\n',
                     '#define K __as##m__\nvoid f(void) { K("addiu $2,$2,1"); }\n',
                     '#define K(...) __VA_ARGS__ ## sm\n'):
            with self.subTest(text=text):
                self.assertEqual(refused(text), {"asm-paste"})

    def test_a_paste_that_cannot_spell_asm_is_fine(self) -> None:
        self.assertEqual(kinds('#define V(n) virtual void v##n();\n#define FLD(n) unk_##n\nstruct S { int FLD(14); };\n'),
                         set())


class IncludeTests(unittest.TestCase):
    """Every directive that reads a file is checked, and so is what a repository header holds."""

    def test_every_include_form_is_checked(self) -> None:
        for text in ('#include_next "table.inc"\n', '#import "table.inc"\n', '%:include "table.inc"\n',
                     '??=include "table.inc"\n', '#define F "table.inc"\n#include F\n'):
            with self.subTest(text=text):
                self.assertEqual(refused(text), {"binary-include"})
        self.assertEqual(kinds('#include_next <limits.h>\n%:include "x.h"\n'), set())

    def test_a_header_the_source_includes_is_checked(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root / "src" / "m").mkdir(parents=True)
            (root / "include" / "g").mkdir(parents=True)
            (root / "src" / "m" / "evil.h").write_text('static void g(void) { asm("addiu $2,$2,1"); }\n', encoding="utf-8")
            (root / "include" / "g" / "sec.h").write_text('#include "deeper.h"\n', encoding="utf-8")
            (root / "include" / "g" / "deeper.h").write_text(
                'const unsigned c[] __attribute__((section(".text"))) = { 1 };\n', encoding="utf-8")
            (root / "include" / "g" / "ok.h").write_text('struct S { int a; };\n', encoding="utf-8")
            (root / "src" / "m" / "a.c").write_text('#include "evil.h"\nint x;\n', encoding="utf-8")
            (root / "src" / "m" / "b.c").write_text('#include <g/sec.h>\nint x;\n', encoding="utf-8")
            (root / "src" / "m" / "c.c").write_text('#include <g/ok.h>\n#include <string.h>\nint x;\n', encoding="utf-8")
            found = G.refusals(G.check_file(root / "src" / "m" / "a.c", root))
            self.assertEqual([(f.kind, f.line) for f in found], [("asm-emit", 1)])
            self.assertIn("src/m/evil.h:1:", found[0].text)
            self.assertEqual(G.summary(G.check_file(root / "src" / "m" / "b.c", root)), "text-section")
            self.assertEqual(G.check_file(root / "src" / "m" / "c.c", root), [])


class AsmInPreprocessorLineTests(unittest.TestCase):
    """Only a #pragma line is left to pragma-code: an asm in a #define (or any other directive) is still an asm."""

    def test_asm_in_a_define_is_checked(self) -> None:
        text = '#define BUMP() __asm__ volatile ("addiu $2, $2, 1")\nvoid f(void) { BUMP(); }\n'
        self.assertEqual(refused(text), {"asm-emit"})
        self.assertEqual(refused('#  define EMIT asm("nop")\nvoid f(void) { EMIT; }\n'), {"asm-emit"})
        self.assertEqual(refused("#pragma asm\n"), {"pragma-code"})


class VfpuTemplateIsLiteralTests(unittest.TestCase):
    """A template the gate checks must be string literals only: a macro or a stringified argument in it adds text
    the gate never reads."""
    HEADER = {"asm": "vfpu"}
    REASON = "/* Original inline VFPU asm: the vector math header. */\n"

    def found(self, text: str) -> set[tuple[str, str]]:
        return {(f.kind, f.severity) for f in G.check_text(self.REASON + text, self.HEADER, "src")}

    def test_a_template_built_from_a_macro_is_refused(self) -> None:
        b = chr(92)
        for text in ('#define X "addiu $2, $2, 1' + b + 'n"\nvoid f(void) { __asm__ volatile(X "vmov.s S000, S001"); }\n',
                     '#define S(x) #x\nvoid f(void) { __asm__ volatile(S(addiu $2, $2, 1;) "vmov.s S000, S001"); }\n',
                     'void f(void) { __asm__ volatile(("vmov.s S000, S001")); }\n'):
            with self.subTest(text=text):
                self.assertEqual(self.found(text), {("asm-emit", G.REFUSE)})

    def test_a_literal_template_is_still_the_idiom(self) -> None:
        b = chr(92)
        self.assertEqual(self.found('void f(float *d, float *s) { __asm__("lv.q C000, %1' + b + 'n sv.s S000, %0" '
                                    ': "=m"(d[0]) : "m"(*s)); }\n'), {("vfpu-inline-asm", G.IDIOM)})


if __name__ == "__main__":
    unittest.main()
