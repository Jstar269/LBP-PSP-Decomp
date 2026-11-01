#!/usr/bin/env python3
"""Private LBP decomp matcher: compile, link at the retail address, compare bytes exactly.
A source that compares equal but fails the fake-match gate (tools/lbpgate.py: asm that emits instructions, data
directives, code-emitting pragmas, `extra=` in src/) is reported as EXACT_FAILS_GATE, never as EXACT.

Each source file starts with a header comment:
    /* LBPMATCH module=EBOOT.BIN start=0x001a4bc8 end=0x001a4c1c symbol=FUN_001a4bc8 lang=c99 */
Optional keys: rodata=0xADDR (place .rodata), extra="..." (experimental extra compiler flags;
a source that needs extra flags is reported as such, never as a profile match),
helpers=NAME@0xSTART-0xEND[,NAME@0xSTART-0xEND...] (same-file helpers at their OWN retail addresses, below).

Same-file helpers (`helpers=`). Retail compiled some functions in one translation unit with a `static` helper that sits
at a different, non-adjacent retail address; MWCC then schedules the caller with knowledge of the helper's body, so the
caller matches only when the helper is defined in the same source. mwccpsp emits every function in its own `.text`
section (all of them called `.text`), so such a source is linked with EACH function at its own retail address: the
matcher renames the function sections of a copy of the object (placed.o; section names only: no byte, relocation or
symbol changes, the same as `objcopy --rename-section` addressed by index) to .lbp_fnN and gives each its address with
`--section-start`; the linker writes every relocation field, including the target's calls into the helper. Every
function the object defines must be the header symbol or a listed helper (UNPLACED_FUNCTION otherwise), each must be
alone in its section, and the file is EXACT only when EVERY function equals the retail bytes at its own address; a
helper that differs makes the whole file a MISMATCH. The header's start/end/symbol stay the target's: that range is
the one the source claims (the ledger counts a helper's bytes through its own one-function source).

Build: mwccpsp (decomp.me build, run with wibo in WSL) -> object; PSPDEV psp-ld links the object
with .text at `start` and defines only the object's undefined symbols from symbols/<module>.txt;
psp-objcopy extracts the bytes. The linked bytes must equal the retail bytes [start, end) exactly.
No masking: every relocated field is produced by the linker from the symbol map.

Usage: lbpmatch.py [paths...]   (default: every source under src/)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shlex
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpconfig as CFG  # noqa: E402
import lbpgate  # noqa: E402

# Machine-specific locations (campaign root, PSPDEV binutils, compiler kits) come from lbp-config.json
# (see tools/lbpconfig.py). They are looked up when first used, so `--help` and the local tools work
# without a configuration; `M.CAMPAIGN`, `M.PSPDEV_BIN`, `M.NATIVE`, `M.GCC_HOME` and `M.GCC_PROFILES` stay
# available to the other tools as lazy attributes.
PROFILE = {
    "build": "219",
    # Strength reduction is a per-object setting in the retail build (index-recompute loops cluster
    # in address runs, 3:1 minority): sources from those objects carry
    # `#pragma opt_strength_reduction off` themselves.
    "flags": '-c -O4,p -sdatathreshold 0',
}
HEADER = re.compile(r"/\*\s*LBPMATCH\s+(.*?)\*/", re.S)
# SDK objects are GCC output: the decomp.me Sony psp-gcc kits (i386 Linux, native WSL copies under the
# configured gcc_home). Header `cc=<profile>` selects one; flags are provisional until the SDK profile is settled.
# -fno-merge-constants: GCC's mergeable literal sections (.rodata.cst4/8, .rodata.str1.4) become plain
# .rodata, which `rodata=` places; it changes section layout only, never the code (all GCC matches were
# re-verified with it).
# gcc131-O0 / gcc171-O0: unoptimized Sony SDK objects (`move s8,sp` frame pointer, every local homed in the frame,
# `j` between blocks); no -mext variant, because no known -O0 object uses ext/ins.
GCC_PROFILE_NAMES = (frozenset(f"gcc{v}{x}-{o}" for v in ("131", "171") for o in ("O1", "O2", "O3", "Os")
                               for x in ("", "x"))  # gcc171x-*: Allegrex ext/ins enabled
                     | frozenset(f"gcc{v}-O0" for v in ("131", "171"))
                     # open-source GCC 4.x cross compilers built from GNU releases + pspdev Allegrex patches (experiments
                     # for LBP's toolchain question; docs/TOOLCHAIN.md): gcc410-* = 4.1.0 (PSPDEV 20060507), gcc432-* = 4.3.2
                     | frozenset(f"gcc{v}-{o}" for v in ("402", "410", "432") for o in ("O0", "O1", "O2", "O3", "Os"))
                     # gcc402h-* = 4.0.2 + the pspdev patch revision with hlide's -mpreferred-stack-boundary (2007-01);
                     # gcc402t-* = the same with -mtune=r4000 (BRANCH_COST 2 / MEMORY_MOVE_COST 6; best measured
                     # candidate of 2026-10-27, docs/TOOLCHAIN.md "Compiler identification")
                     # gcc402x-* = secondary (primary 2026-10-27 until gcc404x): 4.0.2h with a reconstructed Allegrex scheduling model (stores keep a
                     # memory port busy 3 cycles, loads latency 3) and the r4000 branch/move costs for the Allegrex;
                     # a patched open-source GCC (docs/TOOLCHAIN.md)
                     | frozenset(f"gcc{v}-{o}" for v in ("402h", "402t", "402x", "404x") for o in ("O0", "O1", "O2", "O3", "Os"))
                     # gcc404xi-Os = gcc404x-Os + --param max-inline-insns-single=50 (owner decision 2026-10-27): Sony's
                     # -Os keeps larger inline limits than GNU 4.0's -Os (which lowers max-inline-insns-single to 5);
                     # docs/TOOLCHAIN.md "Inline limit"
                     | frozenset({"gcc404xi-Os"})
                     # gcc404xs-* = gcc404x-* + -fno-exceptions, and at -Os GCC 4.0.4's default inline limits restored
                     # (max-inline-insns-single=450, -auto=90: Sony's -Os does not lower them as GNU's opts.c does);
                     # PRIMARY from 2026-10-27 (docs/TOOLCHAIN.md "Inline limit and exceptions")
                     | frozenset(f"gcc404xs-{o}" for o in ("O0", "O1", "O2", "O3", "Os")))
_gcc_profiles: dict | None = None
# PRIMARY profile (owner decision 2026-10-27, superseding gcc402x the same day): gcc404x, GCC 4.0.4 + the pspdev 2007
# patch revision + this project's three measured Allegrex changes (docs/TOOLCHAIN.md "Compiler identification" and
# "Primary profile"; provenance lbp-gcc4/PROVENANCE.json of the build kit, entry ~/lbp-gcc4/psp-gcc-4.0.4x).
# A header without cc= uses it; gcc402x-*, gcc402t-*, gcc410-* and gcc171-* stay secondary profiles for the
# functions only they match (every source in src/ names its own cc=).
# 2026-10-27: primary is gcc404xs-Os (gcc404x-Os with GCC's default inline limits and -fno-exceptions): per function
# identical to gcc404x-Os and gcc404xi-Os on the fixed sample (291 EXACT / 2,942 words left), EXACT on all 799
# gcc404xi-Os sources, and the only one of them that matches the std::map erase-by-key, _M_erase (recursion inlined)
# and ref-counted-pointer families (docs/TOOLCHAIN.md "Inline limit and exceptions").
# 2026-10-28 (owner decision, adopted on measurement): the 4.0.4x build gained a fourth measured Allegrex change: an
# `and` with a mask that clears one contiguous run of bits and that andi cannot encode is emitted as
# `ins rd,$zero,pos,len`, as Sony's compiler does (exp/build_ins.sh; the three-change build is kept as
# ~/lbp-gcc4/psp-gcc-4.0.4x3). Fixed sample 295 EXACT / 2,827 words left (was 291 / 2,942); all 1,172 sources built by
# 4.0.4x stay EXACT; zlib inflateSync EXACT from the stock source (docs/TOOLCHAIN.md "ins from $zero").
PRIMARY_PROFILE = "gcc404xs-Os"
# SN Systems SNC for PSP (decomp.me kit pspsnc_1.2.7503.0, a Windows binary run natively): header cc=snc127-O<n>.
# Part of LittleBigPlanet's linked libraries is SNC output (`or` moves, `ori` constants, s-registers stored from the
# frame bottom with ra after them); docs/TOOLCHAIN.md has the evidence.
SNC_PROFILES = {f"snc127-{o}": f"-{o}" for o in ("O0", "O1", "O2", "O3")}
SNC_PROFILE_NAMES = frozenset(SNC_PROFILES)


def snc_exe() -> Path:
    return CFG.host_path("snc_home") / "pspsnc.exe"


def campaign_root() -> Path:
    return CFG.host_path("campaign_root")


def pspdev_bin() -> str:
    return CFG.wsl_path("pspdev_bin")


def native_dir() -> str:
    """WSL directory with the native-filesystem copy of wibo + builds/<build>/mwccpsp.exe."""
    return CFG.wsl_path("mwcc_native")


def gcc4_home() -> str:
    return CFG.wsl_path("gcc4_home")


def gcc_home() -> str:
    return CFG.wsl_path("gcc_home")


def gcc_profiles() -> dict:
    global _gcc_profiles
    if _gcc_profiles is None:
        home = gcc_home()
        driver = f"{home}/psp-gcc-1.3.1/bin/psp-gcc"
        b = {"131": None, "171": f"{home}/psp-gcc-1.7.1/lib/gcc-lib/psp/3.3.3+allegrex-2.2.2-psp-1.7.1/"}
        _gcc_profiles = {f"gcc{v}{x}-{o}": {"driver": driver, "B": b[v], "flags": f"-{o} -G0 -fno-merge-constants{xf}"}
                         for v in b for o in ("O1", "O2", "O3", "Os") for x, xf in (("", ""), ("x", " -mext"))}
        _gcc_profiles.update({f"gcc{v}-O0": {"driver": driver, "B": b[v], "flags": "-O0 -G0 -fno-merge-constants"}
                              for v in b})
        # 4.1.0 is the build that assembles with the Sony kit's binutils 2.14+allegrex (psp-gcc-4.1.0s), with the
        # PSP's 16-byte stack alignment; 4.3.2 assembles with the pspdev psp-as (diagnostic only)
        g4 = {"402": ("4.0.2s", ""), "410": ("4.1.0s", " -mpreferred-stack-boundary=4"), "432": ("4.3.2", ""),
              "402h": ("4.0.2h", " -mpreferred-stack-boundary=4"),
              "402t": ("4.0.2h", " -mpreferred-stack-boundary=4 -mtune=r4000"),
              "402x": ("4.0.2x", " -mpreferred-stack-boundary=4"),
              # gcc404x-*: PRIMARY: the gcc402x changes on GCC 4.0.4 (C++ static-init test order as retail; 2026-10-27)
              "404x": ("4.0.4x", " -mpreferred-stack-boundary=4")}
        # -mulsw: the Sony assembler rejects lwl/lwr/swl/swr on -march=allegrex unless given its own -mulsw
        # (unaligned load/store word) option; retail LBP has them (docs/TOOLCHAIN.md, "Unaligned word access")
        ulsw = {v: "" if v == "432" else " -Wa,-mulsw" for v in g4}
        _gcc_profiles.update({f"gcc{v}-{o}": {"driver": f"{gcc4_home()}/psp-gcc-{g4[v][0]}/bin/psp-gcc", "B": None,
                                               "flags": f"-{o} -G0 -fno-merge-constants{g4[v][1]}{ulsw[v]}"}
                              for v in g4 for o in ("O0", "O1", "O2", "O3", "Os")})
        _gcc_profiles["gcc404xi-Os"] = dict(_gcc_profiles["gcc404x-Os"],
                                            flags=_gcc_profiles["gcc404x-Os"]["flags"] + " --param max-inline-insns-single=50")
        for o in ("O0", "O1", "O2", "O3", "Os"):
            inl = " --param max-inline-insns-single=450 --param max-inline-insns-auto=90" if o == "Os" else ""
            _gcc_profiles[f"gcc404xs-{o}"] = dict(_gcc_profiles[f"gcc404x-{o}"],
                                                  flags=_gcc_profiles[f"gcc404x-{o}"]["flags"] + inl + " -fno-exceptions")
        assert set(_gcc_profiles) == GCC_PROFILE_NAMES
    return _gcc_profiles


def __getattr__(name: str):  # PEP 562: lazy module attributes (the old constants)
    getters = {"CAMPAIGN": campaign_root, "PSPDEV_BIN": pspdev_bin, "NATIVE": native_dir, "GCC_HOME": gcc_home,
               "GCC_PROFILES": gcc_profiles}
    if name in getters:
        return getters[name]()
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


def to_wsl(path: Path) -> str:
    p = str(path.resolve()).replace("\\", "/")
    return f"/mnt/{p[0].lower()}{p[2:]}"


# The i386 psp-gcc kits cannot stat files on /mnt/c (64-bit inode numbers: EOVERFLOW), so each GCC source compiles as
# a copy in a native temporary directory, with ./include (`#include "lbp/<name>.h"`) a link to ONE native copy of
# include/ per batch ($LBPINC). Copying include/ for every job overloaded the /mnt/c file share under --parallel
# (`cp: ... Cannot allocate memory`), and the job then died before writing compile.log (2026-11-01). A job always
# writes its compile.log (naming a failed staging step), writes obj.o through a temporary name, and exits 0.
STAGE_FAILED = 97   # exit status of a batch script that could not copy include/ into WSL


def gcc_stage_lines(include: Path) -> list[str]:
    """Script lines that copy include/ once into a native directory exported as $LBPINC (exit STAGE_FAILED if not)."""
    return ["LBPINC=$(mktemp -d /tmp/lbpinc.XXXXXX) || exit %d" % STAGE_FAILED, "export LBPINC",
            f"cp -r {shlex.quote(to_wsl(include))} \"$LBPINC/include\" || "
            f"{{ echo 'cannot copy include/ into WSL' >&2; rm -rf \"${{LBPINC:?}}\"; exit {STAGE_FAILED}; }}"]


def gcc_batch(xargs_line: str, include: Path) -> list[str]:
    """The whole batch script: stage include/, run the jobs, remove the staged copy, keep xargs' exit status."""
    return gcc_stage_lines(include) + [xargs_line, "s=$?", 'rm -rf "${LBPINC:?}"', "exit $s"]


