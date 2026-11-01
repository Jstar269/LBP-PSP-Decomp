# Contributing

Contributions are welcome: matched functions, near misses with notes, compiler findings, tool fixes and documentation.
This guide covers what you need, how a function is matched and checked, and what a pull request must contain. The
method is hst-decomp's with the tool names changed; LBP-specific facts are in [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md)
(compiler profiles) and `data/modules.json` (modules). Contributions are licensed GPL-3.0-or-later like the project's
own code (README, "License"). By opening a pull request you confirm that you wrote the change (or name where it came
from) and that you may submit it under that licence.

Read [AGENTS.md](AGENTS.md) too: its rules (game data never enters the repository; only EXACT counts) apply to every
change, by a person or an agent.

## What you need

- **Your own copy of the game** (LittleBigPlanet, UCUS98744, disc version 2.00) and its `PSP_GAME/SYSDIR/EBOOT.BIN`
  decrypted on your own machine with a tool of your choice. This repository contains no game data and no decryption
  code. `tools/lbpbootstrap.py` checks the decrypted file against the SHA-256 in `data/modules.json`.
- **Windows with WSL** (Ubuntu 24.04, with `libc6-i386`) and **Python 3.12 or newer** with no third-party packages.
- **The compilers, which you build or install yourself** (none is in this repository):
  - the open-source GCC 4.x PSP compilers (`psp-gcc-4.0.4x`, the primary, and `4.0.2x`, `4.0.2h`, `4.1.0s`), built
    from the GNU and pspdev sources by [`toolchain/gcc4/`](toolchain/gcc4/README.md), which says exactly what to
    install, downloads and verifies the sources, and checks the result against the recorded builds;
  - the decomp.me `psp-gcc-1.3.1` kit (its Sony binutils 2.14 assembler is what the GCC 4 builds assemble with) and
    the `psp-gcc-1.7.1` kit (the `cc=gcc171-*` sources), from
    [decomp.me's compiler release](https://github.com/decompme/compilers/releases/tag/compilers);
  - PSPDEV binutils (`psp-ld`, `psp-objcopy`, `psp-objdump`), the release `toolchain/gcc4/README.md` names.

  `lbp-config.example.json` documents every key; matching needs `campaign_root`, `gcc4_home`, `gcc_home` and
  `pspdev_bin`. The other keys belong to maintainer tools.

## Setting up

Follow [Setup](docs/GUIDE.md#setup) in the project guide (or the README's short form), then check that one function
builds and that your setup reproduces the published matches:

```text
python tools/lbpconfig.py check
python tools/lbpmatch.py src/eboot/<file>.c
python tools/lbpmatch.py --all --parallel 4
```

## Never allowed

These hold for every file, commit, pull request, issue and comment, here and anywhere else:

- **No game data.** No retail or decrypted modules, no disassembly dumps or listings of the game, no hex dumps, no
  data files, images or audio taken from the disc, no keys. Your inputs stay under the gitignored `inputs/` and
  `campaign/`.
- **No fake matches.** No `asm` that emits general-register instructions (the inline VFPU rule below is the one
  exception), no `.word`/`.byte` directives, no included binaries, no `extra=` flags or byte masking in `src/`.
- **Nothing derived from the disc on web tools.** Do not post retail bytes, disassembly or decompiler output of the
  game to decomp.me scratches, pastebins, chat services, AI web services or public issues. In issues and pull
  requests, addresses, file names and tool status lines are fine.
- **No Sony SDK reconstructions**, no compiler or SDK header text, and no code from another project without its notice
  and an entry in `THIRD_PARTY.md`.
- **No private material**: no paths that name your user account, no tokens or keys, no personal data.

## Ground rules

- **A source enters `src/` only when `tools/lbpmatch.py` reports `EXACT` for it.** Exact means the compiled and linked
  bytes equal the retail bytes of the function, with no masking. There are no near misses in `src/`.
- Code taken from another project (newlib is the case so far) keeps its copyright and licence notices and is listed in
  `THIRD_PARTY.md`; do not remove a notice and do not paste a compiler's or SDK's header text into a source (the
  project-authored headers in `include/` exist for that). Library sources are produced by `tools/lbpnewlib.py`.
- Do not commit game data of any kind: no retail modules, no disassembly dumps of whole modules, no keys, no
  compilers. The ignore rules already exclude binaries and key files; keep it that way.
- Do not edit existing lines of `symbols/` (appending is allowed, see below), and do not edit `tools/` as part of a
  matching change.
- The tools that write into `src/` (`lbpgen`, `lbpclone`, `lbptmpl`, `lbpperm`, `lbpnewlib`) and the one that builds
  batches for matching agents (`lbpqueue`) are run deliberately by whoever runs the campaign, not casually while
  matching a function.
- Keep scratch work out of the tracked tree: use `try/<your-name>/` (gitignored). Build products go to
  `build/<module>/<dir>__<stem>__<digest>` (`tools/lbpmatch.py` `build_key`: the digest comes from the source's path,
  so equally named files in different directories never share build products).
- Do not search recursively (`grep -r`) outside `src/`, `wip/`, `docs/`, `symbols/`, `include/` and your own `try/`
  directory. The campaign directory and `build/` can hold gigabytes.

## Method

1. **Pick a function.** `python tools/lbpfamilies.py` ranks unmatched clone families (code identical apart from
   addresses): one match unlocks the whole family, because `lbpclone.py` propagates it. For single functions,
   `python tools/lbpqueue.py` builds batches, listing the Ghidra pseudocode path and, if one exists, the best
   mechanically generated candidate (`best_generated.source`), which is a good starting point.
2. **Read the code.** `python tools/lbpdis.py 0xADDR` prints the retail disassembly of any reviewed function, so look at
   the callees and callers too: they tell you types, argument counts and what a call returns. The Ghidra pseudocode
   is advisory and often wrong in detail (see the compiler notes below).
3. **Write the function.** Start from the generated candidate, or write it by hand. Prefer natural code (structs with
   fields, member functions, loops with `break`) over the decompiler's pointer arithmetic.
4. **Iterate.** `python tools/lbpmatch.py <file>` compiles, links at the retail address and compares;
   `python tools/lbpdiff.py <file>` then prints your disassembly next to retail's, with `**` marking differing words.
   Always pass explicit file names. `--flags` and a header `extra=` are for experiments and never count as a match.
5. **Budget.** About 40 compiles per function is a reasonable limit. When stuck, move on and come back later; if you
   end within a few words, save a near miss (below).
6. **Install** the source once it is EXACT (below), and re-run the matcher on the installed file.

External references: address-named symbols (`FUN_xxxxxxxx`, `DAT_xxxxxxxx` with exactly 8 hex digits) link at their
address automatically. Every other name must be in `symbols/<module>.txt` (C++ runtime helpers, vtables, mangled
member names). If a match needs a new named symbol, verify its address in the disassembly, check with `grep` that the
name is not already there, and **append** one line:

```text
printf '%s\n' 'name = 0xADDR;   # <your source file>' >> symbols/<module>.txt
```

A function whose constants or jump table live in `.rodata` needs `rodata=0xADDR` in the header, the address that
retail's `lui`/`lo` pair loads. The matcher then also requires the placed bytes to equal retail (`data=`, `sdata=`,
`bss=` and `sbss=` work the same way).

### Same-file static helpers

If retail's code depends on a `static` helper compiled earlier in the same file (for example MWCC keeping `a0` live
across a call it knows preserves `a0`, or a store in the `jal` delay slot), define the helper `static` in front of the
target, in retail address order, and list every function of the file other than the target in the header with its
own retail range: `helpers=FUN_0028fb44@0x0028fb44-0x0028fb70` (comma-separated for several). `start=`, `end=` and
`symbol=` stay the target's. The matcher links each function at its own address and compares every one; a helper that
differs is named in the mismatch, and a defined but unlisted function gives `MISPLACED`/`UNPLACED_FUNCTION`. The
source claims only the target's bytes: a helper is counted through its own one-function source, so install one if it
has none. Say in the note line that the helpers are placed by `helpers=`.

