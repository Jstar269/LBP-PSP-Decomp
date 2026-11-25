#!/usr/bin/env python3
"""The fake-match gate (docs/COMPLETION.md, gate 2): a source may only be C or C++ that the compiler turns into the bytes.

    python tools/lbpgate.py [--all | files ...] [--show idiom|warn]

One check, shared by tools/lbpmatch.py (an EXACT from a source that fails it is reported as EXACT_FAILS_GATE, never as
EXACT, so no installer accepts it), tools/lbplane.py harvest (refuses to install such a source) and tools/lbplint.py
(reports it). The refusals, each looked for in the source text with comments removed (string literals are kept: an
instruction pasted as text is the thing to find):

  asm-emit          any `asm`, `__asm__`, `__asm` or `_asm` statement, block or function that can emit instructions
  data-directive    an assembler data directive in the text: .word .byte .half .long .short .quad .incbin .ascii .space ...
                    (`{ .word = 1 }` designated initialisers are fine)
  pragma-code       a #pragma that emits code (asm, endasm, push_asm and anything with asm in its name)
  text-section      data or code steered into the code section by hand: __attribute__((section(".text..."))),
                    __declspec(section ".text...") or #pragma section/segment naming .text
  binary-include    #include of a file that is not a header (.bin .inc .s .S .o .dat ...)
  extra-header      an `extra=` key in the LBPMATCH header of a source under src/ (experimental flags never count)

Tagged but allowed:

  register-asm-binding   `register T x asm("$N")`: binds a variable to a register and emits nothing (idiom)
  empty-asm-barrier      `asm("" ...)` with an empty template: emits nothing (idiom)
  vfpu-inline-asm        LBP project rule (owner decision 2026-10-27): inline `asm` whose template holds only Allegrex
                         VFPU instructions, in a source whose LBPMATCH header says `asm=vfpu` and whose comments carry
                         a line `Original inline VFPU asm: <reason>`. The original game sources used inline VFPU
                         assembly (the PSP compilers emit few VFPU instructions themselves), so such code is
                         reconstructed as the inline asm it was. Any non-VFPU instruction in the template, or a missing
                         header key or reason, keeps it a refusal (asm-emit). hst-decomp's rule is unchanged.

Reported only (a warning): pragma-unknown, a #pragma that is not on the list of the compiler controls this project uses.
Every refusal and idiom is a finding with a line number; `refusals(findings)` is what the callers act on.
"""
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

REFUSE, IDIOM, WARN = "refuse", "idiom", "warn"
ASM_TOKEN = re.compile(r"(?<![\w$])(?:__asm__|__asm|_asm|asm)(?![\w$])")
DIRECTIVE = re.compile(r"(?<![\w\])>])\.(?:word|byte|half|hword|short|long|quad|dword|incbin|ascii|asciiz|string|space|skip|fill|zero|"
                       r"float|double|octa|int|2byte|4byte|8byte)\b(?!\s*=)", re.I)
PRAGMA = re.compile(r"^[ \t]*#[ \t]*pragma[ \t]+([A-Za-z_][\w]*)(.*)$", re.M)
# every directive that reads a file (the forms tools/hstproof.py include_closure resolves): #include, #include_next,
# #import, with the `%:` digraph or the `??=` trigraph for `#`; a computed include (`#include NAME`) names no file
INCLUDE = re.compile(r'^[ \t]*(?:#|%:|\?\?=)[ \t]*(?:include_next|include|import)\b[ \t]*(.*)$', re.M)
INCLUDE_NAME = re.compile(r'"([^"\n]+)"|<([^>\n]+)>')
TEXT_SECTION = re.compile(r"""(?:section(?:__)?\s*\(\s*["']\s*\.text|__declspec\s*\(\s*section\s*["']\s*\.text|"""
                          r"""(?:#|%:)\s*pragma\s+\w*(?:section|segment)\w*\b[^\n]*\.text|"""
                          r"""_Pragma\s*\(\s*"[^"\n]*(?:section|segment)[^\n]*\.text)""")
# a section attribute (`section` or `__section__`) inside __attribute__((...)): its name must be one string literal,
# or the gate cannot tell that it is not .text (a macro, a macro parameter, or "." "text")
ATTRIBUTE = re.compile(r"(?<![\w$])(?:__attribute__|__attribute)\s*\(\s*\(")
SECTION_ATTR = re.compile(r"(?<![\w$])(?:__section__|section)\s*\(")
SECTION_NAME = re.compile(r'\s*"([^"\\\n]*)"\s*\)')