def gcc_job(src: Path, log: Path, obj: Path, compile_cmd: str, tag: str) -> str:
    """One xargs job: run `compile_cmd` (which reads ./<name>, `-I include`, writes ./obj.o) on a copy of `src` in a
    native temporary directory; copy the log to `log` and the object to `obj`. Always exits 0 (xargs stops at 255)."""
    q = shlex.quote
    lg, ob = q(to_wsl(log)), q(to_wsl(obj))
    part = q(to_wsl(obj) + ".part")
    return (f"t=$(mktemp -d /tmp/{tag}.XXXXXX) || {{ echo 'compile job: mktemp failed' > {lg}; exit 0; }}; "
            f"if cp {q(to_wsl(src))} \"$t/\" && ln -s \"$LBPINC/include\" \"$t/include\"; then "
            f"(cd \"$t\" && {compile_cmd}) > \"$t/log\" 2>&1; "
            f"cp \"$t/log\" {lg} || echo 'compile job: copying the compiler log back failed' > {lg}; "
            f"if [ -f \"$t/obj.o\" ]; then cp \"$t/obj.o\" {part} && mv {part} {ob}; fi; "
            f"else echo 'compile job: staging the source in WSL failed (see the run output)' > {lg}; fi; "
            f"rm -rf \"$t\"; exit 0")


