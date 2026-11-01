# Project guide

How to set up, verify and work in the LittleBigPlanet (PSP) matching decomp. The method, the source format
and most tools come from the Hot Shots Tennis: Get a Grip decomp (hst-decomp); this guide notes where LBP differs.

## Requirements

The tools run on a **Windows host and drive the compilers inside WSL** (they call `wsl.exe`), exactly as hst-decomp.

- Python 3.12 or newer (no third-party packages).
- WSL Ubuntu with `libc6-i386` (the Sony `psp-gcc` kits are 32-bit Linux programs).
- The compilers, found through `lbp-config.json` (see `lbp-config.example.json`): the GCC 4 builds of
  `toolchain/gcc4/` (`gcc4_home`; the primary profile and the comparison builds), the decomp.me psp-gcc 1.3.1 and
  1.7.1 kits (`gcc_home`), SN Systems SNC 1.2.7503 (`snc_home`) for the few SNC objects, and, only for the library
  tools `lbpnewlib.py`/`lbplib.py`, the newlib 1.13-1.15 source trees and built objects (`newlib_home`,
  `newlib_obj_root`). A shared machine may reuse the kits of the Hot Shots Tennis decomp read-only.
- PSPDEV binutils (`psp-ld`, `psp-objcopy`, `psp-objdump`) for linking at the retail address and disassembly.
- Your own decrypted copies of the game's modules (see Setup).

## Setup

1. Decrypt `PSP_GAME/SYSDIR/EBOOT.BIN` of your own disc locally (never in a public tree) with a tool of your choice,
   into `inputs/` (gitignored); this repository has no decryption code. If you have a PSP game decryptor script,
   `python tools/lbpintake.py --iso <your image> --decryptor <script>` runs it read-only and writes the decrypted
   modules under `inputs/`; the maintainers' `decryptor` setting names theirs, which is not published.
2. Describe your machine: `copy lbp-config.example.json lbp-config.json`, edit, then
   `python tools/lbpconfig.py check`. Every key can be overridden by `LBP_<KEY>`.