def section_name_findings(code: str, lines: list[str]) -> list[Finding]:
    """text-section for a section attribute whose name is not one plain string literal."""
    out: list[Finding] = []
    for a in ATTRIBUTE.finditer(code):
        depth, j = 0, code.find("(", a.start())
        start = j
        while j < len(code):
            ch = code[j]
            if ch == '"':
                k = j + 1
                while k < len(code) and code[k] not in '"\n':
                    k += 2 if code[k] == BACKSLASH else 1
                j = k
            elif ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
                if depth == 0:
                    break
            j += 1
        body = code[start:j + 1]
        for s in SECTION_ATTR.finditer(body):
            if not SECTION_NAME.match(body, s.end()):
                ln = _line_of(code, start + s.start())
                out.append(Finding("text-section", REFUSE, ln, lines[ln - 1]))
    return out


HEADER_EXT = {".h", ".hpp", ".hh", ".hxx", ""}
# compiler controls this project's sources use (pragmas that change how code is generated, not what is emitted)
KNOWN_PRAGMAS = {"opt_strength_reduction", "opt_unroll_loops", "opt_common_subs", "opt_common", "opt_loop_invariants",
                 "opt_propagation", "opt_dead_code", "opt_dead_assignments", "opt_lifetimes", "opt_vectorize_loops",
                 "inline_depth", "inline_max_size", "inline_max_total_size", "inline_bottom_up", "auto_inline",
                 "dont_inline", "peephole", "scheduling", "global_optimizer", "optimization_level", "optimize_for_size",
                 "fp_contract", "pack", "once", "exceptions", "cplusplus", "ANSI_strict", "warning", "message",
                 "readonly_strings", "pool_strings", "opt_unroll_count"}
CODE_PRAGMA = re.compile(r"asm", re.I)


@dataclass(frozen=True)
class Finding:
    kind: str
    severity: str        # refuse | idiom | warn
    line: int
    text: str

    def __str__(self) -> str:
        return f"{self.severity}:{self.kind}:{self.line}: {self.text.strip()[:100]}"


def strip_comments(text: str) -> str:
    """The text with /* */ and // comments blanked out (newlines kept, so line numbers survive); strings are kept."""
    out: list[str] = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join("\n" if ch == "\n" else " " for ch in text[i:j]))
            i = j
        elif text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


def splice_lines(text: str) -> str:
    """Translation phase 2, as every C compiler does it before it reads a token: each backslash-newline removed, so
    `as\\<newline>m` is the keyword `asm` and `#pra\\<newline>gma` a #pragma. The removed newlines are put back after
    the logical line, so a finding keeps the line number of the line the logical line starts on and every line
    after it keeps its own."""
    if "\\\n" not in text:
        return text
    out: list[str] = []
    buf: list[str] = []
    for line in text.split("\n"):
        if line.endswith("\\"):
            buf.append(line[:-1])
            continue
        out.append("".join(buf) + line)
        out.extend([""] * len(buf))
        buf = []
    if buf:                     # the text ends in a backslash: no newline follows the last one
        out.append("".join(buf))
        out.extend([""] * (len(buf) - 1))
    return "\n".join(out)


# a backslash, then spaces or tabs, then a newline: GCC splices it (with a warning), other compilers may not, so the
# gate cannot know which tokens or comments the compiler reads there
LOOSE_SPLICE = re.compile(r"\\[ \t]+$", re.M)


# `##` pastes tokens into an identifier after the gate has read the text: `CAT(as, m)("...")` is an asm statement
# that holds no `asm` token. A paste is refused when its two sides could spell one of these keywords.
ASM_KEYWORDS = ("__asm__", "__asm", "_asm", "asm")
DEFINE = re.compile(r"^[ \t]*(?:#|%:|\?\?=)[ \t]*define[ \t]+[A-Za-z_$][\w$]*(?:\(([^)]*)\))?(.*)$", re.M)
PASTE = re.compile(r"##|%:%:")


def _asm_part(token: str) -> bool:
    return any(token in k for k in ASM_KEYWORDS)