def build_key(path: Path) -> str:
    """The directory name of one source's build products under build/<module>/: `<folder>__<stem>__<digest>`, the
    digest being the first 10 hex digits of the SHA-1 of the source's path (relative to the checkout when inside it,
    so a lane and the main tree use the same key for the same file; absolute otherwise; case-folded on Windows).
    `<folder>__<stem>` alone gave try/gen-a1/v0/FUN_x.c and try/gen-a2/v0/FUN_x.c one directory, so each could read
    the other's text.bin (as hst-decomp's hstmatch.build_key, 2026-10-29)."""
    p = Path(path).resolve()
    try:
        ident = p.relative_to(ROOT).as_posix()
    except ValueError:
        ident = p.as_posix()
    digest = hashlib.sha1(os.path.normcase(ident).encode("utf-8")).hexdigest()[:10]
    return f"{p.parent.name}__{p.stem}__{digest}"


def parse_header(path: Path) -> dict:
    m = HEADER.search(path.read_text(encoding="utf-8", errors="replace")[:2000])
    if not m:
        raise ValueError(f"{path}: missing LBPMATCH header")
    fields = dict(re.findall(r'(\w+)=("[^"]*"|\S+)', m.group(1)))
    fields = {k: v.strip('"') for k, v in fields.items()}
    for key in ("module", "start", "end", "symbol"):
        if key not in fields:
            raise ValueError(f"{path}: header lacks {key}")
    fields["start"] = int(fields["start"], 16)
    fields["end"] = int(fields["end"], 16)
    fields.setdefault("lang", "c++" if path.suffix == ".cpp" else "c99")
    if "helpers" in fields:
        fields["helpers"] = parse_helpers(fields["helpers"], fields, path)
    return fields


HELPER = re.compile(r"([A-Za-z_]\w*)@(0x[0-9A-Fa-f]{1,8})-(0x[0-9A-Fa-f]{1,8})")


