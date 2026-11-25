#!/usr/bin/env python3
"""The second, independent verifier: the matcher's bytes, the accepted ledger and every published figure, checked
again by code that shares nothing with the tools that produced them.

    python tools/lbpxverify.py bytes [paths... | --all] [--json FILE]   the matcher's linked bytes against retail
    python tools/lbpxverify.py ledger [--json FILE]                     the accepted ledger against retail and the tree
    python tools/lbpxverify.py figures [--tree DIR] [--ledger FILE]      every published figure, recounted

INDEPENDENCE. This file imports none of the project's tools except tools/lbpconfig.py, and that only to find the
campaign directory. It parses the LBPMATCH header, the boundary lists, ELF images (headers, program headers,
sections, symbols), the accepted ledger, the figure files and the objdiff report itself; tools/test_lbpxverify.py
checks that it imports nothing else and that its build-directory key equals the matcher's. The design follows
hst-decomp's tools/hstxverify.py; the code is this project's own.

bytes (needs your own copy of the game and the matcher's build products: run tools/lbpmatch.py first). For each
source: its header span must start at a function start and end at a function end of data/functions-<module>.txt;
build/<module>/<key>/linked.elf, written by tools/lbpmatch.py and not older than the source, must define the header
symbol only at the header start; the linked code of the span (the whole function when the compiled function is
longer than the span) must equal the retail bytes, read from the decrypted module through its own program headers
after its SHA-256 and size are checked against data/modules.json. A `helpers=` source has every function checked at
its own address, its section padded with the linker's zero fill as the matcher does. Placed .rodata/.data/.sdata
(header keys) are compared too, and placed .bss/.sbss must sit at their header address. `.gnu.linkonce` data
objects are not compared by this verifier (the figures count code only). A source with `extra=` is refused. Status:
MATCH, DIFFER, REFUSED (not eligible, or an artifact is missing or stale) or ERROR; exit 1 unless all MATCH.

ledger (the maintainer's tree: data/accepted-ledger.jsonl, and the game). The ledger's header must match its body
and counts; every proof must be EXACT, its range whole functions of the boundary list, no two proofs may overlap,
its source must have the recorded SHA-256, every compared region (code, placed data, linkonce objects) must hash
to the recorded SHA-256 in the retail module (code only from an executable segment), and where the matcher's
artifact of that source is present and current its linked code must hash to the same digest.

figures (no game needed: any public tree, by default the one this file sits in). From data/public-functions.txt
(the public scope's functions behind the figures) and data/functions-<module>.txt it recounts the public scope and
the matched bytes and functions of every module, and compares them with data/public-figures.json,
docs/objdiff-report.json (top-level and per-module measures, and the units' sums), the README's figure blocks,
progress.json (the strict badge over data/modules.json), the ladder block of docs/COMPLETION.md and the T1
milestones of docs/milestones.json (including that every REGRESSED flag follows from its record). Every row's source
must exist, be listed in ORIGINS.json under a published class, and carry an LBPMATCH header with that module, start
and symbol. With the accepted ledger (--ledger, or data/accepted-ledger.jsonl of this tree when present), every
matched row must be an EXACT proof of that source at that address with those matched bytes. Exit 1 on any
disagreement.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpconfig as CFG  # noqa: E402  (path lookup only)

SHT_PROGBITS, SHT_SYMTAB, SHT_NOBITS = 1, 2, 8
SHF_ALLOC, SHF_EXECINSTR = 2, 4
PT_LOAD, PF_X = 1, 1
HEADER_KEYS = {"module", "start", "end", "symbol", "lang", "cc", "asm", "rodata", "data", "sdata", "bss", "sbss",
               "helpers", "extra"}
PLACED = ("rodata", "data", "sdata")
NOBITS = ("bss", "sbss")
HELPER = re.compile(r"([A-Za-z_]\w*)@0x([0-9A-Fa-f]{1,8})-0x([0-9A-Fa-f]{1,8})")
# origin classes the public tree never carries (the publish policy of docs/COMPLETION.md and ORIGINS.json)
UNPUBLISHED_CLASSES = frozenset({"startup", "sony-sdk", "sony-prx", "unattributed"})


class Refusal(Exception):
    """The input is not eligible for this verifier's check."""


class FormatError(Exception):
    """An ELF image or a data file is malformed."""


# --------------------------------------------------------------------------------------------------------------
# ELF32 little-endian

@dataclass(frozen=True)
class Section:
    index: int
    name: str
    type: int
    flags: int
    addr: int
    offset: int
    size: int
    link: int