Only when the helper is directly adjacent and the run is short does the older form still apply: one source for the
contiguous run (`start=` the first function, `end=` the last, `symbol=` the first), named
`<name>_<first addr8>_run.cpp`, with "run" in the note line. Prefer `helpers=`.

## Source file format

**File name and place.** `src/<module dir>/` (the `src_dir` of the module in `data/modules.json`, e.g. `src/eboot/` for `EBOOT.BIN`); name the file
`<short_descriptive_name>_<addr8>.cpp` (or `.c`), where `<addr8>` is the start address in eight hex digits. Before
installing, check that nobody covered the function already (a duplicate must be removed, keeping the reviewed one):

```text
grep -l "start=0x<addr>" src/*/* wip/*
```

**Line 1: the header.**

```c
/* LBPMATCH module=EBOOT.BIN start=0x0011d894 end=0x0011dad8 symbol=FUN_0011d894 lang=c++ */
```

`module`, `start`, `end` and `symbol` are required (`symbol` must be the name your source defines for the function).
`lang` is `c99` or `c++`. Optional: `cc=<profile>` (absent means MWCC build 219; GCC sources say `cc=gcc171-O2`,
`cc=gcc171-Os` or, for unoptimized SDK code, `cc=gcc171-O0`), the section placement keys above, and the experimental `extra=`.

**Line 2: the note.** Exactly one provenance note, and it must be true. "Hand-written" means a person wrote it; a
source that started from a generated candidate names the generator tag.

- Written from scratch, or heavily restructured:
  `/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human review. */`
- Derived from a generated candidate with small corrections:
  `/* Ghidra pseudocode (lbpgen.py tag <tag>) corrected by hand: <what you changed>; exact match verified; names and types await human review. */`
- The tools write the other kinds themselves: `Mechanical translation of Ghidra pseudocode (lbpgen.py tag X)`,
  `Clone of src/... (lbpclone.py tag X)`, `... (template instance, lbptmpl.py tag X)` and
  `Library source: newlib <release> <dir>/<file>.c function <name>, ...`; the zlib sources name their release first
  (`zlib 1.2.3 (zlib.net, <archive> md5 <digest>), <file>.c; ...`).