3. Create the campaign directory from the decrypted modules (checks each against `data/modules.json`):

   ```
   python tools/lbpbootstrap.py --out campaign --module EBOOT.BIN=inputs\...\EBOOT.BIN [--module NAME=PATH ...]
   ```

   and set `campaign_root` to it (the repository's own `campaign/` is gitignored).
4. `mkdir try` (scratch, gitignored).

## Verify

```
python tools/lbpmatch.py --all --parallel 4     # rebuild and compare every source under src/
python tools/lbpmatch.py src/eboot/<file>.c      # specific files
```

Statuses are hst-decomp's: `EXACT`, `MISMATCH`, `COMPILE_FAILED`, `LINK_FAILED`, `MISSING_SYMBOLS`,
`UNPLACED_SECTION`, `SECTION_MISMATCH`, `NO_SYMBOL`, `MISPLACED`, `EXACT_EXTRA_FLAGS`, `EXACT_FAILS_GATE`,
`UNPLACED_FUNCTION`. Only `EXACT` counts.

## Source conventions

**Header (line 1)**, as hst-decomp with the tag `LBPMATCH`:

```c
/* LBPMATCH module=EBOOT.BIN start=0x0001a2b0 end=0x0001a2f8 symbol=FUN_0001a2b0 lang=c99 cc=gcc171-O2 */
```

`module`, `start`, `end`, `symbol` are required; `lang=c99|c++`; `cc=` selects the compiler profile (absent =
`mwcc219`, mwccpsp 3.0.1 build 219 `-c -O4,p -sdatathreshold 0`; `gcc131-*`/`gcc171-*` the Sony psp-gcc kits);
`asm=vfpu` allows VFPU-only inline asm under the project rule in AGENTS.md (with an `Original inline VFPU asm:` reason line); `cc=snc127-O<n>` selects SN Systems SNC (pspsnc 1.2.7503, run natively);
`rodata=`, `data=`, `sdata=`, `bss=`, `sbss=` place the object's sections; `helpers=` places same-file static
helpers. Which profile LBP's game code needs is recorded in [TOOLCHAIN.md](TOOLCHAIN.md).

**Note (line 2)**, one of hst-decomp's kinds (`lbplint.py` checks them), plus one for this project:

| Kind | Starts with |
| --- | --- |
| hand-written | `Hand-written reconstruction from the disassembly;` |
| generated / corrected | `Mechanical translation of Ghidra pseudocode (lbpgen.py tag X)` / `Ghidra pseudocode (lbpgen.py tag X) corrected by hand: ...` |
| clone | `Clone of src/... (lbpclone.py tag X)` |
| cross-project clone | `Clone of hst-decomp src/... (lbpxclone.py tag X)`: an HST exact source with LBP's addresses |
| library | `Library source: newlib <release> <dir>/<file>.c function <name>, ...` |

## Tools

From hst-decomp (renamed `hst*` -> `lbp*`, module tables read from `data/modules.json` through `lbpproject.py`):
`lbpmatch` (matcher), `lbpgate` (fake-match gate), `lbpdiff`, `lbpdis`, `lbpcc`, `lbpcov`, `lbpclone`,
`lbpfamilies`, `lbplib` + `lbpnewlib` (newlib identification and exact library sources), `lbpgen` + `lbpcat` +
`lbpperm` (Ghidra pseudocode translation, mismatch classes, permuter), `lbpstrings`, `lbplint`, `lbporigin`,
`lbpsample`, `lbpconfig`, `lbpbootstrap` (generalised to any module list). Their documentation is in hst-decomp's
`docs/GUIDE.md`; behaviour is unchanged.

New here:

| Tool | Purpose |
| --- | --- |
| `lbpproject.py` | Module table (`data/modules.json`): names, src dirs, kinds; boundary statuses. |
| `lbpmodinfo.py` | Describe a decrypted module: digest, segments, sections, `.comment`, module info, imports, exports; `--modules-entry` writes its `data/modules.json` entry. |
| `lbpfuncs.py` | Decomp-oriented function boundaries from code and relocations (jal targets, exports, code pointers minus jump tables, code after the last return). Calibrated on hst-decomp's reviewed lists: 99.7% of starts. |
| `lbpbounds.py` | Agreement of boundary lists from other tools (Nakagawa `analyze.py` TOML, a Ghidra CSV). |
| `lbptoolchain.py` | Compiler fingerprints per module or address range (move/li encodings, branch-likely rate, prologue order, movz/movn). |
| `lbpxclone.py` | Cross-project clones: hst-decomp exact sources reused for byte-identical (up to addresses) LBP functions, verified by `lbpmatch.py`. |
| `lbppublish.py` | The would-be public tree (`--out`): tracked files minus Sony SDK / startup / unattributed sources (`lbporigin.py` classes from `data/origin-regions.json` and notes), disc-derived tables, the symbol map and maintainer files; `--audit` checks a tree (including that its objdiff report agrees with its figures); writes `data/public-figures.json` (bytes and function counts) and, from the same figures, `docs/objdiff-report.json`. Never touches a remote. |
| `lbpreport.py` | The objdiff progress report for bytematch.dev and recomp.fyi: `build_report` (used by `lbppublish.py`), and `--tree <public tree> [--check]` to write or compare a tree's report. Counts and percentages only. |
| `lbpqueue.py` | Batches for matching agents (`try/agent-queue/<name>-NN.json`): `--kind families` (one representative per unmatched clone family), `near` (lbpgen candidates within 35% of the words, wip leads), `singles`, `library`; flags `ins_zero_low`/`vfpu`; skips what earlier batches and lanes reserved or attempted. |
| `lbplane.py` | Maintainers only (not in the public tree): external agent lanes (`rewrite-harvest <lane>` takes a readability lane's rewrites of existing sources through the matcher and the gate, then refreshes their clones): `make <lane> <batch>` (isolated copy under `ext_root`, own build/), `harvest <lane>` (gate, main-tree re-verification, install EXACT only, re-proof with the committed symbols, then the promotion gate; wip/notes), `status`. Lane agents follow `docs/LANE-BRIEF.md`; their findings arrive in `docs/LANE-NOTES.md`. |
| `lbpreadable.py` | Readability score of the publishable sources (Ghidra-style names, `undefined` types, offset casts, the pasted typedef block); `lbpqueue.py --kind readability` ranks by it times (1 + clones). Rules for a pass: `docs/READABILITY-BRIEF.md`. |
| `lbpreuse.py` | Licensing: `check` (every tracked file covered by `REUSE.toml`, every licence text present) and `generate --write` (the newlib, zlib and libstdc++ annotation blocks and `third_party/newlib/README.md`). |
| `lbpclone.py --refresh <exemplar>...` | Re-derive the existing clones of rewritten exemplars (EXACT only, in place); then `lbppromote.py promote` the replaced files. |
| `lbppromote.py` | The promotion gate (hst-decomp's `hstpromote.py`, ported): the only writer of the accepted ledger. `lock`, `seed`, `promote [paths]`, `verify [--since REV]`, `migrate-toolchain --reason`, `retarget --reason`, `retract MODULE START --reason`. A proof comes only from a verification run in a fresh `build/proof/<run>/`: pre-checks (no `extra=`, a locked profile, whole functions of the locked inventory, lbpgate), `lbpmatch.py`, an independent re-compile that must give the matcher's object byte for byte (GCC's random-seed `_GLOBAL__I_` digits aside), and its own ELF reader's checks of the linked bytes against retail (code, placed `.rodata`/`.data`/`.sdata`, `.gnu.linkonce` objects, `.bss`/`.sbss` addresses, every linked symbol). One owner per byte; a promotion that would lose accepted bytes is refused. |
| `lbpproof.py` | The accepted ledger and its locks (`data/target-manifest.json`, `data/toolchain-manifest.json`, `data/accepted-ledger.jsonl`, `data/accepted-journal.jsonl`): `status`, `check [--base REV] [--staged]`, `coverage`, `spans MODULE`. The toolchain lock records the profiles `src/` uses plus the primary, with SHA-256 of their kits (WSL and SNC), binutils, `include/` and the comparison code. `lbpcov.py --proofs` counts from it. |
| `lbpbuild.py` | Whole-module build, tier T2 (design: `docs/PLAN.md`): every accepted source compiled by `lbpmatch.py` (object checked against its proof), reduced to its claimed code and compared data, linked in retail order at its retail address with retail-derived scaffolding for the rest (cut from your module at build time into `build/whole/`), the file assembled and compared by SHA-256; `report.json` and `provenance.json` (every byte: source or scaffolding). `--mode private|public|both`, `--reuse-match`, `--parallel 4`. T2 is reported only for a byte-identical image. |

## Never

- Commit or send retail bytes, decrypted modules, disassembly dumps, decompiler output or keys anywhere.
- Edit another project's tree, its campaign, or shared compiler kits from here (read-only reuse only).