def parse_helpers(value: str, h: dict, path: Path | str = "") -> list[tuple[str, int, int]]:
    """`helpers=NAME@0xSTART-0xEND,...` -> [(name, start, end)]: same-file functions placed at their own retail
    addresses. Ranges are word-aligned, non-empty, and overlap neither the target's range nor each other."""
    out: list[tuple[str, int, int]] = []
    for item in value.split(","):
        m = HELPER.fullmatch(item.strip())
        if not m:
            raise ValueError(f"{path}: bad helpers= entry {item!r} (want NAME@0xSTART-0xEND)")
        name, s, e = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
        if not s < e or s % 4 or e % 4:
            raise ValueError(f"{path}: helper {name} has a bad range [0x{s:08x}, 0x{e:08x})")
        out.append((name, s, e))
    spans = [(h["symbol"], h["start"], h["end"])] + out
    names = [n for n, _s, _e in spans]
    if len(set(names)) != len(names):
        raise ValueError(f"{path}: helpers= repeats a function name")
    ordered = sorted(spans, key=lambda t: t[1])
    for (n1, _s1, e1), (n2, s2, _e2) in zip(ordered, ordered[1:]):
        if s2 < e1:
            raise ValueError(f"{path}: helper ranges overlap ({n1} and {n2})")
    return out


def placed_functions(h: dict) -> list[tuple[str, int, int]]:
    """(symbol, start, end) of every function a source places: the target first, then its helpers."""
    return [(h["symbol"], h["start"], h["end"])] + list(h.get("helpers") or [])


EXTRA_SYMBOLS: list[Path] = []  # --symbols-extra: candidate entries tried for one run only


def load_symbols(module: str) -> dict[str, int]:
    out: dict[str, int] = {}
    for path in [ROOT / "symbols" / f"{module}.txt", *EXTRA_SYMBOLS]:
        if not path.exists():
            continue
        for line in path.read_text(encoding="utf-8").splitlines():
            line = line.split("#", 1)[0].strip()
            if not line:
                continue
            m = re.fullmatch(r"(\S+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;?", line)
            if not m:
                raise ValueError(f"bad symbol line in {path}: {line}")
            if m.group(1) in out and out[m.group(1)] != int(m.group(2), 16):
                raise ValueError(f"conflicting symbol {m.group(1)} in {path}")
            out[m.group(1)] = int(m.group(2), 16)
    return out


def default_address(name: str) -> int | None:
    """FUN_/DAT_/LAB_ names carry their address; everything else must be in the symbol map."""
    m = re.fullmatch(r"(?:FUN|DAT|LAB|PTR|jtbl)_([0-9a-fA-F]{8})", name)
    return int(m.group(1), 16) if m else None


