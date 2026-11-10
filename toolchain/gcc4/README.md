# GCC 4.x PSP compiler build kit

Source for the open-source GCC 4.x PSP cross compilers that this decomp's sources use. They are not Sony's own
compiler: they are GNU GCC releases with the open-source pspdev/ps2dev Allegrex patches, and the primary one adds four
changes this project chose by measurement on the retail code. Nothing here is derived from the game; the kit ships no
binaries. Run everything inside Linux (WSL Ubuntu 24.04 is what the recorded builds used), on the native filesystem.

| Build | Made from | Used by profiles (`cc=` in `src/`, counts at commit time) |
| --- | --- | --- |
| `psp-gcc-4.0.4c` | GCC 4.0.4 + pspdev patch 65aaa94632 + all six project changes | `gcc404xs-Os` (**PRIMARY** since 2026-11-09), `gcc404xs-O2`, every `gcc404x*` profile |
| `psp-gcc-4.0.4x` | GCC 4.0.4 + pspdev patch 65aaa94632 + project changes 1-5 | `gcc404p*`/`gcc404ps*`: the previous primary build (until 2026-11-09), kept to undo the migration and re-run any proof under it |
| `psp-gcc-4.0.2x` | GCC 4.0.2 + pspdev patch 65aaa94632 + the three tuning changes (no ins-from-zero) | `gcc402x-Os` (989), `gcc402x-O2` (151), `gcc402x-O1` (104) |
| `psp-gcc-4.0.2h` | GCC 4.0.2 + pspdev patch 65aaa94632, unchanged | `gcc402t-Os` (56), `gcc402t-O2` (10) (with `-mtune=r4000`) |
| `psp-gcc-4.1.0s` | GCC 4.1.0 + pspdev `gcc-4.1.0-PSP.patch` (7be303fec4), unchanged | `gcc410-Os` (1,975) |
| `psp-gcc-4.0.4h` | GCC 4.0.4 + pspdev patch 65aaa94632, unchanged | none: the base the 4.0.4x build is made from |

All are configured `--target=psp` (the pspdev patches' `mipsallegrexel-psp-elf`), C and C++, and assemble with the
Sony binutils 2.14 assembler (below). The profiles add `-G0 -fno-merge-constants -mpreferred-stack-boundary=4` and
more (`tools/lbpmatch.py` `gcc_profiles()`, `data/toolchain-manifest.json`). The decomp.me kits behind `gcc171-*`
and SNC behind `snc127-*` are not built here (576 and 9 sources).

## The project's changes (`patches/`)

The pspdev patch adds the Allegrex as a processor but leaves it untuned. On GCC 4.0.x this project measured and
adopted (owner decisions 2026-10-27 and 2026-10-28; evidence in `docs/TOOLCHAIN.md`):

1. `allegrex` appended to the `cpu` attribute in `mips.md` (it must list `enum processor_type`; without it no
   tune-specific description applies to the Allegrex) - `lbp-allegrex-tuning.diff`;
2. `TUNE_ALLEGREX` given the r4000/r6000 `MEMORY_MOVE_COST` (6) and `BRANCH_COST` (2) in `mips.h` - same diff;
3. an Allegrex scheduling description, `allegrex-sched.md` (loads latency 3; a store holds a memory unit for 3
   cycles), included before `generic.md` - same diff plus the `.md` file; it was hypothesis F-load3-st3 of the
   measured variants;
4. ins-from-zero (4.0.4x only): an SImode `and` with a constant mask that clears one contiguous run of bits and that
   `andi` cannot encode is emitted as `ins rd,$0,pos,len`, as the retail code does (`LBP_INS_CLEAR_LOW_ONLY 0`: any
   run, the adopted variant) - `lbp-ins-from-zero-4.0.4.diff`, applied after the tuning diff;
5. no transfer delay (4.0.4x only, adopted 2026-11-04): the Allegrex interlocks `mtc1`/`mfc1`, so no `nop` is put
   between them and the first use of their register (`ISA_HAS_XFER_DELAY` is off for the Allegrex; the
   floating-point compare delay stays), as the retail code does - `lbp-allegrex-xfer-4.0.4.diff`, applied last. It
   was added after the reproduction below; the current `cc1`/`cc1plus` hashes and fingerprint are those of the
   install made with it (PROVENANCE.json `xfer_change_2026_11_04`).
6. C1, copy order and asm in delay slots (4.0.4c only, adopted 2026-11-09): word-aligned block-copy loops store the
   first word, advance both pointers and store the rest at -12/-8/-4 (retail EBOOT: 295 such loops in 186
   functions, none in stock order), and a one-instruction `asm` may fill a delay slot and is volatile only when
   written `volatile` (retail fills slots with VFPU loads and vector operations) -
   `lbp-copy-order-asm-slot-4.0.4.diff`, applied after change 5. 4.0.4c = the 4.0.4x recipe plus this diff
   (`sh build.sh 4.0.4c`); 4.0.4x stays buildable as before. Full regression of all 6,474 sources under it: EXACT.

`trees/psp-gcc-<build>.sha256` holds the SHA-256 of every source file the pspdev patch and these changes touch, taken
from the recorded builds' work trees (for 4.0.2x, whose work tree a later experiment edited, from this kit's
rebuild, whose binaries equal the recorded ones); `build.sh` checks them before configuring.