def paste_findings(code: str, lines: list[str]) -> list[Finding]:
    """asm-paste for a `##` in a #define whose operands could join into `asm`, `_asm`, `__asm` or `__asm__`: a
    macro parameter (or __VA_ARGS__) can be anything; a literal side counts only if it is part of such a keyword."""
    out: list[Finding] = []
    for d in DEFINE.finditer(code):
        params = {p.strip() for p in (d.group(1) or "").split(",") if p.strip()}
        if "..." in params:
            params.add("__VA_ARGS__")
        body = re.sub(r'"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'', '""', d.group(2))
        for p in PASTE.finditer(body):
            left = re.search(r"([\w$]+)\s*$", body[:p.start()])
            right = re.match(r"\s*([\w$]+)", body[p.end():])
            sides = [s.group(1) if s else None for s in (left, right)]
            free = [s is None or s in params for s in sides]
            if all(free):
                risky = True
            elif not any(free):
                risky = _asm_part(sides[0] + sides[1])
            else:
                risky = _asm_part(sides[1] if free[0] else sides[0])
            if risky:
                ln = _line_of(code, d.start())
                out.append(Finding("asm-paste", REFUSE, ln, lines[ln - 1]))
                break
    return out


def _line_of(text: str, pos: int) -> int:
    return text.count("\n", 0, pos) + 1


def _statement_around(text: str, pos: int) -> str:
    start = max(text.rfind(";", 0, pos), text.rfind("{", 0, pos), text.rfind("}", 0, pos)) + 1
    ends = [e for e in (text.find(";", pos), text.find("{", pos)) if e >= 0]
    return text[start:(min(ends) + 1 if ends else len(text))]


# Allegrex VFPU mnemonics (vector unit; COP2 loads/stores, moves and branches included). Prefixes `.s/.p/.t/.q` and
# condition suffixes are part of the mnemonic token and accepted by the patterns below.
VFPU_MNEMONIC = re.compile(r"^(?:v[a-z0-9]+(?:\.[spqt])?|lv\.[sq]|lvl\.q|lvr\.q|sv\.[sq]|svl\.q|svr\.q|ulv\.q|usv\.q|"
                           r"mfv|mtv|mfvc|mtvc|bvfl?|bvtl?)$", re.I)
VFPU_REASON = re.compile(r"Original inline VFPU asm:\s*\S")
ASM_PREFIX = re.compile(r"^(?:__asm__|__asm|_asm|asm)\s*(?:__volatile__|volatile)?\s*$")
BACKSLASH = chr(92)


def asm_template(code: str, pos: int) -> str | None:
    """The concatenated string-literal template of the asm statement whose keyword is at pos (escape sequences kept
    as written), or None when the statement is not of the form asm [volatile] ( "..." ... )."""
    i = code.find("(", pos)
    if i < 0 or not ASM_PREFIX.match(code[pos:i].strip()):
        return None
    out: list[str] = []
    depth, j = 0, i
    while j < len(code):
        ch = code[j]
        if ch == "(":
            depth += 1
            if depth > 1:
                return None     # a parenthesised or macro-built template
        elif ch == ")":
            depth -= 1
            if depth == 0:
                break
        elif ch == ":" and depth == 1:
            break
        elif ch == '"':
            k = j + 1
            while k < len(code) and code[k] != '"':
                if code[k] == BACKSLASH and k + 1 < len(code):
                    out.append(code[k:k + 2])
                    k += 2
                else:
                    out.append(code[k])
                    k += 1
            out.append("\n")
            j = k
        elif not ch.isspace():
            return None         # a macro or anything else but string literals in the template: unreadable
        j += 1
    return "".join(out)


C_ESCAPES = {"n": "\n", "t": "\t", "r": "\r", "v": "\v", "f": "\f", "a": "\a", "b": "\b", "e": "\x1b",
             BACKSLASH: BACKSLASH, '"': '"', "'": "'", "?": "?"}


def decode_template(template: str) -> str | None:
    """An asm template as the compiler hands it to the assembler: every C escape decoded, octal (`\\012`) and hex
    (`\\x0a`) included, and a backslash-newline removed; tabs become spaces. None when the decoded text holds
    anything but printable ASCII, spaces, tabs and newlines, or a backslash: an escape the assembler could read as
    a line or statement separator (`\\012`, `\\073`), a macro argument or a second instruction."""
    out: list[str] = []
    i, n = 0, len(template)
    while i < n:
        c = template[i]
        if c != BACKSLASH or i + 1 >= n:
            out.append(c)
            i += 1
            continue
        d = template[i + 1]
        if d in "01234567":
            m = re.match(r"[0-7]{1,3}", template[i + 1:])
            out.append(chr(int(m.group(0), 8) & 0xFF))
            i += 1 + len(m.group(0))
        elif d == "x":
            m = re.match(r"[0-9A-Fa-f]+", template[i + 2:])
            if not m:
                return None
            out.append(chr(int(m.group(0), 16) & 0xFF))
            i += 2 + len(m.group(0))
        elif d == "\n":
            i += 2              # a line splice inside the literal
        elif d in C_ESCAPES:
            out.append(C_ESCAPES[d])
            i += 2
        else:
            return None
    text = "".join(out)
    if BACKSLASH in text or any(not (ch in "\n\t" or " " <= ch <= "~") for ch in text):
        return None
    return text.replace("\t", " ")


def vfpu_only(template: str) -> bool:
    """True when every instruction of an asm template is a VFPU instruction (labels and empty parts ignored)."""
    text = decode_template(template)
    if text is None:
        return False
    seen = False
    for part in re.split(r"[\n;]", text):
        part = part.strip()
        if not part or part.endswith(":"):
            continue
        if not VFPU_MNEMONIC.match(part.split()[0]):
            return False
        seen = True
    return seen


REGISTER_BINDING = re.compile(r"""^\s*(?:static\s+|volatile\s+|const\s+)*register\b[^;{}]*?(?<![\w$])(?:__asm__|__asm|asm)\s*\(\s*["']\$?\w+["']\s*\)\s*(?:=[^;]*)?;?\s*$""", re.S)
EMPTY_ASM = re.compile(r"""(?:__asm__|__asm|asm)\s*(?:__volatile__|volatile)?\s*\(\s*(?:""\s*)+(?:[:)]|$)""")


def check_text(text: str, header: dict | None = None, tree: str = "src") -> list[Finding]:
    """Findings for one source. `header` is the parsed LBPMATCH header (for extra=); `tree` is "src" for a source that
    is installed (an `extra=` key is refused there) or anything else (wip, a lane's scratch file)."""
    code = strip_comments(splice_lines(text))
    findings: list[Finding] = []
    lines = text.split("\n")
    code_lines = code.split("\n")
    for m in ASM_TOKEN.finditer(code):
        stmt = _statement_around(code, m.start())
        ln = _line_of(code, m.start())
        shown = lines[ln - 1] if ln - 1 < len(lines) else m.group(0)
        if PRAGMA.match(code_lines[ln - 1]):     # the logical line (a #pragma split by a line splice too)
            continue            # a #pragma asm line is reported once, as pragma-code (an asm in a #define is not)
        if REGISTER_BINDING.match(stmt):
            findings.append(Finding("register-asm-binding", IDIOM, ln, shown))
        elif EMPTY_ASM.match(code, m.start()):     # this statement's own template, not a later barrier
            findings.append(Finding("empty-asm-barrier", IDIOM, ln, shown))
        elif (header is not None and header.get("asm") == "vfpu" and VFPU_REASON.search(text)
              and (tpl := asm_template(code, m.start())) is not None and vfpu_only(tpl)):
            findings.append(Finding("vfpu-inline-asm", IDIOM, ln, shown))
        else:
            findings.append(Finding("asm-emit", REFUSE, ln, shown))
    for m in LOOSE_SPLICE.finditer(text):
        ln = _line_of(text, m.start())
        findings.append(Finding("loose-line-splice", REFUSE, ln, lines[ln - 1]))


    findings += paste_findings(code, lines)
    for m in DIRECTIVE.finditer(code):
        ln = _line_of(code, m.start())
        findings.append(Finding("data-directive", REFUSE, ln, lines[ln - 1]))
    for m in PRAGMA.finditer(code):
        name = m.group(1)
        ln = _line_of(code, m.start())
        if CODE_PRAGMA.search(name):
            findings.append(Finding("pragma-code", REFUSE, ln, lines[ln - 1]))
        elif name not in KNOWN_PRAGMAS and not TEXT_SECTION.search(m.group(0)):
            findings.append(Finding("pragma-unknown", WARN, ln, lines[ln - 1]))
    for m in TEXT_SECTION.finditer(code):
        ln = _line_of(code, m.start())
        findings.append(Finding("text-section", REFUSE, ln, lines[ln - 1]))
    shown_at = {f.line for f in findings if f.kind == "text-section"}
    findings += [f for f in section_name_findings(code, lines) if f.line not in shown_at]
    for m in INCLUDE.finditer(code):
        name = INCLUDE_NAME.match(m.group(1).strip())
        if not name or Path(name.group(1) or name.group(2)).suffix.lower() not in HEADER_EXT:
            ln = _line_of(code, m.start())
            findings.append(Finding("binary-include", REFUSE, ln, lines[ln - 1]))
    if tree == "src" and header is not None and "extra" in header:
        findings.append(Finding("extra-header", REFUSE, 1, f'extra="{header["extra"]}"'))
    return sorted(findings, key=lambda f: (f.line, f.kind))


def tree_of(path: Path, root: Path = ROOT) -> str:
    try:
        rel = path.resolve().relative_to(root.resolve())
    except ValueError:
        return "other"
    return rel.parts[0] if rel.parts else "other"


def included_findings(path: Path, root: Path = ROOT, seen: set | None = None,
                      dirs: tuple[Path, ...] = ()) -> list[Finding]:
    """The refusals inside the repository files `path` includes, transitively: the compiler reads a header as part of
    the source, so an asm statement or a .text section in it is one in the source. `"x"` resolves in the including
    file's directory, the source's directory, then include/ (MWCIncludes); `<x>` in include/; a file outside the
    repository (a compiler header) is not read. Each refusal is reported on the line of the source's #include."""
    seen = set() if seen is None else seen
    dirs = dirs or (path.parent,)
    text = path.read_text(encoding="utf-8", errors="replace")
    code, lines = strip_comments(splice_lines(text)), text.split("\n")
    out: list[Finding] = []
    for m in INCLUDE.finditer(code):
        nm = INCLUDE_NAME.match(m.group(1).strip())
        if not nm:
            continue
        name = nm.group(1) or nm.group(2)
        options = ([path.parent / name, *(d / name for d in dirs)] if nm.group(1) else []) + [root / "include" / name]
        hit = next((c.resolve() for c in options if c.is_file()), None)
        if hit is None or hit in seen or not hit.is_relative_to(root.resolve()):
            continue
        seen.add(hit)
        inner = refusals(_check_header(hit.read_text(encoding="utf-8", errors="replace"), root))
        inner += included_findings(hit, root, seen, dirs)
        ln = _line_of(code, m.start())
        rel = hit.relative_to(root.resolve()).as_posix()
        out += [Finding(f.kind, REFUSE, ln, f"{rel}:{f.line}: {f.text.strip()}") for f in inner]
    return out


def check_file(path: Path, root: Path = ROOT) -> list[Finding]:
    text = path.read_text(encoding="utf-8", errors="replace")
    header = None
    m = re.search(r"/\*\s*LBPMATCH\s+(.*?)\*/", text[:2000], re.S)
    if m:
        header = {k: v.strip('"') for k, v in re.findall(r'(\w+)=("[^"]*"|\S+)', m.group(1))}
    found = check_text(text, header, tree_of(path, root)) + included_findings(path, root)
    return sorted(found, key=lambda f: (f.line, f.kind))


def _check_header(text: str, root: Path) -> list[Finding]:
    """A header: no LBPMATCH header, so no VFPU allowance and no extra= key."""
    return check_text(text, None, "include")


def refusals(findings: list[Finding]) -> list[Finding]:
    return [f for f in findings if f.severity == REFUSE]


def summary(findings: list[Finding]) -> str:
    """'kind1,kind2' for the refusals (used as the detail of a failed gate)."""
    return ",".join(sorted({f.kind for f in refusals(findings)}))


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", type=Path)
    ap.add_argument("--all", action="store_true", help="every source under src/")
    ap.add_argument("--wip", action="store_true", help="with --all, wip/ as well")
    ap.add_argument("--show", choices=("idiom", "warn"), help="also list idioms or warnings")
    args = ap.parse_args()
    paths = list(args.files)
    if args.all:
        trees = ["src"] + (["wip"] if args.wip else [])
        paths += sorted(p for t in trees for p in (ROOT / t).rglob("*") if p.suffix in (".c", ".cpp"))
    if not paths:
        ap.error("name files or pass --all")
    refused = 0
    counts: dict[str, int] = {}
    for p in paths:
        findings = check_file(p)
        for f in findings:
            counts[f.kind] = counts.get(f.kind, 0) + 1
            shown = {REFUSE} | ({IDIOM} if args.show in ("idiom", "warn") else set()) | ({WARN} if args.show == "warn" else set())
            if f.severity in shown:
                print(f"{p.resolve().relative_to(ROOT).as_posix() if p.resolve().is_relative_to(ROOT) else p}: {f}")
        refused += bool(refusals(findings))
    print(f"checked {len(paths)} files: {refused} refused; findings by kind: {counts or 'none'}")
    return 1 if refused else 0


if __name__ == "__main__":
    raise SystemExit(main())