def elf_sections(obj: bytes):
    shoff, = struct.unpack_from("<I", obj, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", obj, 0x2E)
    secs = [struct.unpack_from("<IIIIIIIIII", obj, shoff + i * shentsize) for i in range(shnum)]
    names = secs[shstrndx]
    named = []
    for s in secs:
        n = obj[names[4] + s[0]:obj.index(b"\0", names[4] + s[0])].decode()
        named.append((n, s))
    return named


def elf_symbols(obj: bytes):
    secs = [s for _, s in elf_sections(obj)]
    symtab = next(s for s in secs if s[1] == 2)
    strtab = secs[symtab[6]]
    for i in range(symtab[5] // 16):
        st_name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", obj, symtab[4] + i * 16)
        name = obj[strtab[4] + st_name:obj.index(b"\0", strtab[4] + st_name)].decode()
        yield name, value, size, info, shndx


STT_FUNC = 2


def function_sections(obj: bytes, funcs: list[tuple[str, int, int]]) -> tuple[dict[int, tuple[str, int, int]], str]:
    """For a `helpers=` source: {.text section index: (symbol, start, end)} for every placed function, or ({}, why)
    when the object cannot be placed function by function: a function missing, defined twice, not alone at the start
    of its own section, or a non-empty .text section (a function) that the header does not place."""
    sections = elf_sections(obj)
    text = {i for i, (n, s) in enumerate(sections) if n == ".text" and s[1] == 1}
    nonempty = {i for i in text if sections[i][1][5]}
    symbols = list(elf_symbols(obj))
    out: dict[int, tuple[str, int, int]] = {}
    for f in funcs:
        hits = [(v, sh) for n, v, sz, info, sh in symbols if n == f[0] and sh in text]
        if len(hits) != 1:
            return {}, f"{f[0]} is {'not defined' if not hits else 'defined more than once'} in a .text section"
        value, sh = hits[0]
        others = sorted({n for n, v, sz, info, x in symbols if x == sh and n and n != f[0] and info & 15 == STT_FUNC})
        if value != 0 or others or sh in out:
            return {}, (f"{f[0]} shares its .text section with {', '.join(others) or 'other code'}: helpers= needs one "
                        "section per function (mwccpsp)")
        out[sh] = f
    rest = sorted(nonempty - set(out))
    if rest:
        names = sorted({n for n, v, sz, info, sh in symbols if sh in rest and n and info & 15 == STT_FUNC})
        return {}, (f"{', '.join(names) or f'{len(rest)} .text section(s)'} defined but not placed: list every function "
                    "of the file in helpers= (NAME@0xSTART-0xEND)")
    return out, ""


def rename_sections(obj: bytes, names: dict[int, str]) -> bytes:
    """A copy of the relocatable object with the sections at the given indices renamed. Only the section-name string
    table (appended anew at the end of the file) and those sections' sh_name fields change: section contents,
    relocations and symbols are untouched (objcopy --rename-section cannot address one of several `.text` sections)."""
    shoff, = struct.unpack_from("<I", obj, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", obj, 0x2E)
    hdr = lambda i: shoff + i * shentsize  # noqa: E731
    st_off, st_size = struct.unpack_from("<II", obj, hdr(shstrndx) + 16)
    table = bytearray(obj[st_off:st_off + st_size])
    out = bytearray(obj)
    for i, name in sorted(names.items()):
        if not 0 < i < shnum or i == shstrndx:
            raise ValueError(f"cannot rename section {i}")
        struct.pack_into("<I", out, hdr(i), len(table))
        table += name.encode("ascii") + b"\0"
    out += b"\0" * (-len(out) % 4)
    struct.pack_into("<II", out, hdr(shstrndx) + 16, len(out), len(table))
    out += table
    return bytes(out)


def linked_section(elf: bytes, name: str):
    """(address, contents) of the linked section `name`, or None."""
    for n, s in elf_sections(elf):
        if n == name:
            return s[3], (elf[s[4]:s[4] + s[5]] if s[1] != 8 else b"")
    return None


def word_diffs(body: bytes, target: bytes) -> list[int]:
    return [i for i in range(max(len(body), len(target)) // 4) if body[i * 4:i * 4 + 4] != target[i * 4:i * 4 + 4]]


def module_bytes(cache: dict, module: str):
    if module not in cache:
        if "_ledger" not in cache:
            campaign = campaign_root()
            cache["_ledger"] = json.loads((campaign / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
            cache["_selection"] = json.loads((campaign / "inputs" / "revision-selection.json").read_text(encoding="utf-8-sig"))
        mod = next(m for m in cache["_ledger"]["modules"] if m["name"] == module)
        data = (Path(cache["_selection"]["roots"][mod["input"]["root"]]) / mod["input"]["path"]).read_bytes()
        seg = next(s for s in mod["load_segments"] if s["flags"] & 1)
        cache[module] = (data, seg)
    return cache[module]


PLACED_WITH_CONTENTS = (".rodata", ".data", ".sdata")
# GCC 4.0 (with the kit's binutils, no COMDAT groups) emits each object with vague linkage -- the local static of an
# inline function, a static data member of a class template -- in its own section .gnu.linkonce.<kind>.<symbol>
# (b = bss, sb = small bss, d = data, s = small data, r = rodata). Such a section holds that one object, so it is
# placed at the object's address in symbols/<module>.txt (owner decision 2026-10-27; the object is weak, which is
# why retail does not hoist its loads, e.g. the std::map allocator's heap pointer in _Rb_tree::_M_insert).
LINKONCE_DATA = re.compile(r"\.gnu\.linkonce\.(?:b|sb|d|s|r)\.(?P<symbol>.+)")


def linkonce_placements(sections: dict, ref_sections: set | None, syms: dict) -> tuple[list[tuple[str, int, bool]], str]:
    """([(section, address, has contents)], '') for the non-empty linkonce data sections the compared function refers
    to (all of them when that is unknown), each at its object's address from the symbol map; ([], why) when one
    has no address there."""
    out = []
    for name, s in sorted(sections.items()):
        m = LINKONCE_DATA.fullmatch(name)
        if not m or not s[5] or (ref_sections is not None and name not in ref_sections):
            continue
        addr = syms.get(m["symbol"])
        if addr is None:
            return [], f"{name} ({s[5]} bytes) needs '{m['symbol']} = 0x...;' in the module's symbol file"
        out.append((name, addr, s[1] != 8))   # SHT_NOBITS has no contents to compare
    return out, ""


def target_references(obj: bytes, h: dict):
    """(undefined names, section names) that relocations INSIDE the compared function refer to, or
    (None, None) when that cannot be determined (then every name and section is required, as before).
    The compared function is the header symbol, over max(its size, the header span)."""
    sections = elf_sections(obj)
    names = [n for n, _ in sections]
    if ".text" not in names:
        return None, None
    symbols = list(elf_symbols(obj))
    if h.get("helpers"):
        # every function of a helpers= source is placed and compared: every .text relocation counts
        span_of = {i: (0, 1 << 32) for i, (n, s) in enumerate(sections) if n == ".text" and s[1] == 1}
    else:
        ti = names.index(".text")
        sym = next(((v, s) for n, v, s, i, sh in symbols if n == h["symbol"] and sh == ti), None)
        if sym is None:
            return None, None
        span_of = {ti: (sym[0], sym[0] + max(sym[1], h["end"] - h["start"]))}
    ref_names, ref_sections = set(), set()
    for n, s in sections:
        if s[1] != 9 or s[7] not in span_of:  # SHT_REL applying to a compared .text
            continue
        lo, hi = span_of[s[7]]
        for k in range(s[5] // 8):
            off, info = struct.unpack_from("<II", obj, s[4] + k * 8)
            if not lo <= off < hi:
                continue
            name, value, size, sinfo, shndx = symbols[info >> 8]
            if shndx == 0:
                ref_names.add(name)
            elif 0 < shndx < len(names):
                ref_sections.add(names[shndx])
    if any(s[1] == 4 and s[7] in span_of for _, s in sections):  # SHT_RELA: not produced by these compilers
        return None, None
    return ref_names, ref_sections


def placed_section_differs(h: dict, out: Path, data: bytes, seg: dict, expected: list[str]) -> str:
    """'' when every placed section with contents equals the retail bytes at its header address."""
    for sec in expected:
        key = sec.strip(".")
        blob = out / f"{key}.bin"
        if not blob.exists():
            return f"{sec} was not extracted"
        ours = blob.read_bytes()
        addr = int(h[key], 16)
        off = seg["file_offset"] + addr - seg["start"]
        if data[off:off + len(ours)] != ours:
            return f"{sec} ({len(ours)} bytes at 0x{addr:08x}) differs from retail"
    return ""


def linkonce_section_differs(out: Path, data: bytes, seg: dict, expected: list[tuple[str, int]]) -> str:
    """'' when every placed linkonce data section with contents equals the retail bytes at its address."""
    for k, (sec, addr) in enumerate(expected):
        blob = out / f"linkonce{k}.bin"
        if not blob.exists():
            return f"{sec} was not extracted"
        ours = blob.read_bytes()
        off = seg["file_offset"] + addr - seg["start"]
        if data[off:off + len(ours)] != ours:
            return f"{sec} ({len(ours)} bytes at 0x{addr:08x}) differs from retail"
    return ""


def compare_placed(h: dict, out: Path, data: bytes, seg: dict, placement: list, expected: list[str]) -> tuple[str, str, int | None]:
    """(status, detail, differing words) of a helpers= source: every function, read from its own linked section, must
    sit at its retail address and equal the retail bytes there. A section shorter than its range is followed by the
    linker's zero fill (as between any two input sections), so the rest of the range must be zero in retail too. Writes
    text.bin/target.bin for the target and helper-<name>.bin/.target.bin for each helper (tools/lbpdiff.py)."""
    elf_path = out / "linked.elf"
    if not elf_path.exists():
        return "LINK_FAILED", (out / "link.log").read_text(errors="replace")[-600:], None
    elf = elf_path.read_bytes()
    where: dict[str, list[int]] = {}
    for n, v, s, i, sh in elf_symbols(elf):
        if sh != 0 and n:
            where.setdefault(n, []).append(v)
    total, parts = 0, []
    for sec, sym, start, end in placement:
        got = linked_section(elf, sec)
        if got is None or got[0] != start or where.get(sym) != [start]:
            at = f"0x{where[sym][0]:x}" if where.get(sym) else "nowhere"
            return "MISPLACED", f"{sym} linked at {at}, not at its retail address 0x{start:08x}", None
        span = end - start
        body = got[1] + bytes(max(0, span - len(got[1])))
        off = seg["file_offset"] + start - seg["start"]
        target = data[off:off + span]
        stem = "text" if sym == h["symbol"] else f"helper-{sym}"
        (out / f"{stem}.bin").write_bytes(body)
        ((out / "target.bin") if sym == h["symbol"] else (out / f"{stem}.target.bin")).write_bytes(target)
        diffs = word_diffs(body, target)
        total += len(diffs)
        if sym == h["symbol"]:
            parts.insert(0, f"{len(body)//4}/{span//4} words; {len(diffs)} differ"
                         + (f"; first at +0x{diffs[0]*4:x}" if diffs else ""))
        elif diffs:
            parts.append(f"helper {sym}@0x{start:08x}: {len(body)//4}/{span//4} words; {len(diffs)} differ; "
                         f"first at +0x{diffs[0]*4:x}")
    if total:
        return "MISMATCH", "; ".join(parts), total
    section_diff = placed_section_differs(h, out, data, seg, expected)
    if section_diff:
        return "SECTION_MISMATCH", section_diff, 0
    return "EXACT", "", 0


def run_wsl(lines: list[str], name: str, native: str | None = None) -> int:
    # Per-process script names, so independent runs (on different sources) can overlap. `native` is exported
    # as $N for the MWCC command lines. Returns the script's exit status.
    script = ROOT / "build" / f"{name}.{os.getpid()}.sh"
    prologue = ["#!/bin/sh"] + (["export N=" + native] if native else [])
    script.write_text("\n".join(prologue + lines) + "\n", encoding="utf-8", newline="\n")
    try:
        return subprocess.run(["wsl.exe", "-e", "sh", to_wsl(script)], check=False).returncode
    finally:
        script.unlink(missing_ok=True)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="*", type=Path)
    ap.add_argument("--parallel", type=int, default=6)
    ap.add_argument("--quiet", action="store_true")
    ap.add_argument("--json", type=Path)
    ap.add_argument("--all", action="store_true", help="rebuild every source under src/")
    ap.add_argument("--flags", help="experimental: override the profile flags (results are not profile matches)")
    ap.add_argument("--symbols-extra", type=Path, action="append", default=[],
                    help="extra 'name = 0xaddr;' entries for this run (results are not committed symbols)")
    args = ap.parse_args()
    EXTRA_SYMBOLS.extend(args.symbols_extra)
    flags = args.flags if args.flags is not None else PROFILE["flags"]
    if not args.paths and not args.all:
        raise SystemExit("no sources given: pass files, or --all to rebuild every source under src/ "
                         "(a full rebuild shares build/ with any concurrent run)")
    paths = args.paths or sorted(p for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))
    jobs = []
    # LBPMATCH_BUILD_ROOT: a different directory for the build products (tools/lbppromote.py gives every verification
    # run a fresh one, so no artifact of an earlier run can be read); the comparison is the same
    build_root = Path(os.environ["LBPMATCH_BUILD_ROOT"]) if os.environ.get("LBPMATCH_BUILD_ROOT") else ROOT / "build"
    for p in paths:
        h = parse_header(p)
        out = build_root / h["module"] / build_key(p)
        out.mkdir(parents=True, exist_ok=True)
        for f in out.glob("*"):
            f.unlink()
        jobs.append((p, h, out))
    # 1. compile (parallel)
    cmds = []
    native = None
    for p, h, out in jobs:
        extra = h.get("extra", "")
        log = shlex.quote(to_wsl(out / 'compile.log'))
        obj = shlex.quote(to_wsl(out / 'obj.o'))
        cc = h.get("cc", PRIMARY_PROFILE)
        if cc in GCC_PROFILE_NAMES:
            g = gcc_profiles()[cc]
            gflags = args.flags if args.flags is not None else g["flags"]
            lang = "-x c++" if h["lang"] == "c++" else "-x c"
            bprefix = f"-B {g['B']} " if g["B"] else ""
            # a copy in a native temporary directory, ./include linked to the batch's native include/ (gcc_job)
            cmds.append(gcc_job(p, out / "compile.log", out / "obj.o",
                                f"{g['driver']} {bprefix}-c {gflags} {lang} -I include {extra} -o obj.o "
                                f"{shlex.quote(p.name)}", "lbpgcc"))
            continue
        if cc in SNC_PROFILE_NAMES:
            # SN Systems SNC (pspsnc.exe, a Windows program): compiled natively here, before the WSL batch
            snc = snc_exe()
            sflags = (args.flags if args.flags is not None else SNC_PROFILES[cc]).split()
            r = subprocess.run([str(snc), "-c", *sflags, *shlex.split(extra), f"-I{ROOT / 'include'}",
                                "-o", str(out / "obj.o"), p.name], cwd=str(p.parent), capture_output=True, text=True)
            (out / "compile.log").write_text(r.stdout + r.stderr, encoding="utf-8")
            continue
        if cc != "mwcc219":
            raise SystemExit(f"{p}: unknown compiler profile cc={cc}")
        native = native_dir()
        cmds.append(f"cd {shlex.quote(to_wsl(p.parent))} && MWCIncludes={shlex.quote(to_wsl(ROOT / 'include'))} "
                    f"$N/wibo $N/builds/{PROFILE['build']}/mwccpsp.exe {flags} -lang {h['lang']} {extra} "
                    f"-o {obj} {shlex.quote(p.name)} > {log} 2>&1")
    jobfile = ROOT / "build" / f"compile.{os.getpid()}.jobs"
    jobfile.write_text("\n".join(cmds) + "\n", encoding="utf-8", newline="\n")
    try:
        xargs = f"tr '\\n' '\\0' < {shlex.quote(to_wsl(jobfile))} | xargs -0 -P {args.parallel} -n 1 sh -c"
        if run_wsl(gcc_batch(xargs, ROOT / "include"), "compile", native) == STAGE_FAILED:
            raise SystemExit("could not copy include/ into WSL for the GCC compiles (message above); nothing compiled")
    finally:
        jobfile.unlink(missing_ok=True)
    # 2. link each object at its retail address with only the symbols it needs
    symcache: dict[str, dict[str, int]] = {}
    link = []
    results = {}
    expected_sections: dict = {}
    linkonce_expected: dict = {}   # [(linkonce data section with contents, retail address)] per source
    text_off: dict = {}
    fn_placement: dict = {}   # helpers= sources: [(linked section, symbol, start, end)], in address order
    for p, h, out in jobs:
        obj_path = out / "obj.o"
        if not obj_path.exists():
            log_path = out / "compile.log"
            results[p] = ("COMPILE_FAILED", log_path.read_text(errors="replace")[-600:] if log_path.exists() else
                          "the compile job wrote no compile.log (it did not run; see the run output)")
            continue
        obj = obj_path.read_bytes()
        syms = symcache.setdefault(h["module"], load_symbols(h["module"]))
        undefined = sorted({n for n, v, s, i, sh in elf_symbols(obj) if sh == 0 and n})
        # Names and sections reached only by OTHER functions of the object (a library file compiled whole)
        # cannot change the compared bytes: such names get a placeholder and such sections stay unplaced.
        ref_names, ref_sections = target_references(obj, h)
        defs, missing = [], []
        for n in undefined:
            addr = syms.get(n, default_address(n))
            if addr is None and ref_names is not None and n not in ref_names:
                defs.append(f"{n} = 0x00000000;   /* not referenced by the compared function */")
                continue
            (defs.append(f"{n} = 0x{addr:08x};") if addr is not None else missing.append(n))
        if missing:
            results[p] = ("MISSING_SYMBOLS", " ".join(missing))
            continue
        sections = {n: s for n, s in elf_sections(obj)}
        link_obj = obj_path
        if h.get("helpers"):
            # same-file helpers: every function at its own retail address (see the module docstring)
            cc = h.get("cc", PRIMARY_PROFILE)   # this source's profile (not the last one the compile loop saw)
            if cc != "mwcc219":
                results[p] = ("UNPLACED_FUNCTION", f"helpers= needs one .text section per function (mwccpsp); cc={cc}")
                continue
            fsecs, why = function_sections(obj, placed_functions(h))
            if why:
                results[p] = ("UNPLACED_FUNCTION", why)
                continue
            order = sorted(fsecs, key=lambda i: fsecs[i][1])
            fn_names = {i: f".lbp_fn{k}" for k, i in enumerate(order)}
            fn_placement[p] = [(fn_names[i], *fsecs[i]) for i in order]
            link_obj = out / "placed.o"
            link_obj.write_bytes(rename_sections(obj, fn_names))
            placed = [f"--section-start {fn_names[i]}=0x{fsecs[i][1]:08x}" for i in order]
        else:
            # the target need not be the object's first function (a library file compiled whole): .text is
            # placed so that the target lands at its retail address, and the rest of the object around it
            names = [n for n, _ in elf_sections(obj)]
            text_idx = names.index(".text") if ".text" in names else -1
            text_off[p] = next((v for n, v, s, i, sh in elf_symbols(obj) if n == h["symbol"] and sh == text_idx), 0)
            placed = [f"-Ttext=0x{h['start'] - text_off[p]:08x}"]
        for sec in (".rodata", ".data", ".sdata", ".sbss", ".bss"):
            if sec in sections and sections[sec][5]:
                key = sec.strip(".")
                if key not in h and ref_sections is not None and sec not in ref_sections:
                    continue  # unreferenced by the compared function: left where ld puts it, not compared
                if key not in h:
                    results[p] = ("UNPLACED_SECTION", f"{sec} ({sections[sec][5]} bytes) needs {key}=0x... in the header")
                    break
                placed.append(f"--section-start {sec}=0x{int(h[key], 16):08x}")
        if p in results:
            continue
        lplaced, why = linkonce_placements(sections, ref_sections, syms)
        if why:
            results[p] = ("UNPLACED_SECTION", why)
            continue
        if lplaced:
            # renamed first: the linker's default script would merge .gnu.linkonce.* into .bss/.data/.rodata
            index = {n: i for i, (n, _s) in enumerate(elf_sections(obj))}
            lo_names = {index[sec]: f".lbp_lo{k}" for k, (sec, _a, _c) in enumerate(lplaced)}
            renamed = rename_sections(link_obj.read_bytes(), lo_names)
            link_obj = out / "placed.o"
            link_obj.write_bytes(renamed)
            lplaced = [(f".lbp_lo{k}", addr, contents) for k, (_sec, addr, contents) in enumerate(lplaced)]
        placed += [f"--section-start {sec}=0x{addr:08x}" for sec, addr, _c in lplaced]
        linkonce_expected[p] = [(sec, addr) for sec, addr, contents in lplaced if contents]
        (out / "syms.ld").write_text("\n".join(defs) + "\n", encoding="utf-8", newline="\n")
        # placed sections with contents are extracted too and must equal the retail bytes at their address
        expected_sections[p] = [sec for sec in PLACED_WITH_CONTENTS if sec in sections and sections[sec][5]
                                and sec.strip(".") in h]
        pspdev = pspdev_bin()
        extract = "".join(f" && {pspdev}/psp-objcopy -O binary -j {sec} {to_wsl(out / 'linked.elf')} "
                          f"{to_wsl(out / (sec.strip('.') + '.bin'))}" for sec in expected_sections[p])
        extract += "".join(f" && {pspdev}/psp-objcopy -O binary -j {shlex.quote(sec)} {to_wsl(out / 'linked.elf')} "
                           f"{to_wsl(out / f'linkonce{k}.bin')}" for k, (sec, _a) in enumerate(linkonce_expected[p]))
        # helpers= sources: each function's bytes are read from its own linked section (step 3)
        text_cmd = "" if p in fn_placement else \
            f" && {pspdev}/psp-objcopy -O binary -j .text {to_wsl(out / 'linked.elf')} {to_wsl(out / 'text.bin')}"
        link.append(f"{pspdev}/psp-ld -EL -e 0x{h['start']:08x} {' '.join(placed)} -o {to_wsl(out / 'linked.elf')} "
                    f"{to_wsl(link_obj)} {to_wsl(out / 'syms.ld')} > {to_wsl(out / 'link.log')} 2>&1"
                    + text_cmd + extract)
    if link:
        run_wsl(link, "link")
    # 3. compare
    cache: dict = {}
    summary = []
    for p, h, out in jobs:
        rel = p.relative_to(ROOT).as_posix() if p.is_relative_to(ROOT) else str(p)
        words = None   # differing words (all compared functions), when the source linked and was compared
        if p in results:
            status, detail = results[p]
        elif p in fn_placement:
            data, seg = module_bytes(cache, h["module"])
            status, detail, words = compare_placed(h, out, data, seg, fn_placement[p], expected_sections.get(p, []))
            if status == "EXACT":
                if h.get("extra") or args.flags is not None:
                    status = "EXACT_EXTRA_FLAGS"
                gate = lbpgate.refusals(lbpgate.check_file(p))
                if status == "EXACT" and gate:
                    status, detail = "EXACT_FAILS_GATE", lbpgate.summary(gate)
        else:
            data, seg = module_bytes(cache, h["module"])
            off = seg["file_offset"] + h["start"] - seg["start"]
            target = data[off:off + h["end"] - h["start"]]
            text = out / "text.bin"
            if not text.exists():
                status, detail = "LINK_FAILED", (out / "link.log").read_text(errors="replace")[-600:]
            else:
                linked = text.read_bytes()
                elf = (out / "linked.elf").read_bytes()
                sym = [(v, s) for n, v, s, i, sh in elf_symbols(elf) if n == h["symbol"] and sh != 0]
                if not sym:
                    status, detail = "NO_SYMBOL", h["symbol"]
                elif sym[0][0] != h["start"]:
                    status, detail = "MISPLACED", f"{h['symbol']} linked at 0x{sym[0][0]:x}, not at its retail address"
                    others = sorted({n for n, v, s, i, sh in elf_symbols((out / "obj.o").read_bytes())
                                     if sh != 0 and i & 15 == STT_FUNC and n and n != h["symbol"]})
                    if others:
                        detail += (f" (the file also defines {', '.join(others[:4])}: same-file helpers go at their own "
                                   "addresses with helpers=NAME@0xSTART-0xEND)")
                else:
                    span = h["end"] - h["start"]
                    linked = linked[text_off.get(p, 0):]
                    # a multi-function run (same-file static helpers) compares the whole header span
                    body = linked[:span] if sym[0][1] < span else linked[:sym[0][1]]
                    section_diff = (placed_section_differs(h, out, data, seg, expected_sections.get(p, []))
                                    or linkonce_section_differs(out, data, seg, linkonce_expected.get(p, [])))
                    if body == target:
                        words = 0
                    if body == target and section_diff:
                        status, detail = "SECTION_MISMATCH", section_diff
                    elif body == target:
                        status, detail = ("EXACT_EXTRA_FLAGS" if h.get("extra") or args.flags is not None else "EXACT"), ""
                        # gate 2 of docs/COMPLETION.md: bytes pasted in as asm or data directives are not a match
                        gate = lbpgate.refusals(lbpgate.check_file(p))
                        if status == "EXACT" and gate:
                            status, detail = "EXACT_FAILS_GATE", lbpgate.summary(gate)
                    else:
                        n = len(target) // 4
                        diffs = word_diffs(body, target)
                        words = len(diffs) if diffs else abs(len(body) - len(target)) // 4 or 1
                        status = "MISMATCH"
                        detail = f"{len(body)//4}/{n} words; {len(diffs)} differ; first at +0x{diffs[0]*4:x}" if diffs else "size differs"
                        (out / "target.bin").write_bytes(target)
        summary.append({"source": rel, "module": h["module"], "start": f"0x{h['start']:08x}", "status": status, "detail": detail,
                        "words_differ": words})
        if not args.quiet or status != "EXACT":
            print(f"{status:18s} {h['module']} 0x{h['start']:08x} {rel} {detail}")
    exact = sum(1 for s in summary if s["status"] == "EXACT")
    print(f"EXACT {exact}/{len(summary)}" + (f" (EXACT_EXTRA_FLAGS {sum(1 for s in summary if s['status'] == 'EXACT_EXTRA_FLAGS')} under --flags {flags})" if args.flags is not None else ""))
    if args.json:
        args.json.write_text(json.dumps(summary, indent=1) + "\n", encoding="utf-8")
    return 0 if exact == len(summary) else 1


if __name__ == "__main__":
    raise SystemExit(main())
