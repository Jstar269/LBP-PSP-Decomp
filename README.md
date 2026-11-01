# LittleBigPlanet (PSP) decompilation

A matching decompilation of LittleBigPlanet for the PSP (UCUS98744, disc version 2.00). It rebuilds the game's
executable as C and C++. A function counts only when its compiled and linked bytes equal the original exactly.

This repository contains **no game data**: no executable, no decrypted module, no disassembly or byte dumps, no keys.
You build and verify it against **your own copy of the game**. It is not affiliated with or endorsed by Sony
Interactive Entertainment or Media Molecule.

Related: [Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp) (PSP games as native Windows programs) and
[hst-decomp](https://github.com/Jstar269/hst-decomp) (Hot Shots Tennis: Get a Grip).

## Progress

The whole executable, rebuilt from these sources plus scaffolding cut from your own copy, is byte-identical to the
original (SHA-256 equal). The published sources produce 393,372 code bytes of it, 13.752% of the public scope;
scaffolding never counts. Measured by the maintainers on 2026-10-28 with `tools/lbpbuild.py` (tier T2), which reads
the accepted proofs that stay private.

The public scope is the game's own code and the open-source libraries it links. Sony SDK code stays in the strict
100% scope ([`docs/COMPLETION.md`](docs/COMPLETION.md)) but not in the public figure.
[`data/public-figures.json`](data/public-figures.json) has the figures.

Progress trackers read [`docs/objdiff-report.json`](docs/objdiff-report.json), an objdiff report with the module
totals and no per-function detail. Its number is the one in `data/public-figures.json`: the maintainers measure that
file from the private proofs, then run `python tools/lbpreport.py` and commit the result (`--check` compares them).
Public CI cannot build the project, so [`progress-report.yml`](.github/workflows/progress-report.yml) only uploads the
committed file as the artifact `UCUS98744_report`.

## Contents

- `src/`: the reconstructed game code, and matched open-source library code the game links (newlib, zlib 1.2.3, GNU
  libstdc++/libsupc++), each under its own licence (see the notice in each library source).
- `symbols/`: the names this project gives to addresses in the executable (no code or data from the game).
- `ORIGINS.json`: the origin class of every source.
- Not included: Sony SDK code the game links (FIOS, SCREAM audio, SceRT networking, the SNC-built library, Sony's
  crt0, and small SDK functions identical to ones in Hot Shots Tennis) and code not attributed yet. A build takes
  those functions from your own executable.

## Requirements

- A Windows host with WSL (Ubuntu, `libc6-i386`), and Python 3.12 or newer with no third-party packages.
- GCC 4.0.4 with the pspdev Allegrex patch and this project's measured Allegrex changes, built from the GNU and
  pspdev sources by `toolchain/gcc4/` (evidence in [`docs/TOOLCHAIN.md`](docs/TOOLCHAIN.md)).
- The decomp.me PSP compiler kits (their binutils 2.14 assembler, and the few objects built with other compilers),
  and PSPDEV binutils (`psp-ld`, `psp-objcopy`, `psp-objdump`).
- Your own LittleBigPlanet disc image, and a way to decrypt its executable on your machine (a tool of your choice;
  this repository has no decryption code).

## Setup

1. Decrypt `PSP_GAME/SYSDIR/EBOOT.BIN` from your own disc locally into `inputs/` (gitignored), with a tool of your
   choice. Its SHA-256 must equal the one in `data/modules.json`.
2. `copy lbp-config.example.json lbp-config.json`, fill in your paths, then run `python tools/lbpconfig.py check`.
3. Describe and check your module (its SHA-256 must equal `data/modules.json`), make the function list from it, and
   create the campaign directory:

   ```text
   python tools/lbpmodinfo.py inputs/<...>/EBOOT.elf
   python tools/lbpfuncs.py --elf inputs/<...>/EBOOT.elf --out data/functions-EBOOT.BIN.txt
   python tools/lbpbootstrap.py --out campaign --module EBOOT.BIN=inputs/<...>/EBOOT.elf
   ```

4. `mkdir try` (scratch, gitignored), then verify every source: `python tools/lbpmatch.py --all --parallel 4`.

## Licensing

The project's own code, tools and documentation are GPL-3.0-or-later (`LICENSE`). Third-party code keeps its own terms:
newlib (the notices in each source; `third_party/newlib/`), zlib 1.2.3 (zlib licence), GNU libstdc++/libsupc++
(GPL-2.0-or-later with the runtime exception, distributed here under GPL-3.0-or-later). `REUSE.toml` maps every file
(`python tools/lbpreuse.py check`); `THIRD_PARTY.md` has the details. The GPL grant covers only this project's
contributions, not the original game.

## Contributing

Contributions are welcome: matched functions, near misses with notes, compiler findings, tool fixes and
documentation. [`CONTRIBUTING.md`](CONTRIBUTING.md) says what you need (your own copy of the game, the compilers you
install yourself), the method, what a pull request contains and what is never allowed; [`docs/GUIDE.md`](docs/GUIDE.md)
has the tools. Issues labelled
[good first issue](https://github.com/Jstar269/lbp-psp-decomp/labels/good%20first%20issue) are small functions to
start with.

If the game misbehaves in the recompiled player, the fix usually belongs in
[Nakagawa Recomp](https://github.com/Jstar269/nakagawa-recomp) (system calls, instructions, graphics, the build); work
belongs here when it rebuilds a function's source so that it compiles to the original bytes.

Never post bytes, disassembly or decompiler output of the game anywhere: not in issues or pull requests, not on web
tools such as decomp.me.