## What you need

- Linux x86_64 with: `gcc`/`g++` (recorded: Ubuntu 24.04 `gcc` 13.3.0-6ubuntu2~24.04.1), GNU `make` (4.3),
  GNU `patch` (2.7.6), GNU `tar` with `bzip2`, `gpgv`, `wget` or `curl`, `sha256sum`; `libc6-i386` for the Sony
  assembler (a 32-bit program). The recorded host also had `libgmp-dev`/`libmpfr-dev` 6.3.0/4.2.1, `bison`,
  `flex`, `texinfo`; install the same set to reproduce the recorded configuration.
- **The assembler (not shipped).** The Sony `as` of the decomp.me `psp-gcc-1.3.1` kit:
  `GNU assembler 2.14+allegrex-vfpu-2.2.1-psp-1.3.1 20030612`. It encodes `move` as `addu` like the retail code (the
  pspdev assembler uses `or`). Get `psp-gcc-1.3.1.tar.gz` from the decomp.me compilers release
  (<https://github.com/decompme/compilers/releases/tag/compilers>; 17,194,959 bytes, SHA-256
  `788b6d2c0ed920da6b1e5a350d23dd7ea05820526e23f9b5b967b3b238ea2e2e`) and unpack it into `$HOME/hst-gcc/psp-gcc-1.3.1`
  (the directory `lbp-config.json` `gcc_home` names; the gcc131/gcc171 profiles use the same kit):
  `mkdir -p ~/hst-gcc/psp-gcc-1.3.1 && tar -xzf psp-gcc-1.3.1.tar.gz -C ~/hst-gcc/psp-gcc-1.3.1`.
  The assembler is then `psp/bin/as` in it (884,860 bytes, SHA-256
  `f16bc7a6a8e410626519d4cb256facc127bbfcfc356e97aee10abfbbb4bcf6e3`). The compilers call it through `sony-as`, a
  four-line wrapper in this directory (SHA-256 `79bfab65ad61ac1e0d908889439b6ac25831274e3e95787e91cc96554cb2f7a2`,
  pinned by `data/toolchain-manifest.json`) that drops `-mno-pdr` (the GCC 4.x psp spec passes it; binutils 2.14 does
  not know it) and runs `$HOME/hst-gcc/psp-gcc-1.3.1/psp/bin/as`. `build.sh` installs the wrapper as
  `$LBP_GCC4_ROOT/sony-as` when that is missing. The configured assembler path is compiled into the driver, which
  calls it for every `-c`, so it must stay at that path.
- **pspdev binutils (not shipped).** `PSPDEV_BIN` must name the `bin/` directory of a pspdev toolchain with `psp-ld`,
  `psp-as`, `psp-objdump`, `psp-objcopy`. The recorded builds used pspdev release `v20261001`
  (<https://github.com/pspdev/pspdev/releases/tag/v20261001>, asset `pspdev-ubuntu-latest-x86_64.tar.gz`,
  172,980,272 bytes, SHA-256 `a86efe624e770005290919617859e9df24bd8fcd3e364f4587f23ec7364b0acd`; GNU Binutils 2.44,
  Allegrex branch `a8b53fe2`); its `pspdev/bin/psp-ld` has SHA-256
  `d119ef5866b70ce46293b4ee7bb5ac356d44f93d802ca25580d3ad682b95937b`. `psp-ld` is the configured linker (compiled
  into the driver and `collect2`); configure also probes the linker and `psp-objdump` for features that end up in
  `cc1`, so another binutils release can change the compiler. Each install's `bin/` gets `psp-as`, `psp-ld`,
  `psp-objdump`, `psp-objcopy` links to `PSPDEV_BIN` and `psp-nm`/`psp-ar`/`psp-ranlib`/`psp-strip` links to the
  host tools, as the recorded installs have.
