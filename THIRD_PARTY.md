# Third-party material

This file lists the third-party code that is **in** this repository, the third-party components that are **present
in the retail game** (and so may be reconstructed here), and the third-party tools that are **needed to build and
verify but are not included**. The project's own work is CC0-1.0, a public-domain dedication (GPL-3.0-or-later until
2026-11-23; the revisions published before then remain available under it), except where it derives from GPL code:
the GNU libstdc++/libsupc++ reconstructions and the GCC build kit in `toolchain/gcc4/` stay GPL-3.0-or-later.
`REUSE.toml` maps every file to its licence and `LICENSES/` holds the texts (`python tools/lbpreuse.py check`).

## In the repository

### newlib (C library function sources)

*newlib* (<https://sourceware.org/newlib/>) is the C library the game links. Its files carry BSD-style and other
permissive notices, kept in each source. The library sources in `src/` are the upstream files preprocessed with their
comments kept (`tools/lbpnewlib.py`); `third_party/newlib/README.md` lists every one with its release, upstream file
and terms, and `LICENSES/LicenseRef-Newlib-*` hold the texts.

### zlib 1.2.3

The game links Sony's reduced build of zlib 1.2.3 ("1.2.3.f-minzlib"). The zlib sources in `src/` (`zlib_*.c`) are
zlib 1.2.3's own code (<https://zlib.net/>, `zlib-1.2.3.tar.gz`, MD5 `debc62758716a169df9f62e6ab2bc634`), with
the changes Sony's build shows reconstructed and marked in each file's note. They keep zlib's notice:
Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler, under the zlib licence (`LICENSES/Zlib.txt`).

### GNU libstdc++ / libsupc++ (GCC 4.0)

The game links GCC 4.0's C++ runtime library. The sources classified `libstdc++` in `ORIGINS.json` reconstruct
functions of it (the address range with `.eh_frame` frame descriptions; `data/origin-regions.json`). Upstream:
the `libstdc++-v3` directory of GCC 4.0 (<https://gcc.gnu.org/>), Copyright (C) 1997-2005 Free Software Foundation,
Inc., licensed under GPL-2.0-or-later with the libstdc++ runtime exception. The reconstructions here are distributed
under GPL-3.0-or-later, which that licence permits. They derive from GPL code, so they are not part of the CC0
dedication: the project's own contribution to them stays GPL-3.0-or-later. The headers in `include/lbp/` that
describe libstdc++-style layouts (the red-black tree node, the copy-on-write string) were written from the game's
machine code and hold no libstdc++ text: they are the project's own work, under CC0-1.0 (owner decision 2026-11-23).

### Reconstructed game code

Everything else in `src/` and `wip/` is the project's reconstruction of LittleBigPlanet's own code, under CC0-1.0.
The dedication covers only the project's contributions; it grants no rights in the original work of Sony Interactive
Entertainment, Media Molecule or anyone else. The repository contains no game data: no executable, no decrypted
module, no disassembly or byte dumps, no keys.

## Third-party components present in the retail game (not in this repository)

The executable also links Sony SDK code: Sony's crt0, FIOS (file I/O), SCREAM audio and snd_stream, WWS pthread,
SceRT 3.04 networking (Dme, rt_cert, rt_crypt, rt_upnp), and a library built with SN Systems' SNC compiler. Its
reconstructions are kept out of this repository (`ORIGINS.json` classes `startup`, `sony-sdk`); a build takes those
functions from the user's own executable.

## Needed, not included

- The game: your own copy of LittleBigPlanet (PSP, UCUS98744 v2.00) and a way to decrypt its executable locally.
- GCC 4.0.x / 4.1.0 built for the PSP with the pspdev Allegrex patch and this project's measured changes:
  `toolchain/gcc4/` builds them from the GNU and pspdev sources (GPL; the project's patches and scripts are in that
  directory and stay GPL-3.0-or-later).
- The decomp.me PSP compiler kits (psp-gcc 1.3.1 and 1.7.1, whose binutils 2.14 assembler the GCC 4 builds use;
  SN Systems SNC 1.2.7503 for a few objects), from the decomp community's compiler archive. They are proprietary
  Sony / SN Systems software and are never included here.
- PSPDEV binutils (`psp-ld`, `psp-objcopy`, `psp-objdump`), GPL, from <https://github.com/pspdev>.