A one-line pointer comment may sit between the header and the note; nothing else may. `python tools/lbplint.py` and
`python tools/lbpprcheck.py` reject a source without a known note kind there.

## Inline VFPU asm (LBP project rule)

Project rule (2026-10-27): where LittleBigPlanet's original source clearly used inline VFPU assembly, the matched
source reproduces it as inline asm. Requirements (checked by `tools/lbpgate.py`): the header has `asm=vfpu`; every
`asm` template holds only VFPU instructions (`lv.q`, `sv.q`, `vadd.q`, `vdot.q`, `mtv`, `mfv`, `vsync`, ...); a comment
line in the source starts `Original inline VFPU asm:` and says what in the retail code shows it (for example a block of
VFPU instructions with the compiler's own register moves around it that no C construct produces). General-register
instructions inside an asm template remain forbidden. hst-decomp keeps its stricter rule.

## Source quality

The owner reviews every source at the end, so aim for the code the developers wrote.

- Among exact variants, keep the most natural one: real control flow (loops, early returns, `switch`), structs and
  member access instead of `*(int *)(p + 0x14)` when the layout is clear, `bool` and enums where obvious.
- Avoid matching hacks: duplicated conditions, dummy locals or parameters that only steer register allocation,
  `volatile`, self-assignments, gratuitous casts or `goto`. When one of them is the **only** exact form you found,
  keep it, but name it in the note line (`matching idiom: <what>`); `lbplint.py` counts those.
- Names: use a descriptive name only when the evidence supports it (strings, callers, documented research findings);
  otherwise keep the Ghidra address names. Never invent semantics.
- Be consistent: one declaration per symbol across the tree (`lbplint.py` reports symbols declared with different
  types in different sources).

## Near misses

If a function ends within about six differing words but not exact, save your best attempt to
`wip/<short_name>_<addr8>.cpp` (the same line-1 header, once; a near miss needs no provenance note) with trailing `//`
comments: what differs and what you ruled out. Files
in `wip/` are not built by `--all` and are not verified; they are leads for the next person.

## Compiler notes

Read [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md) first: which compiler and flags built each part of LittleBigPlanet, with
the evidence. Where LBP shares a compiler with Hot Shots Tennis, hst-decomp's verified codegen facts apply unchanged:
[docs/reference/HST-MWCC-DIGEST.md](docs/reference/HST-MWCC-DIGEST.md) (by topic) and
[docs/reference/HST-MWCC-NOTES.md](docs/reference/HST-MWCC-NOTES.md) (the log, frozen copy). New LBP findings go to
`docs/TOOLCHAIN.md` (append one dated line under "Findings log", naming the function address).

## Checks before you finish

```text
python tools/lbpmatch.py <every file you added or changed>     # EXACT n/n
python tools/lbplint.py --show header                          # none of your files listed
python tools/lbpprcheck.py --base origin/main                  # what CI checks on your pull request
```

`lbpprcheck.py` needs no game or compiler: it refuses binary files, game data, never-versioned paths (`build/`,
`try/`, `inputs/`, `campaign/`, `lbp-config.json`), personal paths and tokens, disassembly-like listings, changed lines
in `symbols/`, sources without a valid header or note, and anything the fake-match gate refuses. When a change adds
files, `python tools/lbpreuse.py check` confirms that every tracked file is covered by the licence map (`REUSE.toml`);
new library sources need `python tools/lbpreuse.py newlib --write` first. `ORIGINS.json` is a snapshot made by
`python tools/lbporigin.py`; refresh it before a release (`lbppublish.py` classifies afresh).

For a change that touches many sources, `python tools/lbpmatch.py --all` is the full regression (it shares `build/`
with any concurrent run, so run one at a time).

## Pull requests

Fork the repository, make a branch, and open a pull request against `main`. Keep one topic per pull request: one
function, a family of clones of one function, one compiler finding, or one tool fix.

A pull request contains:

- the sources, each with its header and a true line-2 note (`Hand-written reconstruction ...` when you wrote it;
  name the generator tag when you started from a generated candidate), and any appended `symbols/` lines;
- in the description: the `python tools/lbpmatch.py <files>` output showing `EXACT` for each source, what you
  changed from any starting candidate, and, if you used an AI tool, which one;
- for a near miss in `wip/`: the differing words and what you ruled out, as trailing `//` comments.

What happens next: CI runs the checks above on the pull request (a first-time contributor's run waits for the
maintainer's approval). The maintainer reviews the change and **re-verifies every source with the maintainers' tools
before accepting it**: the source is compiled and compared against the retail bytes again and passes the promotion
gate, whatever the pull request's own output said. This repository is published from the maintainer's own tree, so
an accepted pull request is not merged with the button: the change is applied there, credited to you in the commit
and in the next publish commit's message, and the pull request is closed with a link to that commit.

Not accepted: anything under "Never allowed" above, Sony SDK reconstructions, sources that are not `EXACT`, edits to
existing `symbols/` lines, and tool changes mixed into a matching change.