- About 0.6 GB per build in the work directory and a few minutes per build.

## Build

```sh
export PSPDEV_BIN=/path/to/pspdev/bin            # required
export LBP_GCC4_ROOT=$HOME/lbp-gcc4              # the prefix (this is the default); lbp-config.json gcc4_home
sh toolchain/gcc4/build.sh all                   # or any of: 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s
sh toolchain/gcc4/check.sh                       # compare with the recorded builds
```

`build.sh` first runs `fetch.sh`, which downloads what is missing into `$LBP_GCC4_DL` (default
`$LBP_GCC4_ROOT/dl`) and verifies every file against `sources.sha256` and every GNU tarball's signature with `gpgv`
against the GNU keyring (`GNU_KEYRING=<file>` for a local copy, otherwise fetched from ftp.gnu.org). For each build it
then: extracts the tarball, applies the pspdev patch with `patch -p1 -F3 -N` (the 2007 patch was written for 4.0.2;
on 4.0.4 it applies with fuzz and its `version.c` hunk is rejected, so 4.0.4h/4.0.4x report plain `4.0.4`; the
expected rejects and then `trees/` are checked), configures

```
../gcc-<ver>/configure --target=psp --prefix=$LBP_GCC4_ROOT/psp-gcc-<build> --enable-languages=c,c++ --disable-nls
  --disable-libssp --with-newlib --without-headers --disable-shared --disable-threads --disable-multilib
  --disable-werror --with-as=$LBP_GCC4_AS --with-ld=$PSPDEV_BIN/psp-ld
CC=gcc CFLAGS="-O1 -g0 -std=gnu89 -fcommon -fpermissive -w -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"  (4.1.0s also
  -DGMP_RNDN=MPFR_RNDN -DGMP_RNDZ=MPFR_RNDZ -DGMP_RNDU=MPFR_RNDU -DGMP_RNDD=MPFR_RNDD)
```

(old-dialect host flags only; no source change beyond the patches), runs `make -k all-gcc` (libgcc fails: the Sony
assembler rejects the `lwl`/`lwr` homebrew GCC emits for it; the compiler proper is complete by then) and installs
the compiler proper with `make install-driver install-common`. An x build copies its h build's work tree, applies the
project's diffs, copies `allegrex-sched.md` in, rebuilds `cc1` and `cc1plus` there and installs a copy of the h
install with the new `cc1`/`cc1plus`: so `psp-gcc-4.0.4x` keeps the 4.0.4h driver and embeds the
`psp-gcc-4.0.4h` prefix, exactly like the recorded build (this is why the x driver hashes equal the h ones).
Settings: see the head of `build.sh` (`LBP_GCC4_WORK`, `LBP_GCC4_AS`, `JOBS` default 2, ...). Work trees stay in
`$LBP_GCC4_WORK` (the x builds need their h tree); delete them when done.

## Check the result

`check.sh [build...]` does two comparisons:

1. **Binaries**: every installed program against `expected-sha256.txt`, the recorded installs (the hashes
   `data/toolchain-manifest.json` pins; all 36 non-link files there are in it).
2. **Code generation**: `fingerprint.sh` compiles the 671 GCC 4.0.4 C torture compile tests (from the verified
   tarball; no game material) with the installed `psp-gcc -S -Os ...`, once as C and once as C++, and hashes all the
   assembly (about 1,000 of the 1,342 compiles succeed; the rest fail the same way every time); the result must equal
   `fingerprints.txt`. This one does not depend on where the compiler is installed. It takes about a minute.

### What the hashes depend on

The binaries embed configured path strings, so their bytes depend on where they were configured:

