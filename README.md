<div align="center">

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/brand/lbp-psp-decomp-mark-dark.svg">
  <img src="docs/images/brand/lbp-psp-decomp-mark-light.svg" width="96" height="96" alt="lbp-psp-decomp mark">
</picture>

# lbp-psp-decomp

**Matching decompilation of LittleBigPlanet for the PSP.**

**[Progress](#progress-at-a-glance) · [Guide](docs/GUIDE.md) · [Contributing](CONTRIBUTING.md) · [Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp) · [Hot Shots Tennis decomp](https://github.com/Jstar269/hst-decomp)**

[![Code matched](https://bytematch.dev/Jstar269/lbp-psp-decomp.svg?label=code%20matched&measure=matched_code_percent)](https://bytematch.dev/Jstar269/lbp-psp-decomp)
[![Functions matched](https://bytematch.dev/Jstar269/lbp-psp-decomp.svg?label=functions&measure=matched_functions)](https://bytematch.dev/Jstar269/lbp-psp-decomp)
[![License: GPL-3.0-or-later](https://img.shields.io/badge/license-GPL--3.0--or--later-blue.svg)](#license)
[![REUSE status](https://api.reuse.software/badge/github.com/Jstar269/lbp-psp-decomp)](https://api.reuse.software/info/github.com/Jstar269/lbp-psp-decomp)
[![Platform: PSP (UCUS98744)](https://img.shields.io/badge/PSP-UCUS98744-555555.svg)](#lbp-psp-decomp)

</div>

This project rebuilds the code of *LittleBigPlanet* (PSP, UCUS98744, disc version 2.00) as C and C++. A function counts
as matched only when its compiled and linked bytes equal the original exactly.

It covers the game's executable (`EBOOT.BIN`): the game's own code and the open-source libraries it links (newlib,
zlib 1.2.3, GNU libstdc++/libsupc++). The whole executable, rebuilt from these sources plus scaffolding cut from your own
copy, is byte-identical to the original (SHA-256 equal).

The repository holds source only and doesn't build a playable game on its own. Running PSP games natively on a PC is
the work of [Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp), which needs your own copy of the game.

<div align="center"><sub>LittleBigPlanet belongs to its publisher. This repository contains no game data: you need your own copy of the game, and nothing here downloads one.</sub></div>

## Progress at a glance

<!-- lbpfigures:begin readme-glance -->

| | Progress | What it counts |
|---|---|---|
| **Code matched** (T1) | **17.9%** | 511,672 of the 2,860,624 code bytes of the public scope come from C or C++ that compiles, function by function, to the game's exact bytes. |
| **Functions matched** (T1) | **42.5%** | 4,979 of the 11,712 functions of the public scope. |
| **Built from source** (T2) | not measured | No whole-module build of the current accepted proofs yet (`python tools/lbpbuild.py`). |

Code and functions matched are the figures the trackers ([bytematch.dev](https://bytematch.dev/Jstar269/lbp-psp-decomp), [recomp.fyi](https://recomp.fyi)) show. The public scope is the game's own code and the open-source libraries it links; Sony SDK code the game links is not published and not counted here, but stays in the project's strict finish line ([`docs/COMPLETION.md`](docs/COMPLETION.md)). Measured from the maintainer's private ledger of accepted proofs of 2026-11-05 by `tools/lbppublish.py`, which needs that ledger.

<!-- lbpfigures:end -->

T1 counts each function matched on its own; T2 counts what the whole executable, rebuilt from this source, reproduces in
place. Decompilation projects usually count a game as done when both reach 100%.
[`docs/COMPLETION.md`](docs/COMPLETION.md) defines every tier, and [`data/public-figures.json`](data/public-figures.json)
has the figures.

## Contribute, or check the matches yourself

Contributions are welcome: matched functions, near misses with notes, compiler findings, tool fixes and documentation.
[CONTRIBUTING.md](CONTRIBUTING.md) says what you need and what a pull request contains, and issues labelled
[good first issue](https://github.com/Jstar269/lbp-psp-decomp/labels/good%20first%20issue) are small functions to
start with. Every task here needs your own copy of the game. You can also re-check every published match on your own
machine:

1. Bring your own copy of the game. The tools need its executable, `PSP_GAME/SYSDIR/EBOOT.BIN`, decrypted on your
   own machine into `inputs/` (gitignored) with a tool of your choice, for example Nakagawa Recomp's `nk_decrypt`
   ([how to run it](https://github.com/Jstar269/nakagawa-recomp/blob/main/docs/SETUP.md#encrypted-executables));
   this repository contains no decryption code. Its SHA-256 must equal the one in `data/modules.json`.
2. Install the compilers in WSL (`toolchain/gcc4/` builds the GCC 4.0.4, 4.0.2 and 4.1.0 compilers the sources use,
   with the pspdev Allegrex patch and this project's measured Allegrex changes; the decomp.me kits add psp-gcc 1.7.1
   and SNC) and set up the campaign directory: [Setup](docs/GUIDE.md#setup).
3. Run `python tools/lbpmatch.py --all --parallel 4`.

**Where does a fix belong?** If the game misbehaves in the recompiled player, the cause is usually general: a missing
or wrong system-call behaviour, an instruction form, a display-list command or a build step. That work belongs in
[Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp), where it helps every game and needs no disc to test.
Work belongs here only when it rebuilds a function's source so that the compiled bytes equal the original, and it
needs your own copy of the game.

The compiler findings are in [`docs/TOOLCHAIN.md`](docs/TOOLCHAIN.md). Never post bytes, disassembly or decompiler
output of the game to web tools such as decomp.me, pastebins or public issues.

## Progress in detail

<!-- lbpfigures:begin readme-progress -->

**4,979 functions** matched exactly in isolation (T1), measured from the maintainer's private ledger of accepted proofs of 2026-11-05 by `tools/lbppublish.py`, which needs that ledger and counted in code bytes of the public scope:

| Module | Matched / functions | Matched / code bytes | Share of bytes |
|---|---|---|---|
| `EBOOT.BIN` | 4,979 / 11,712 | 511,672 / 2,860,624 | 17.887 % |

`EBOOT.BIN` built from source (T2): not measured (no whole-module build report: python tools/lbpbuild.py).

<!-- lbpfigures:end -->

Every source is re-verified with `python tools/lbpmatch.py` before it is published, and the figures count only the
sources the promotion gate (`tools/lbppromote.py`) has accepted. Progress trackers (bytematch.dev, recomp.fyi, and the badges above) read
[`docs/objdiff-report.json`](docs/objdiff-report.json), an objdiff report written in the same run as
`data/public-figures.json` and from the same figures. Its units are the game's translation units, recovered from the
program's static constructors ([`data/translation-units.json`](data/translation-units.json): the program keeps no file
names, so each is numbered and named by its first function); the open-source library code outside them has one unit
per published source and one "not yet recovered" unit per function no source matches yet. bytematch.dev draws a tile
per unit and, inside one, a tile per function. The functions come from the published boundary list
([`data/functions-EBOOT.BIN.txt`](data/functions-EBOOT.BIN.txt)) and
[`data/public-functions.txt`](data/public-functions.txt) (`python tools/lbpreport.py --check` compares them). Public CI cannot build the project, so
[`progress-report.yml`](.github/workflows/progress-report.yml) uploads the committed file as the artifact
`UCUS98744_report`.

## How the sources are made

The method and most tools come from [hst-decomp](https://github.com/Jstar269/hst-decomp). Where LittleBigPlanet
contains code that hst-decomp has already matched, a tool reuses that source; open-source library functions are
translated from their published sources; other tools translate decompiler output into C. AI agents work on the
functions the tools can't match, under the maintainer's direction.

Every source passes the same checks. An automatic checker compiles it with the recorded compiler profile and accepts it
only if it produces exactly the original bytes, and a second gate rejects fake matches, such as machine code pasted in
as assembly instead of written as C. [`ORIGINS.json`](ORIGINS.json) gives the origin class of every source.

Not included: Sony SDK code the game links (FIOS, SCREAM audio, SceRT networking, the SNC-built library, Sony's crt0,
and small SDK functions identical to ones in Hot Shots Tennis) and code not attributed yet. A build takes those
functions from your own executable.

## Legal

No game assets, executables, keys, decryption code or compilers are in this repository, and nothing in it lets you
obtain the game. You build or install the compilers yourself: GCC from the GNU and pspdev sources with
[`toolchain/gcc4/`](toolchain/gcc4/README.md), and the [decomp.me compiler kits](https://github.com/decompme/compilers),
under their own licences ([THIRD_PARTY.md](THIRD_PARTY.md)).

This is an unofficial fan project, not affiliated with or endorsed by Sony Interactive Entertainment or Media Molecule.
LittleBigPlanet, PlayStation and PSP are trademarks of Sony Interactive Entertainment Inc. or its affiliates, used here
only to identify the game and platform.

Rights holders with a concern about anything in this repository can contact the maintainer privately through
[GitHub private reporting](https://github.com/Jstar269/lbp-psp-decomp/security/advisories/new) or the contact on the
[maintainer's profile](https://github.com/Jstar269). We aim to respond promptly and will remove material where
appropriate.

## License

Licensed by path ([REUSE](https://reuse.software/)): the project's own code (`src/`, `wip/`, `symbols/`), tools and
documentation are **GPL-3.0-or-later** (`LICENSE`). Third-party code keeps its own terms: newlib (the notices in each
source; `third_party/newlib/`), zlib 1.2.3 (zlib licence), GNU libstdc++/libsupc++ (GPL-2.0-or-later with the runtime
exception, distributed here under GPL-3.0-or-later). The GPL covers only the project's own work, not the original game.
`REUSE.toml` maps every file (`python tools/lbpreuse.py check`); [THIRD_PARTY.md](THIRD_PARTY.md) has the details.

## Learn more

[Project guide](docs/GUIDE.md) · [What 100% means](docs/COMPLETION.md) · [Contributing](CONTRIBUTING.md) ·
[Compiler findings](docs/TOOLCHAIN.md) · [Security](SECURITY.md) · [Code of conduct](CODE_OF_CONDUCT.md) ·
[bytematch.dev](https://bytematch.dev/Jstar269/lbp-psp-decomp)

One family of projects by the same maintainer, with the same rules: [Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp)
runs PSP games natively on a PC, and the matching decompilations are
[hst-decomp](https://github.com/Jstar269/hst-decomp) (Hot Shots Tennis: Get a Grip) and lbp-psp-decomp (this one).