@dataclass(frozen=True)
class Symbol:
    name: str
    value: int
    size: int
    info: int
    shndx: int


class Elf:
    def __init__(self, data: bytes, label: str = "ELF") -> None:
        self.data, self.label = data, label
        if len(data) < 52 or data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
            raise FormatError(f"{label}: not an ELF32 little-endian image")
        (self.type, machine, _version, _entry, phoff, shoff, self.flags, _ehsize, phentsize, phnum, shentsize,
         shnum, shstrndx) = struct.unpack_from("<HHIIIIIHHHHHH", data, 16)
        if machine != 8:
            raise FormatError(f"{label}: machine {machine}, not MIPS")
        if phnum and (phentsize < 32 or phoff + phnum * phentsize > len(data)):
            raise FormatError(f"{label}: program headers out of bounds")
        if shnum and (shentsize < 40 or shoff + shnum * shentsize > len(data)):
            raise FormatError(f"{label}: section headers out of bounds")
        self.segments = [struct.unpack_from("<IIIIIIII", data, phoff + i * phentsize) for i in range(phnum)]
        raw = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
        if shnum and not 0 <= shstrndx < shnum:
            raise FormatError(f"{label}: no section name table")
        names = raw[shstrndx][4] if shnum else 0
        self.sections = []
        for i, (name, typ, flags, addr, off, size, link, _info, _align, _entsize) in enumerate(raw):
            if typ != SHT_NOBITS and off + size > len(data):
                raise FormatError(f"{label}: section {i} out of bounds")
            self.sections.append(Section(i, self._cstr(names + name), typ, flags, addr, off, size, link))

    def _cstr(self, off: int) -> str:
        end = self.data.find(b"\0", off)
        if off >= len(self.data) or end < 0:
            raise FormatError(f"{self.label}: string out of bounds")
        return self.data[off:end].decode("latin-1")

    def contents(self, sec: Section) -> bytes:
        return b"" if sec.type == SHT_NOBITS else self.data[sec.offset:sec.offset + sec.size]

    def symbols(self) -> list[Symbol]:
        out = []
        for sec in self.sections:
            if sec.type != SHT_SYMTAB:
                continue
            strtab = self.sections[sec.link]
            for k in range(sec.size // 16):
                name, value, size, info, _other, shndx = struct.unpack_from("<IIIBBH", self.data, sec.offset + 16 * k)
                out.append(Symbol(self._cstr(strtab.offset + name), value, size, info, shndx))
        return out

    def image_range(self, start: int, end: int, executable: bool) -> bytes:
        """Bytes [start, end) from one file-backed PT_LOAD segment (an executable one when `executable`)."""
        for kind, off, vaddr, _paddr, filesz, _memsz, flags, _align in self.segments:
            if kind == PT_LOAD and (flags & PF_X or not executable) and vaddr <= start and end <= vaddr + filesz:
                return self.data[off + start - vaddr:off + end - vaddr]
        raise Refusal(f"{self.label}: [0x{start:08x}, 0x{end:08x}) is not inside one file-backed "
                      f"{'executable ' if executable else ''}load segment")


# --------------------------------------------------------------------------------------------------------------
# the project's files, read here once more

def build_key(path: Path, root: Path = ROOT) -> str:
    """The matcher's build directory name (`<folder>__<stem>__<SHA-1 of the relative path, 10 digits>`), written out
    again because this file must not import tools/lbpmatch.py (tools/test_lbpxverify.py checks they agree)."""
    p = Path(path).resolve()
    try:
        ident = p.relative_to(root).as_posix()
    except ValueError:
        ident = p.as_posix()
    return f"{p.parent.name}__{p.stem}__{hashlib.sha1(os.path.normcase(ident).encode('utf-8')).hexdigest()[:10]}"


def parse_header(text: str, label: str = "source") -> dict:
    """The LBPMATCH header of a source's first lines: every key known, start/end and placements hexadecimal."""
    m = re.search(r"/\*\s*LBPMATCH\s+(.*?)\*/", text[:2000], re.S)
    if not m:
        raise Refusal(f"{label}: no LBPMATCH header")
    fields: dict[str, str] = {}
    for key, value in re.findall(r'(\w+)=("[^"]*"|\S+)', m.group(1)):
        if key in fields:
            raise Refusal(f"{label}: header key {key} given twice")
        fields[key] = value.strip('"')
    unknown = sorted(set(fields) - HEADER_KEYS)
    if unknown:
        raise Refusal(f"{label}: unknown header key(s) {', '.join(unknown)}")
    for key in ("module", "start", "end", "symbol"):
        if not fields.get(key):
            raise Refusal(f"{label}: the header has no {key}")
    try:
        out = {"module": fields["module"], "symbol": fields["symbol"], "start": int(fields["start"], 16),
               "end": int(fields["end"], 16), "extra": fields.get("extra"),
               "placed": {k: int(fields[k], 16) for k in PLACED + NOBITS if k in fields}}
    except ValueError:
        raise Refusal(f"{label}: start, end and placements must be hexadecimal") from None
    if not out["start"] < out["end"]:
        raise Refusal(f"{label}: empty or reversed span")
    helpers = []
    for item in filter(None, (fields.get("helpers") or "").split(",")):
        h = HELPER.fullmatch(item.strip())
        if not h:
            raise Refusal(f"{label}: bad helpers= entry {item!r}")
        helpers.append((h.group(1), int(h.group(2), 16), int(h.group(3), 16)))
    out["functions"] = [(out["symbol"], out["start"], out["end"])] + helpers
    spans = sorted(out["functions"], key=lambda f: f[1])
    if any(b[1] < a[2] for a, b in zip(spans, spans[1:])) or any(not s < e for _n, s, e in helpers):
        raise Refusal(f"{label}: helper ranges overlap or are empty")
    return out


def boundary_list(path: Path) -> tuple[set[int], set[int], dict[int, int]]:
    """(starts, ends, {start: end}) of a boundary list (`start end [status]` in hex per line)."""
    starts, ends, table = set(), set(), {}
    for n, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        cols = line.split()
        try:
            s, e = int(cols[0], 16), int(cols[1], 16)
        except (ValueError, IndexError):
            raise FormatError(f"{path.name}:{n}: not `start end`") from None
        if not s < e or s in table:
            raise FormatError(f"{path.name}:{n}: an empty or repeated function")
        starts.add(s)
        ends.add(e)
        table[s] = e
    return starts, ends, table


def whole_functions(bounds: tuple[set[int], set[int], dict] | None, start: int, end: int, label: str) -> None:
    if bounds is None:
        raise Refusal(f"{label}: no boundary list for its module")
    if start not in bounds[0] or end not in bounds[1]:
        raise Refusal(f"{label}: [0x{start:08x}, 0x{end:08x}) is not whole functions of the boundary list")


def module_identities(root: Path) -> dict[str, dict]:
    doc = json.loads((root / "data" / "modules.json").read_text(encoding="utf-8"))
    return {m["name"]: m for m in doc["modules"]}


def retail_images(root: Path = ROOT) -> dict[str, Elf]:
    """The decrypted retail modules through the campaign (lbp-config campaign_root), each checked against the SHA-256
    and size of data/modules.json."""
    campaign = CFG.host_path("campaign_root")
    ledger = json.loads((campaign / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    selection = json.loads((campaign / "inputs" / "revision-selection.json").read_text(encoding="utf-8-sig"))
    want = module_identities(root)
    out = {}
    for m in ledger["modules"]:
        if m["name"] not in want:
            continue
        data = (Path(selection["roots"][m["input"]["root"]]) / m["input"]["path"]).read_bytes()
        if len(data) != want[m["name"]]["size"] or hashlib.sha256(data).hexdigest() != want[m["name"]]["sha256"]:
            raise FormatError(f"{m['name']}: the campaign's image is not the module data/modules.json identifies")
        out[m["name"]] = Elf(data, f"retail {m['name']}")
    return out


# --------------------------------------------------------------------------------------------------------------
# bytes: the matcher's linked image against retail

def word_differences(ours: bytes, theirs: bytes) -> list[int]:
    n = max(len(ours), len(theirs))
    return [i for i in range(0, n, 4) if ours[i:i + 4] != theirs[i:i + 4]]


def compare_linked(linked: Elf, retail: Elf, header: dict) -> dict:
    """MATCH or DIFFER for one source's linked image, with what was compared."""
    defs: dict[str, list[int]] = {}
    sizes: dict[str, int] = {}
    for s in linked.symbols():
        if s.shndx and s.name:
            defs.setdefault(s.name, []).append(s.value)
            sizes.setdefault(s.name, s.size)
    helpers = len(header["functions"]) > 1
    compared, differing, first = 0, 0, None
    regions = []
    for name, start, end in header["functions"]:
        span = end - start
        if defs.get(name) != [start]:
            raise Refusal(f"{name} is not linked once at its retail address 0x{start:08x} (at "
                          f"{', '.join(f'0x{a:08x}' for a in defs.get(name, [])) or 'nowhere'})")
        code = [s for s in linked.sections if s.flags & SHF_ALLOC and s.flags & SHF_EXECINSTR
                and s.type == SHT_PROGBITS and s.addr <= start < s.addr + s.size]
        if len(code) != 1:
            raise Refusal(f"{name}: {len(code)} linked code sections hold 0x{start:08x}")
        sec = code[0]
        if helpers:
            if sec.addr != start:
                raise Refusal(f"{name}: its section does not start at 0x{start:08x}")
            body = linked.contents(sec)
            body += bytes(max(0, span - len(body)))         # the linker's zero fill up to the next function
        else:
            want = max(span, sizes.get(name, 0))
            body = linked.contents(sec)[start - sec.addr:start - sec.addr + want]
        target = retail.image_range(start, end, executable=True)
        diffs = word_differences(body, target)
        compared += span
        differing += len(diffs)
        if diffs and first is None:
            first = start + diffs[0]
        regions.append({"what": ".text" if name == header["symbol"] else f".text:{name}", "address": f"0x{start:08x}",
                        "bytes": span, "linked_bytes": len(body), "differing_words": len(diffs),
                        "sha256": hashlib.sha256(body).hexdigest()})
    for key, addr in header["placed"].items():
        secs = [s for s in linked.sections if s.name == "." + key and s.flags & SHF_ALLOC and s.size]
        if not secs:
            continue                                         # the object has no such section: nothing is placed
        if len(secs) != 1 or secs[0].addr != addr:
            raise Refusal(f".{key} is not linked at its header address 0x{addr:08x}")
        if key in NOBITS:
            continue
        ours = linked.contents(secs[0])
        diffs = word_differences(ours, retail.image_range(addr, addr + len(ours), executable=False))
        compared += len(ours)
        differing += len(diffs)
        if diffs and first is None:
            first = addr + diffs[0]
        regions.append({"what": "." + key, "address": f"0x{addr:08x}", "bytes": len(ours),
                        "differing_words": len(diffs), "sha256": hashlib.sha256(ours).hexdigest()})
    return {"status": "DIFFER" if differing else "MATCH", "compared_bytes": compared, "differing_words": differing,
            "first_difference": None if first is None else f"0x{first:08x}", "regions": regions}


def verify_source(path: Path, retail: dict[str, Elf], bounds: dict, root: Path = ROOT,
                  build_root: Path | None = None) -> dict:
    rel = path.relative_to(root).as_posix() if path.is_relative_to(root) else str(path)
    rec: dict = {"source": rel, "status": "ERROR"}
    try:
        h = parse_header(path.read_text(encoding="utf-8", errors="replace"), rel)
        rec.update({"module": h["module"], "start": f"0x{h['start']:08x}", "end": f"0x{h['end']:08x}",
                    "symbol": h["symbol"]})
        if h["extra"] is not None:
            raise Refusal(f"{rel}: extra= flags never count")
        if h["module"] not in retail:
            raise Refusal(f"{rel}: no retail image of {h['module']}")
        for name, s, e in h["functions"]:
            whole_functions(bounds.get(h["module"]), s, e, f"{rel} ({name})")
        linked_path = (build_root or root / "build") / h["module"] / build_key(path, root) / "linked.elf"
        if not linked_path.is_file():
            raise Refusal(f"{rel}: no matcher artifact (run tools/lbpmatch.py)")
        if linked_path.stat().st_mtime_ns < path.stat().st_mtime_ns:
            raise Refusal(f"{rel}: the matcher artifact is older than the source")
        rec.update(compare_linked(Elf(linked_path.read_bytes(), "linked.elf"), retail[h["module"]], h))
    except Refusal as exc:
        rec.update({"status": "REFUSED", "reason": str(exc)})
    except (OSError, FormatError, struct.error) as exc:
        rec.update({"status": "ERROR", "reason": str(exc)})
    return rec


def load_bounds(root: Path, modules: list[str]) -> dict:
    out = {}
    for m in modules:
        p = root / "data" / f"functions-{m}.txt"
        if p.is_file():
            out[m] = boundary_list(p)
    return out


# --------------------------------------------------------------------------------------------------------------
# ledger: the accepted proofs against retail and the tree

def read_ledger(path: Path) -> tuple[dict, list[dict]]:
    data = path.read_bytes()
    lines = data.split(b"\n")
    if not data.endswith(b"\n") or len(lines) < 2:
        raise FormatError(f"{path.name}: truncated")
    lines = lines[:-1]
    header = json.loads(lines[0])
    body = b"".join(x + b"\n" for x in lines[1:])
    if header.get("body_sha256") != hashlib.sha256(body).hexdigest():
        raise FormatError(f"{path.name}: the body does not match its header")
    proofs = [json.loads(x) for x in lines[1:]]
    if header.get("count") != len(proofs):
        raise FormatError(f"{path.name}: the header counts {header.get('count')} proofs, the body has {len(proofs)}")
    return header, proofs


def ledger_problems(proofs: list[dict], root: Path, retail: dict[str, Elf] | None, bounds: dict,
                    build_root: Path | None = None) -> list[str]:
    out = []
    last_end: dict[str, int] = {}
    for p in sorted(proofs, key=lambda q: (q["module"], int(q["start"], 16))):
        label = f"{p.get('source')} ({p['module']} {p['start']})"
        s, e = int(p["start"], 16), int(p["end"], 16)
        if p.get("status") != "EXACT":
            out.append(f"{label}: status {p.get('status')}")
        if s < last_end.get(p["module"], 0):
            out.append(f"{label}: overlaps the proof before it")
        last_end[p["module"]] = max(e, last_end.get(p["module"], 0))
        try:
            whole_functions(bounds.get(p["module"]), s, e, label)
        except Refusal as exc:
            out.append(str(exc))
        src = root / p["source"]
        if not src.is_file():
            out.append(f"{label}: the source is not in the tree")
            continue
        if hashlib.sha256(src.read_bytes()).hexdigest() != p.get("source_sha256"):
            out.append(f"{label}: the source differs from the one the proof names")
        if retail is None:
            continue
        image = retail.get(p["module"])
        if image is None:
            out.append(f"{label}: no retail image of {p['module']}")
            continue
        for what, rec in sorted(p.get("compared", {}).items()):
            a = int(rec["addr"], 16)
            try:
                got = image.image_range(a, a + rec["size"], executable=what.split(":")[0] == ".text")
            except Refusal as exc:
                out.append(f"{label} {what}: {exc}")
                continue
            if hashlib.sha256(got).hexdigest() != rec["sha256"]:
                out.append(f"{label} {what}: the retail bytes at 0x{a:08x} do not have the recorded digest")
        text = p.get("compared", {}).get(".text")
        artifact = (build_root or root / "build") / p["module"] / build_key(src, root) / "linked.elf"
        if text and artifact.is_file() and artifact.stat().st_mtime_ns >= src.stat().st_mtime_ns:
            try:
                linked = Elf(artifact.read_bytes(), "linked.elf")
                sec = next(x for x in linked.sections if x.flags & SHF_EXECINSTR and x.type == SHT_PROGBITS
                           and x.addr <= s < x.addr + x.size)
                body = linked.contents(sec)[s - sec.addr:s - sec.addr + text["size"]]
                if hashlib.sha256(body).hexdigest() != text["sha256"]:
                    out.append(f"{label}: the matcher's linked code does not have the proof's digest")
            except (StopIteration, FormatError, struct.error) as exc:
                out.append(f"{label}: the matcher's artifact does not load ({exc!r})")
    return out


# --------------------------------------------------------------------------------------------------------------
# figures: every published figure, recounted

def read_rows(path: Path) -> dict[str, list[tuple[int, int, int, str | None, str | None]]]:
    rows: dict[str, list] = {}
    for n, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        if not raw.strip() or raw.startswith("#"):
            continue
        cols = raw.split()
        if len(cols) != 6:
            raise FormatError(f"{path.name}:{n}: not `module start size matched source name`")
        module, start, size, matched, source, name = cols
        rows.setdefault(module, []).append((int(start, 16), int(size), int(matched), None if source == "-" else source,
                                            None if name == "-" else name))
    return rows


def recount(rows: dict[str, list]) -> dict[str, dict]:
    """{module: scope bytes, matched bytes, functions, matched functions} from the rows."""
    return {m: {"scope": sum(r[1] for r in rs), "matched": sum(r[2] for r in rs), "functions": len(rs),
                "matched_functions": sum(1 for r in rs if r[3])} for m, rs in rows.items()}


def share(part: int, whole: int) -> float:
    return round(100 * part / whole, 3) if whole else 0.0


def thousands(n: int) -> str:
    return f"{n:,}"


def figure_problems(tree: Path, proofs: list[dict] | None = None) -> list[str]:
    out: list[str] = []
    figures = json.loads((tree / "data" / "public-figures.json").read_text(encoding="utf-8"))
    rows = read_rows(tree / "data" / "public-functions.txt")
    counts = recount(rows)
    if set(counts) != set(figures):
        out.append(f"the rows list {sorted(counts)}, the figures {sorted(figures)}")
    origins = json.loads((tree / "ORIGINS.json").read_text(encoding="utf-8")) if (tree / "ORIGINS.json").is_file() else {}
    for module, rs in rows.items():
        bounds = (boundary_list(tree / "data" / f"functions-{module}.txt")
                  if (tree / "data" / f"functions-{module}.txt").is_file() else None)
        starts = [r[0] for r in rs]
        if starts != sorted(set(starts)):
            out.append(f"{module}: the rows are not in address order or repeat a function")
        for start, size, matched, source, name in rs:
            where = f"{module} 0x{start:08x}"
            if bounds is not None and bounds[2].get(start) != start + size:
                out.append(f"{where}: not a function of data/functions-{module}.txt")
            if not 0 <= matched <= size or bool(source) != bool(matched):
                out.append(f"{where}: {matched} matched bytes of {size} with source {source or '-'}")
            if not source:
                continue
            if origins.get(source) in UNPUBLISHED_CLASSES or source not in origins:
                out.append(f"{where}: {source} is not a published source of ORIGINS.json")
            path = tree / source
            if not path.is_file():
                out.append(f"{where}: {source} is not in the tree")
                continue
            try:
                h = parse_header(path.read_text(encoding="utf-8", errors="replace"), source)
            except Refusal as exc:
                out.append(f"{where}: {exc}")
                continue
            if (h["module"], h["start"], h["symbol"]) != (module, start, name) or h["end"] < start + matched:
                out.append(f"{where}: {source}'s header says {h['module']} 0x{h['start']:08x}-0x{h['end']:08x} "
                           f"{h['symbol']}")
    for module, fig in figures.items():
        c = counts.get(module)
        if c is None:
            continue
        got = (fig.get("public_scope_bytes"), fig.get("public_t1_bytes"), fig.get("public_scope_functions"),
               fig.get("public_t1_functions"), fig.get("public_t1_percent"))
        want = (c["scope"], c["matched"], c["functions"], c["matched_functions"], share(c["matched"], c["scope"]))
        if got != want:
            out.append(f"data/public-figures.json {module}: {got}, recounted {want} (scope bytes, matched bytes, "
                       "functions, matched functions, percent)")
    total = {k: sum(c[k] for c in counts.values()) for k in ("scope", "matched", "functions", "matched_functions")}
    out += report_problems(tree, counts, total)
    out += readme_problems(tree, counts, total)
    out += ladder_problems(tree, figures, total)
    out += milestone_problems(tree, counts)
    if proofs is not None:
        owners = {(p["module"], int(p["start"], 16)): p for p in proofs if p.get("status") == "EXACT"}
        for module, rs in rows.items():
            for start, size, matched, source, _name in rs:
                p = owners.get((module, start))
                if source and (p is None or p.get("source") != source
                               or min(int(p["end"], 16) - start, size) != matched):
                    out.append(f"{module} 0x{start:08x}: {source} is not an accepted proof with {matched} bytes here")
                elif not source and p is not None and origins.get(p.get("source")) not in (None, *UNPUBLISHED_CLASSES):
                    out.append(f"{module} 0x{start:08x}: the published source {p.get('source')} has an accepted proof "
                               "here, but the figures do not count it")
    return out


def report_problems(tree: Path, counts: dict, total: dict) -> list[str]:
    path = tree / "docs" / "objdiff-report.json"
    if not path.is_file():
        return ["docs/objdiff-report.json is missing"]
    rep = json.loads(path.read_text(encoding="utf-8"))
    out = []

    def check(where: str, m: dict, c: dict) -> None:
        got = (int(m.get("total_code", -1)), int(m.get("matched_code", -1)), m.get("total_functions"),
               m.get("matched_functions"), m.get("matched_code_percent"))
        want = (c["scope"], c["matched"], c["functions"], c["matched_functions"], share(c["matched"], c["scope"]))
        if got != want:
            out.append(f"docs/objdiff-report.json {where}: {got}, recounted {want}")

    check("measures", rep.get("measures", {}), total)
    cats = {c.get("id"): c for c in rep.get("categories", [])}
    for module, c in counts.items():
        if module not in cats:
            out.append(f"docs/objdiff-report.json has no category {module}")
            continue
        check(f"category {module}", cats[module]["measures"], c)
        units = [u for u in rep.get("units", []) if u.get("metadata", {}).get("module_name") == module]
        got = (sum(int(u["measures"].get("total_code", "0")) for u in units),
               sum(int(u["measures"].get("matched_code", "0")) for u in units),
               sum(u["measures"].get("total_functions", 0) for u in units),
               sum(u["measures"].get("matched_functions", 0) for u in units))
        if got != (c["scope"], c["matched"], c["functions"], c["matched_functions"]):
            out.append(f"docs/objdiff-report.json {module}: its units add up to {got}")
    return out


def readme_problems(tree: Path, counts: dict, total: dict) -> list[str]:
    path = tree / "README.md"
    text = path.read_text(encoding="utf-8") if path.is_file() else ""
    if "<!-- lbpfigures:begin" not in text:
        return []                                       # a README without figure blocks publishes no figure
    out = []
    code = re.search(r"\*\*Code matched\*\* \(T1\) \| \*\*([\d.]+)%\*\* \| ([\d,]+) of the ([\d,]+) code bytes", text)
    fns = re.search(r"\*\*Functions matched\*\* \(T1\) \| \*\*([\d.]+)%\*\* \| ([\d,]+) of the ([\d,]+) functions", text)
    want_code = (f"{100 * total['matched'] / total['scope'] if total['scope'] else 0:.1f}", thousands(total["matched"]),
                 thousands(total["scope"]))
    want_fns = (f"{100 * total['matched_functions'] / total['functions'] if total['functions'] else 0:.1f}",
                thousands(total["matched_functions"]), thousands(total["functions"]))
    if not code or code.groups() != want_code:
        out.append(f"README.md code matched: {code.groups() if code else 'missing'}, recounted {want_code}")
    if not fns or fns.groups() != want_fns:
        out.append(f"README.md functions matched: {fns.groups() if fns else 'missing'}, recounted {want_fns}")
    for module, c in counts.items():
        row = (f"| `{module}` | {thousands(c['matched_functions'])} / {thousands(c['functions'])} | "
               f"{thousands(c['matched'])} / {thousands(c['scope'])} | {share(c['matched'], c['scope']):.3f} % |")
        if row not in text:
            out.append(f"README.md has no progress row {row}")
    return out


def ladder_problems(tree: Path, figures: dict, total: dict) -> list[str]:
    """progress.json and the ladder block against T1 recounted over the strict scope of data/modules.json."""
    have_badge = (tree / "progress.json").is_file()
    completion = tree / "docs" / "COMPLETION.md"
    text = completion.read_text(encoding="utf-8") if completion.is_file() else ""
    if not have_badge and "lbpprogress:begin" not in text:
        return []
    scope = sum(m["size"] for m in module_identities(tree).values())
    t1 = total["matched"]
    reviewed = 0                                        # T4 = strict: no review is recorded yet
    out = []
    if have_badge:
        doc = json.loads((tree / "progress.json").read_text(encoding="utf-8"))
        want = f"{100 * reviewed / scope:.1f}% (tier 1: {100 * t1 / scope:.1f}%)"
        if doc.get("message") != want or doc.get("label") != "strict match":
            out.append(f"progress.json says {doc.get('message')!r}, recounted {want!r}")
    else:
        out.append("progress.json is missing")
    if "lbpprogress:begin" in text:
        if f"| T1: function matches (published sources) | {thousands(t1)} B | {share(t1, scope):.3f}% |" not in text:
            out.append("docs/COMPLETION.md: the T1 row of the ladder does not match the recount")
        if f"Share of the strict scope ({thousands(scope)} B)" not in text:
            out.append("docs/COMPLETION.md: the ladder's strict scope does not match data/modules.json")
        for n, label in ((2, "whole-module build"), (3, "shifted build")):
            measured = [f for f in figures.values() if isinstance(f.get(f"t{n}"), dict)]
            if not measured and f"| T{n}: {label} | not measured |" not in text:
                out.append(f"docs/COMPLETION.md: T{n} shows a figure the published figures do not measure")
        if f"| T4: strict (reviewed) | 0 B | 0.000% |" not in text:
            out.append("docs/COMPLETION.md: T4 shows a figure no review supports")
    return out


def milestone_problems(tree: Path, counts: dict) -> list[str]:
    path = tree / "docs" / "milestones.json"
    defs_path = tree / "data" / "milestones.json"
    if not path.is_file() or not defs_path.is_file():
        return []
    doc = json.loads(path.read_text(encoding="utf-8"))
    defs = {m["id"]: m for m in json.loads(defs_path.read_text(encoding="utf-8"))["milestones"]}
    all_names = list(module_identities(tree))
    out = []
    for r in doc.get("milestones", []):
        progress, reached = r.get("progress"), r.get("reached")
        if bool(r.get("regressed")) != (bool(reached) and (progress is None or progress < 100.0)):
            out.append(f"docs/milestones.json {r.get('id')}: the REGRESSED flag does not follow from the record")
        d = defs.get(r.get("id"))
        if d is None:
            out.append(f"docs/milestones.json {r.get('id')}: not a milestone of data/milestones.json")
            continue
        if d["measure"] != "t1":
            continue
        names = [n for n in (all_names if d["modules"] == "all" else d["modules"]) if n in counts]
        done = sum(counts[n]["matched"] for n in names)
        whole = sum(counts[n]["scope"] for n in names)
        want = None if not names else 100.0 if whole and done >= whole else \
            min(round(100 * done / whole, 3), 99.999) if whole else 0.0
        if progress != want:
            out.append(f"docs/milestones.json {r['id']}: progress {progress!r}, recounted {want!r}")
    return out


# --------------------------------------------------------------------------------------------------------------

def sources(paths: list[Path], all_sources: bool, root: Path) -> list[Path]:
    found = sorted(p for p in (root / "src").rglob("*")) if all_sources else \
        [p if p.is_absolute() else Path.cwd() / p for p in paths]
    return [p.resolve() for p in found if p.is_file() and p.suffix in (".c", ".cpp")]


def emit(report: dict, json_path: Path | None) -> None:
    if json_path:
        json_path.parent.mkdir(parents=True, exist_ok=True)
        json_path.write_text(json.dumps(report, indent=1, sort_keys=True) + "\n", encoding="utf-8")
        print(f"JSON={json_path}")


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)
    b = sub.add_parser("bytes", help="the matcher's linked bytes against retail")
    b.add_argument("paths", nargs="*", type=Path)
    b.add_argument("--all", action="store_true", help="every source under src/")
    b.add_argument("--json", type=Path)
    led = sub.add_parser("ledger", help="the accepted ledger against retail and the tree")
    led.add_argument("--ledger", type=Path, default=ROOT / "data" / "accepted-ledger.jsonl")
    led.add_argument("--json", type=Path)
    f = sub.add_parser("figures", help="every published figure, recounted from the rows")
    f.add_argument("--tree", type=Path, default=ROOT)
    f.add_argument("--ledger", type=Path, help="also check the rows against this accepted ledger")
    args = ap.parse_args(argv)
    if args.command == "figures":
        tree = args.tree.resolve()
        if not (tree / "data" / "public-figures.json").is_file():
            print(f"no {tree / 'data' / 'public-figures.json'}: pass --tree with a public tree")
            return 2
        ledger_path = args.ledger or (tree / "data" / "accepted-ledger.jsonl")
        proofs = read_ledger(ledger_path)[1] if ledger_path.is_file() else None
        problems = figure_problems(tree, proofs)
        print(f"figures of {tree}: " + ("every published figure agrees with the recount" if not problems
                                        else f"{len(problems)} disagreement(s)")
              + ("" if proofs is None else f"; rows checked against {len(proofs)} accepted proofs"))
        for p in problems[:100]:
            print("  " + p)
        return 1 if problems else 0
    modules = list(module_identities(ROOT))
    bounds = load_bounds(ROOT, modules)
    if args.command == "ledger":
        if not args.ledger.is_file():
            print(f"no {args.ledger}: the accepted ledger is the maintainer's")
            return 2
        header, proofs = read_ledger(args.ledger)
        problems = ledger_problems(proofs, ROOT, retail_images(ROOT), bounds)
        emit({"schema": "lbpxverify.ledger.v1", "proofs": len(proofs), "problems": problems}, args.json)
        print(f"ledger: {len(proofs)} proofs, " + ("every one holds" if not problems else f"{len(problems)} problem(s)"))
        for p in problems[:100]:
            print("  " + p)
        return 1 if problems else 0
    if args.all == bool(args.paths):
        ap.error("pass source paths or --all")
    paths = sources(args.paths, args.all, ROOT)
    retail = retail_images(ROOT)
    records = [verify_source(p, retail, bounds) for p in paths]
    summary = {s: sum(r["status"] == s for r in records) for s in ("MATCH", "DIFFER", "REFUSED", "ERROR")}
    for r in records:
        if r["status"] != "MATCH":
            print(f"{r['status']:8} {r['source']} {r.get('reason') or r.get('first_difference') or ''}")
    emit({"schema": "lbpxverify.report.v1", "sources": len(records), "summary": summary, "records": records}, args.json)
    print("SUMMARY " + " ".join(f"{k}={v}" for k, v in summary.items()))
    return 0 if summary["MATCH"] == len(records) and records else 1


if __name__ == "__main__":
    raise SystemExit(main())
