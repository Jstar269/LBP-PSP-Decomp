# What 100% means

The goal of this project is a source tree from which the game's executables can be rebuilt, byte for byte, with
the original compilers, and then changed, relocated and rebuilt into whatever the owner wants. "100%" is defined here
once, at its strictest, so that no later requirement can lower a published percentage: every gate below is part of
the definition from the start, and the scope never shrinks.

## Scope (the denominator)

- **Modules:** every code module of the disc that the campaign ledger tracks: every module listed in
  `data/modules.json` (the game's executable(s) and the Sony modules the game ships and loads).
- **Bytes:** every byte of each decrypted module: code, initialised data, read-only data, small data, the size and
  placement of zero-initialised data, relocation tables, module info, and the import and export tables. The
  container encryption of the retail files is not part of the program and is not a target.
- **Nothing is removed from the denominator** because it is hard: a function the available toolchain cannot
  reproduce (`data/blocked_functions.json`) stays counted as not done until a toolchain or an explanation is found.
- Game data files on the disc (textures, models, audio, scripts) are inputs the build reads from the owner's own
  copy; they are not reconstructed here.

## Gates (a byte counts only when all of them hold)

1. **Exact.** The full module build reproduces the byte; a module is complete only when its whole rebuilt image has
   the same SHA-256 as the decrypted retail module.
2. **From source, with the original toolchain.** The byte comes from C or C++ compiled with the recorded profile
   (the compiler identified in docs/TOOLCHAIN.md) and recorded per-file settings, or from
   hand-written assembly only where the original was assembly. Never: retail bytes included as binary, machine code
   pasted into a source (`asm` statements that emit instructions, `.word`/`.byte` emission), object or byte patching,
   post-link fix-ups, changes to the verifier, skipped comparisons, or experimental flags (`extra=`).
3. **In its place in the whole module.** The source is compiled in its translation unit and linked in link order as
   part of the module build. Today's function-at-a-time matches (`tools/lbpmatch.py`) are a necessary step, counted
   in tier 1 below, not the goal.
4. **Data is source too.** Initialised and read-only data are defined in source (or are the pools the compiler
   generates from it), not binary blobs.
5. **Relocatable.** No numeric addresses in source: every reference is symbolic. A shifted build (padding inserted
   ahead of the code and the data, then rebuilt) still loads and runs a documented scenario, which proves the tree
   can be modified.
6. **Reproducible.** A clean checkout plus the owner's own disc builds every module with documented commands; no
   Ghidra project or private cache is needed.
7. **Reviewed.** Every source has passed human review (names and types); no placeholder `FUN_`/`DAT_` names remain
   in reviewed code, and no symbol is declared with conflicting types in different files.

## Tiers (how progress is reported)

The headline figure is **strict**: bytes passing all seven gates over all in-scope bytes. Because the gates and the
scope are fixed, it can only rise. The tiers below explain where the remaining work is; each is a share of the same
denominator.

| Tier | A byte is in this tier when |
| --- | --- |
| T1 | its function matches exactly in isolation (`lbpmatch.py`); code only |
| T2 | it is reproduced by the whole-module build from source (gates 1-4, 6) |
| T3 | T2, and the shifted build passes (gate 5) |
| T4 = strict | T3, and the source is reviewed (gate 7) |

Current figures: the README's "Progress at a glance" and "Progress in detail", which `tools/lbppublish.py` generates
from `data/public-figures.json` and dates by the newest accepted proof they count. T1 comes from the accepted ledger
(`python tools/lbpcov.py --proofs`); T2 from `python tools/lbpbuild.py` (the whole-module build, docs/PLAN.md), counted
only when the rebuilt file is byte-identical to the decrypted module and was built from the current ledger; scaffolding
and the container never count. T3 and T4: not measured (0). The first byte-identical rebuild was measured 2026-10-28
(ledger journal 48, 5,095 proofs): T2 = 427,216 B produced from source (425,848 code, 1,368 data), 10.449% of the
strict scope (4,088,394 bytes, both modules).

## Milestones (capabilities on the way)

As hst-decomp: every byte classified; game code matched (T1); all code matched; PSP rebuild of the game executable(s)
byte for byte; moddable (relocated) rebuild; every module rebuilt; portable source; platform layer (Nakagawa
Recomp's runtime); plays on Windows/Android hybrid, then from source; strict 100%. Each is reached only when its own
criterion fully holds, and a later drop is shown as REGRESSED, never hidden.

Nakagawa Recomp covers what this source cannot yet: its runtime is the platform layer and its recompiled code fills
unmatched functions in hybrid builds. Game-derived code never moves into Nakagawa Recomp.

## What 100% cannot mean

The original source text, comments and most names are not in the executables. A complete reconstruction compiles to
the same bytes and behaves the same; its names, types and structure are the project's best reconstruction, which is
why gate 7 requires human review.