| Program | Embeds |
| --- | --- |
| `bin/psp-gcc`, `psp-gcc-<ver>`, `psp-g++`, `psp-c++` | the prefix, the assembler path, the linker path, the configure command line |
| `libexec/gcc/psp/<ver>/cc1`, `cc1plus` | the prefix (include search directories) |
| `libexec/gcc/psp/<ver>/collect2` | the linker path |
| `bin/psp-gccbug` (a shell script) | the configure command line and prefix |
| `bin/psp-gcov` | nothing: identical for any prefix |

Nothing else machine-specific is embedded: no timestamps, work or build directories or host names (`-g0`; sources
are compiled by relative path). The configured paths themselves do name a home directory when the prefix is under
one. The recorded binaries were configured with the maintainer's prefix (`$HOME/lbp-gcc4/psp-gcc-<build>` of the
maintainer's account), `$HOME/lbp-gcc4/sony-as` and the maintainer's `PSPDEV_BIN`; those strings name a local
account and directory, so they are not published. A build configured anywhere else therefore differs in exactly
those strings (and in the code that addresses the strings after them), and `check.sh` reports BINARY_DIFFERS for the
programs above while the fingerprint matches. With the same path strings and the recorded host toolchain the
binaries are byte-identical (see Reproducibility).

## Reproducibility (2026-10-28)

All five builds were rebuilt from this directory, with sources fetched fresh from ftp.gnu.org and GitHub, on the
recorded host (Ubuntu 24.04 WSL, gcc 13.3.0, pspdev v20261001 binutils, the decomp.me 1.3.1 assembler), twice:

| Build | Same path strings as the recorded build | Fresh prefix (a different, longer path) |
| --- | --- | --- |
| `psp-gcc-4.0.4x` | 9 of 9 programs byte-identical; fingerprint identical | driver, cc1, cc1plus, gccbug differ; gcov, collect2 identical; fingerprint identical |
| `psp-gcc-4.0.2x` | 9 of 9 byte-identical; fingerprint identical | same pattern; fingerprint identical |
| `psp-gcc-4.0.2h` | 9 of 9 byte-identical; fingerprint identical | same pattern; fingerprint identical |
| `psp-gcc-4.1.0s` | 9 of 9 byte-identical; fingerprint identical | same pattern; fingerprint identical |
| `psp-gcc-4.0.4h` | 9 of 9 byte-identical; fingerprint identical | same pattern; fingerprint identical |

"Same path strings" ran `build.sh all` in a private mount namespace (`unshare -rm`) with an empty directory
bind-mounted over the recorded install root, so the configured prefix, assembler and linker strings equalled the
recorded ones without touching the recorded installs. In the fresh prefix, the differing programs differ only where
the paths are: `cc1`'s `.text` size is unchanged, `.rodata` grows by the longer include-directory strings, and the
remaining byte changes are the references to strings placed after them. `collect2` was identical there only
because `PSPDEV_BIN` was the same path. The 4.0.4x build is made in one step from the 4.0.4h tree (both diffs
together); the recorded one was made in two (three changes, then ins-from-zero); the binaries are the same.

Not reproducible by design: the recorded binary hashes for anyone whose configured paths differ (use the
fingerprint). Not tested: other host compilers or distributions, other pspdev releases (configure probes the
linker and objdump), and other GNU `patch` versions (the 4.0.4 fuzz application is checked by `trees/`).

## Licence

GCC 4.0.2, 4.0.4 and 4.1.0 and the pspdev/ps2dev Allegrex patches are GPL-2.0-or-later (GNU GCC; the patches modify
it). The kit's own files (the scripts, `sony-as`, the diffs, `allegrex-sched.md`, the hash lists) are
GPL-3.0-or-later, as `REUSE.toml` gives `toolchain/**`. The diffs and `allegrex-sched.md` modify and extend
GPL-2.0-or-later GCC files; "or any later version" lets them be combined under GPL-3.0-or-later, so a compiler built
with them is distributable under GPL-3.0-or-later, with its corresponding source being the GNU tarball, the pspdev
patch and these files. The Sony assembler and the pspdev binutils are not part of the kit; they come under their own
terms from the sources named above.

## Provenance

`PROVENANCE.json`: every source URL and SHA-256, the signature results, the user-supplied tools, the project's
change files, and per build the expected SHA-256 of every installed program, the fingerprint and the reproduction
result. The original experiment logs (the variant measurements behind change 3, the ins-from-zero evidence) are
summarised in `docs/TOOLCHAIN.md`.
