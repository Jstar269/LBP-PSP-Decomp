# Toolchain

Which compiler built which part of LittleBigPlanet (PSP), with the evidence, and the facts learned while matching.

`lbp-gcc4/` below is the maintainers' compiler build kit, where these measurements were made: build scripts, the GNU
and pspdev sources with their signatures, this project's diffs and scheduling descriptions, and `PROVENANCE.json`
with every hash. Its reproducible form is in this repository: `toolchain/gcc4/` (README there). The compilers are
installed in WSL under `~/lbp-gcc4/` (setting `gcc4_home`).

## Status (2026-10-27 ~03:45Z, disc UCUS98744 v2.00)

`EBOOT.BIN` is a static ELF executable (ET_EXEC at 0x08804000, no relocations; `.init`/`.fini`, `.ctors`/`.dtors`,
`.jcr`, `.eh_frame`, `.gcc_except_table`; `.comment` stripped; module `LBPPSP`, module SDK version word 0x05050010 =
SDK 5.05). Two compiler families are present:

1. **GCC, almost all of the code** (about 11,550 of 15,562 functions by fingerprint, 3.69 MB of 3.83 MB).
   Evidence of the family: GCC crt layout (`.init`/`.fini`/`.jcr`, one CIE per object in `.eh_frame`), GNU
   libstdc++/libsupc++ (`St9exception`, `N10__cxxabiv117__class_type_infoE`, `basic_string::_S_construct NULL not
   valid`, `N9__gnu_cxx14recursive_initE`, the guard exception class of libsupc++ from GCC 4.0 to 4.3), a newlib-style C
   library, `-G0`, Allegrex `ext`/`ins`, heavy inline VFPU use.
   **Which GCC (2026-10-27 measurements, `try/gcctest`, `lbpsigscan.py`):**
   - `ins rd,$zero` for bit-field clears: LBP 1,344 times, `move rX,zero; ins` 7 times. The decomp.me GCC 3.3.3 kits
     (1.3.1, 1.7.1 `-mext`) emit `move v1,zero; ins`; GCC 4.x emits `ins v0,zero` like LBP.
   - Frame layout: `ra` at the frame top in 18.6% of LBP's `ra`-saving frames, 9-22% for GCC 3.3.3, 88-100% for GCC
     4.3.2 (the MIPS frame rework of GCC 4.3). So LBP's GCC is older than 4.3.
   - MIPS sibling calls (`j` tail calls with the epilogue in the delay slot) are present in LBP; GCC 3.3.3 never
     emits them on MIPS (TARGET sibcalls arrived in 3.4).
   - Probe of four small game functions: GCC 4.1.0 (open-source, pspdev Allegrex patch, assembled with the Sony
     binutils 2.14+allegrex assembler, `-Os -mpreferred-stack-boundary=4`) is EXACT on 0x089baa90, 0x089ea93c and
     0x0891c00c, where the 3.3.3 kits fail; the fourth (0x0894fad8) is EXACT with the 3.3.3 kits and one
     instruction-scheduling swap away under 4.1.0. GCC 4.3.2 (pspdev patch) is far off on all four.
   - Library code: newlib `_Bfree`, `std`, `abort` and ctype are EXACT with 3.3.3 `-O2`; `_Balloc` differs from every
     tried build (3.3.3 at O1/O2/O3/Os, 4.1.0, 4.3.2; newlib 1.13-1.15).
   - At scale (`lbpgen.py` over Ghidra pseudocode, 600 functions of 12-96 bytes): `gcc410-Os` gives 140 EXACT;
     only 39 of those 140 are also EXACT with the GCC 3.3.3 kit at `-Os` (31 at `-O2`).
   - Whole pass over 12-96-byte functions (2026-10-27): `gcc410-Os` 1,275 EXACT; a following `gcc171-Os` pass on
     what 4.1.0 missed adds 159, scattered over the whole game code (not one library); 100-256 bytes with
     `gcc410-Os`: 48 more (mechanical translations get rarer with size).
   **Conclusion:** the game code was built by a GCC 4.0-4.2-era Sony psp-gcc (sibling calls, `ins $zero`, pre-4.3
   frame layout, GCC 4.0+ libsupc++), most likely at `-Os`, with a 16-byte stack alignment and the Sony binutils
   (moves encoded as `addu`). The open-source GCC 4.1.0 + pspdev Allegrex patch build (profile `gcc410-*`, provenance
   in `lbp-gcc4/PROVENANCE.json`) reproduces part of it exactly and is the working profile;
   whether Sony's own build differs elsewhere (its Allegrex scheduling model, `lwl`/`lwr` avoidance) is measured
   function by function. Some objects (the SDK C library, perhaps middleware) match GCC 3.3.3 and keep `gcc171-*`.
2. **SN Systems SNC, a small linked library** (about 63 functions, 17 KB, mostly 0x08b15000-0x08b52d00): `move` as
   `or`, constants as `ori rt,zero,K`, s-registers stored from the frame bottom with `ra` after them. The decomp.me
   kit `pspsnc` 1.2.7503 at `-O2` reproduces it: 0x08b151d8 is EXACT (`cc=snc127-O2`, the matcher's new SNC profile).

## Compiler identification (2026-10-27, owner decision: `gcc410` not yet accepted)

The owner asked for a closer match to Sony's own psp-gcc 4.x than the homebrew GCC 4.1.0 build. Every candidate was
measured on one fixed sample with `tools/lbpcceval.py` (same compile, link and exact comparison code as
`lbpmatch.py`, imported; results are experiments and never written to `src/`).

**Fixed evaluation sample** (`data/cceval-sample.json`, addresses only; `python tools/lbpcceval.py sample`, seed
20261027; regenerate the miss translations with the same command, the sources stay under `try/cceval/sample/`):

| Part | Functions | Words | What |
| --- | --- | --- | --- |
| `gcc171` | 159 | 2,166 | every source matching only with the GCC 3.3.3 kit (lbpgen tag g3, `gcc171-Os`) |
| `gcc410` | 150 | 1,144 | random sample of the 1,323 lbpgen matches with `gcc410-*` (tags g1, g2, g4) |
| `miss` | 150 | 3,736 | random sample of the 3,532 functions whose Ghidra v0 translation linked but did not match (g2/g4 pass 1), source regenerated with lbpgen's translator |

`words left` = differing words summed over the sample (a source that does not compile or link counts all its
words). The `gcc410` part is biased towards the 4.1.0 build (it was selected by it) and the `gcc171` part towards
3.3.3; the `miss` part is the unbiased one, so read the three columns, not only the total.

**Candidates built** (all open source: GNU release tarballs, gpgv-verified against the GNU keyring, plus public
pspdev/psptoolchain patches; built in WSL with `lbp-gcc4/build_gcc4_psp_s.sh`, assembling
with the Sony binutils 2.14 assembler; provenance, URLs, SHA-256 and signatures in
`lbp-gcc4/PROVENANCE.json`):

- GCC 4.0.2, 4.0.3, 4.0.4 + the pspdev GCC 4.0.2 patch revision 65aaa94632 (2007-01-23, "hlide's
  mpreferred-stack-boundary patch"): GCC 4.0.x with the PSP's 16-byte stack alignment. 4.0.1 with that patch does not
  apply (config.gcc); the 4.0.0 and 4.0.1 patch revisions were fetched but not built.
- GCC 4.1.1, 4.1.2 + the 2008 pspdev gcc-4.1.0-PSP.patch (the 2006 original differs only by the stack option).
- GCC 4.2.0-4.2.4: pspdev never shipped a 4.2 patch; the 4.1.0 patch ported here (two bogus config.gcc hunks
  dropped, one misplaced `TUNE_ALLEGREX` line fixed; `src2/gcc-4.2.4-PSP-port.patch`).
- Flags on the best compilers: `-O1/-O2/-O3/-Os`, `-fno-schedule-insns2`, `-fschedule-insns`,
  `-fno-sched-interblock`, `-fno-sched-spec`, and every `-mtune=` model of GCC 4.0's MIPS backend.

**Results on the fixed sample** (EXACT / words left; full list: `python tools/lbpcceval.py table`):

| Rank | Candidate (profile) | Compiler, flags (all `-G0 -fno-merge-constants`, 4.x with `-mpreferred-stack-boundary=4`) | EXACT | words left | `gcc171` part | `gcc410` part | `miss` part |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | `gcc402t-Os` | 4.0.2h `-Os -mtune=r4000` (same: `-mtune=r6000`) | 196 | 3,434 | 46/159 (570) | 149/150 (10) | 1/150 (2,854) |
| 2 | | 4.0.2h `-Os -mtune=r4130` | 178 | 3,582 | 40 (684) | 133 (59) | 5 (2,839) |
| 3 | | 4.0.2h `-Os -mtune=vr5400` | 176 | 3,613 | 41 (689) | 129 (92) | 6 (2,832) |
| 4 | | 4.0.2h `-Os -mtune=r4000 -fno-schedule-insns2` | 165 | 3,777 | 48 (692) | 116 (188) | 1 (2,897) |
| 5 | | 4.1.0 `-Os -mtune=r4000` | 164 | 3,711 | 14 (861) | 150 (0) | 0 (2,850) |
| 6 | `gcc402h-Os` | 4.0.2h / 4.0.3h / 4.0.4h `-Os` (identical results) | 159 | 3,617 | 8 (785) | 150 (0) | 1 (2,832) |
| 7 | | 4.0.2h `-Os -mtune=r3000/r3900/r4600/r5000` | 153 | 3,649-3,699 | 9 | 144 | 0 |
| 8 | `gcc410-Os` | 4.1.0 / 4.1.1 / 4.1.2 `-Os` (current working profile) | 150 | 3,857 | 0 (928) | 150 (0) | 0 (2,929) |
| 9 | | 4.0.2h `-Os -fno-schedule-insns2` | 132 | 3,969 | 14 (900) | 117 (178) | 1 (2,891) |
| 10 | `gcc402-Os` | 4.0.2 old patch (8-byte stack) `-Os` | 129 | 3,880 | 6 (930) | 122 (60) | 1 (2,890) |
| 11 | | 4.2.0-4.2.4 `-Os` (identical) | 127 | 4,964 | 0 (1,521) | 127 (242) | 0 (3,201) |
| 12 | | 4.0.2h `-O2` (= `-O3`, = 4.0.3h/4.0.4h) | 126 | 4,646 | 6 | 117 | 3 |
| 13 | | 4.1.0 `-O2` (= `-O3`, = 4.1.1/4.1.2) | 124 | 4,511 | 2 | 119 | 3 |
| 14 | | 4.2.4 `-O2` | 112 | 5,223 | 2 | 108 | 2 |
| 15 | | 4.0.2h / 4.1.0 `-O1` | 41-43 | 5,289-5,366 | 13-15 | 23 | 5 |
| ref | `gcc171-Os` | Sony psp-gcc 3.3.3 kit `-Os` (decomp.me 1.7.1) | 205 | 3,499 | 159 (0) | 46 (492) | 0 (3,007) |

On every GCC source in `src/` (`lbpcceval.py run --src-set`; 1,326 sources with a 4.x profile, 569 with a 3.3.3
profile, 406 of those hst cross-clones): `gcc402t-Os` 1,728 EXACT (1,311 + 417), `gcc402h-Os` 1,704 (1,325 + 379),
`gcc410-Os` 1,697 (1,326 + 371).

**Findings.**
- GCC 4.0.x beats 4.1.x, which beats 4.2.x; point releases inside a series make no difference on this sample.
  4.2 loses about 15% of the 4.1.0 matches (frame and scheduling changes), so LBP's compiler is not a 4.2.
- The PSP 16-byte stack alignment is required (4.0.2 without it: 122/150 of the 4.1.0 matches).
- The largest single gain is `-mtune=r4000`: 38 more of the 3.3.3-only functions become EXACT under GCC 4.0.2, at
  the cost of one 4.1.0 match. With the homebrew patch the Allegrex has no cost or pipeline model of its own (its
  `cpu` attribute is outside the scheduler's list, so the generic model applies, `BRANCH_COST` 1,
  `MEMORY_MOVE_COST` 4). What `-mtune=r4000` and `r6000` share is `BRANCH_COST` 2 and `MEMORY_MOVE_COST` 6; their
  other differences (multiply/divide/FP costs and pipeline entries) change no sample result. Sony's own build therefore most likely gave the Allegrex its own
  tuning with a branch cost of 2.
- What still separates the best candidate from LBP is instruction scheduling after reload: the typical remaining
  difference in the 3.3.3-only set is a two-instruction swap in the epilogue (`li v0,1; lw ra` in LBP, `lw ra;
  li v0,1` from GCC 4.x, where the generic model gives loads a latency of 3). `-fno-schedule-insns2` fixes those
  functions but breaks many others, and `-mtune=r4130`/`vr5400`/`sb1` (other latency models) fix a different subset
  each. So Sony's 4.x had its own Allegrex pipeline description; no open-source build has it.
- Of the 159 3.3.3-only functions, 101 are EXACT under at least one 4.x candidate; their neighbours in address order
  are mostly 4.x matches. They look like the same compiler, not separate 3.3.3 objects.

**Reconstructing Sony's Allegrex scheduling model (experiment, patched GCC).** The homebrew patch never adds
`allegrex` to the `cpu` attribute of `mips.md`, so no tuning reaches the Allegrex. In a copy of the 4.0.2h tree
(`lbp-gcc4/exp/`, `build_variant.sh`), `allegrex` was appended to the attribute, the Allegrex
given the r4000 `BRANCH_COST`/`MEMORY_MOVE_COST`, and one scheduling description per hypothesis included before
`generic.md` (only `cc1`/`cc1plus` rebuilt). Sample results (`exp/variants*.log`):

| Hypothesis (loads; stores) | EXACT | words left | `gcc171` | `gcc410` | `miss` |
| --- | --- | --- | --- | --- | --- |
| B0 no change (control: equals `gcc402t-Os`) | 196 | 3,434 | 46 | 149 | 1 |
| A latency 1; generic | 169 | 3,734 | 47 | 122 | 0 |
| B latency 2; generic | 190 | 3,461 | 47 | 143 | 0 |
| E latency 3; store holds a memory port 2 cycles | 251 | 3,124 | 92 | 149 | 10 |
| **F latency 3; store holds a memory port 3 cycles** | **290** | **2,987** | **128** | **149** | **13** |
| J / K latency 3; store 4 / 5 cycles | 279 / 278 | 3,026 / 3,036 | 119 | 149 / 148 | 11 |
| M latency 2; store 3 cycles | 281 | 3,029 | 126 | 143 | 12 |
| G / H / I (loads also hold the port; latency 4; latency 2 + port) | 236 / 249 / 230 | 3,127-3,231 | 87-93 | 134-147 | 8-9 |

Hypothesis F, installed as `~/lbp-gcc4/psp-gcc-4.0.2x` (profile `gcc402x-*`, experimental, provenance and diffs in
`PROVENANCE.json`), is EXACT on 1,815 of the 1,899 GCC sources in `src/` (`gcc402t-Os` 1,728, `gcc410-Os` 1,697) and
makes zlib's `updatewindow` EXACT, which no unpatched build does (the prologue interleaves `sw sN` with the argument
moves into `sN`, exactly what a busy store port produces). The remaining differences in the 3.3.3-only set are mostly
not scheduling (operand order, FP instruction placement, `movz`/`movn` selection).

**Primary profile: `gcc404x` (owner decision 2026-10-27, superseding `gcc402x` the same day).** GCC 4.0.4 + the
pspdev 2007 patch revision with the same three measured Allegrex changes (`~/lbp-gcc4/psp-gcc-4.0.4x`, built by
`exp/build_x.sh`, diffs `exp/gcc-4.0.4x-mips.{md,h}.diff`; hashes in `PROVENANCE.json`, which also records
`primary_profile`). Why: strictly better on the fixed sample (291 EXACT / 2,942 words left vs 290 / 2,987, all 150
gcc410 matches kept), and it reproduces LBP's C++ static-init argument-test order (`__initialize_p` before
`__priority`), which GCC 4.0.2 reverses: LBP's compiler is 4.0.3 or later. `tools/lbpmatch.py` `PRIMARY_PROFILE =
"gcc404x-Os"` is the default of headers without `cc=`, of `lbpgen.py --cc` and of `lbpcc.py`. `gcc402x-*`,
`gcc402t-*`, `gcc410-*` and `gcc171-*` stay secondary profiles; existing sources keep their own `cc=` (a full rebuild
after the switch stayed EXACT 3,862/3,862).

Earlier decision (same day, superseded): **`gcc402x` is LittleBigPlanet's PRIMARY compiler profile.** `gcc402t-*`, `gcc410-*`
and `gcc171-*` stay secondary profiles for the functions only they match. `tools/lbpmatch.py` `PRIMARY_PROFILE =
"gcc402x-Os"` is the default of headers without `cc=`, of `lbpgen.py --cc` and of `lbpcc.py`. Compiler binaries
`~/lbp-gcc4/psp-gcc-4.0.2x` (WSL); provenance, diffs and hashes: `lbp-gcc4/PROVENANCE.json`
and `exp/gcc-4.0.2x-mips.{md,h}.diff`, `exp/F-load3-st3.md`.

**Recommendation as submitted (before the decision).**
1. Primary GCC 4.x profile: `gcc402x-*` if the owner accepts a patched open-source GCC whose only changes are a
   tuning description chosen by measurement (three small diffs to the MIPS backend; no Sony code). It is the closest
   match found by a wide margin.
2. If only unmodified builds are acceptable: `gcc402t-*` (GCC 4.0.2 + pspdev's 2007 patch revision,
   `-mpreferred-stack-boundary=4 -mtune=r4000`; a stock tuning flag).
3. Either way keep `gcc410-*` and `gcc171-*` as secondary profiles for the functions only they match.
Neither is Sony's build; the evidence (4.0.x series, 16-byte stack, branch cost 2, a 3-cycle store port) narrows what
Sony's Allegrex backend did, and more hypotheses (multiply/divide and VFPU latencies, `movz`/`movn` costs) can be
tested the same way.

**Sony's own psp-gcc 4.x (not fetched).** It shipped only inside Sony's licensed PSP SDK (SCE developer network, SDK
releases contemporary with LBP's SDK 5.05). The decomp.me PSP compilers the owner accepted are Sony's 3.3.3 kits
(1.3.1, 1.7.1) and SNC; no lawfully available copy of a Sony GCC 4.x kit is known here. Any copy would come from a
leaked SDK, which needs the desk's decision before anyone looks for it.

## Reference fingerprints (measured 2026-10-26)

`python tools/lbptoolchain.py profile --elf <module> --functions <list> [--only <starts>]` on hst-decomp's matched
code (labels from its source headers) and `lbptoolchain.py object` on compiled probes:

| Family | move encoding | `li 0` | branch-likely per branch | `sw ra` is first prologue store | movz/movn per 1000 words | `.comment` |
| --- | --- | --- | --- | --- | --- | --- |
| mwccpsp 3.0.1 (Metrowerks; HST game code, 5,930 fns) | `addu` | `addu` from `$zero` | 0.38 | 0.97 | 0 | `MW MIPS C Compiler (2.4.1.01)` |
| psp-gcc 3.3.3 (Sony SDK in HST, 727 fns) | `addu` | `addu` from `$zero` | 0.12 | 0.55 | 4.2 | (none of its own) |
| pspsnc 1.2.7503 (SN Systems SNC, probe) | `or` | `ori rt,zero,0` | 0 (probe) | no (s-regs first) | 0 (probe) | to be read from a probe object |

Other known facts carried over from hst-decomp:

- mwccpsp emits every function in its own `.text` section; game objects are `-O4,p -sdatathreshold 0` (no
  gp-relative data); strength reduction, inline depth and loop unrolling vary per object (pragmas in the source).
- The Sony SDK objects are psp-gcc output; part of the SDK math library uses Allegrex `ins rd,$zero,pos,size`,
  which neither decomp.me psp-gcc kit (1.3.1, 1.7.1) emits: such functions are blocked until a later kit exists.
- newlib is the SDK C library (HST: closest 1.13.0, GCC -O2).

## ins from $zero (fourth Allegrex change, adopted 2026-10-28)

Sony's compiler clears a contiguous run of bits with `ins rX,$zero,pos,n`, not with a loaded mask and `and`. Retail game
code (0x08804000-0x08ad0000) has 279 low-run (`x & ~7`), 391 middle-run and 156 high-run `ins rX,$zero`, and only 4
masks of that shape that andi cannot encode loaded as a constant for `and` (bit-field stores of zero also give `ins`).
GCC 4.0.4 with the pspdev patch uses `ins` only for bit-field stores. The change (`exp/build_ins.sh` in
`lbp-gcc4/`, diffs `exp/gcc-4.0.4z-*.diff`): the `and<mode>3` expander accepts an SImode mask
that clears one contiguous run and that andi cannot encode, and a new insn `*andsi3_allegrex_clear` emits it as
`ins rd,$0,pos,len` with rd tied to the source (a `move` first when the source stays live). Measured with the
`gcc404xs-Os` flags:

| Build | Fixed sample EXACT | Words left | 4.0.4x sources EXACT |
| --- | --- | --- | --- |
| three changes (`psp-gcc-4.0.4x3`, primary until 2026-10-28) | 291 | 2,942 | 1,172 / 1,172 |
| + low runs only (`psp-gcc-4.0.4y`, experiment) | 293 | 2,844 | 1,172 / 1,172 |
| + any contiguous run (`psp-gcc-4.0.4z`, ADOPTED as `psp-gcc-4.0.4x`) | 295 | 2,827 | 1,172 / 1,172 |

Owner decision 2026-10-28: adopt if all sources stay EXACT and the sample improves; both held, so the any-run build is
now `~/lbp-gcc4/psp-gcc-4.0.4x` (profile names unchanged; provenance in PROVENANCE.json). zlib `inflateSync`
0x08ad59dc is EXACT from the stock zlib 1.2.3 source (`state->bits -= state->bits & 7`), 9 words off before.
Results: `try/cceval/results/gcc404{x,y,z}s-Os-ins.json`.

## Unaligned word access (assembler option `-mulsw`, adopted 2026-10-29)

Retail game code has unaligned word loads and stores (`lwl`/`lwr`, `swl`/`swr`: packed struct members, `memcpy` of
unaligned blocks, libgcc's unwinder): 288 functions without a source contain them. GCC 4.0.4 emits them, but the
Sony kit assembler (binutils 2.14+allegrex-2.2.1-psp-1.3.1, behind the `sony-as` wrapper) rejected every one with
"opcode not supported on this processor: allegrex (mips2)". The opcode table is not the cause (`lwl` has the same
ISA I membership as `lw`). In `mips_ip`, a flag rejects exactly these four mnemonics when the CPU is `allegrex` (and
`allegrex31`). The assembler's own option pair `-mulsw` / `-mno-ulsw` (not in `--help`) clears or sets the flag,
and `-mno-ulsw` is the default. The `allegrex2`/`allegrex3` CPUs accept the four but reject `ext`/`ins`, so no
`-march` value fits LBP's code. With `-mulsw` the assembler emits the four with retail's encodings and keeps every
Allegrex instruction.

The GCC 4 profiles that assemble with `sony-as` (all except `gcc432-*`, which uses the pspdev assembler) now pass
`-Wa,-mulsw`. migrate-toolchain re-proved all 5,490 accepted proofs EXACT under it (no output changed: a source
that held one of the four could not assemble before).

## Findings log

Append one dated line per verified finding, naming the function address and the evidence.
- 2026-10-27: compiler identification on the fixed sample (data/cceval-sample.json): best open-source candidate GCC 4.0.2 + pspdev 65aaa94632 patch, `-Os -mpreferred-stack-boundary=4 -mtune=r4000` (profile `gcc402t-Os`): 196 EXACT / 3,434 words left vs 150 / 3,857 for `gcc410-Os`; 4.2.x worse than 4.1.x worse than 4.0.x; e.g. 0x0887cb4c still differs only by the epilogue order `sltu v0; lw ra`.
- 2026-10-27: zlib 1.2.3 `updatewindow` 0x08ad3c54 is the same size as retail only under `-mtune=r4000` costs and EXACT only with the store-port scheduling hypothesis (gcc402x-Os): the minzlib in LBP was built by the game's GCC.
- 2026-10-27: zlib `inflate` 0x08ad3dac (5,948 bytes) is the stock function except that `order[19]` is a non-static, non-const local array whose 19 halfwords are stored one by one (retail prologue); with that edit the frame size matches, but GCC 4.0.2 still copies the initializer as a block.
- 2026-10-27: minzlib differs from stock 1.2.3: inflateEnd 0x08ad54e8 frees only the window (EXACT without ZFREE of the state); inflateInit2_ 0x08ad58c4 keeps the 9,520-byte inflate_state in its own stack frame (addiu sp,-0x2530; strm->state = sp), so it is a reworked one-shot routine, not the stock initializer.
- 2026-10-27: the gcc402x changes on GCC 4.0.3 / 4.0.4 (`gcc404x-*`, `exp/build_x.sh`): 291 EXACT / 2,942 words left on the sample (gcc402x 290 / 2,987; all 150 gcc410 matches kept), identical for 4.0.3 and 4.0.4. GCC 4.0.2's C++ front end tests `__priority` before `__initialize_p` in `__static_initialization_and_destruction_0`; LBP (84-member clone family 0x089f8e28) tests `__initialize_p` first, as 4.0.3+ and 4.1 do. So LBP's compiler is 4.0.3 or later: recommend moving the primary profile from gcc402x to gcc404x (same patch, later point release).
- 2026-10-27: branch cost 1 with the r4000 memory cost (exp/build_bc1.sh) is worse (252 vs 290): keep branch cost 2. MOVE_RATIO 32 (exp/build_moveratio.sh) is neutral on the sample and makes zlib `inflate`'s `order[]` stores element-wise like retail (first 49 words EXACT): a candidate refinement, not adopted.
- 2026-10-27 (gcc404x primary): LBP's C++ static-initialisation functions come in two layouts. Some translation units are `-O2` (block reordering: the init path is out of line, `static_init_089f8e28.cpp`); most are `-Os` (init path falls through, `static_init_08804948.cpp`), but retail inlines the holders' constructor and destructor, which stock GCC 4.0.4 `-Os` does not (its `-Os` lowers `max-inline-insns-single`/`-auto` to 5). The matched sources mark them `always_inline`. Experiment (not a profile): `-Os --param max-inline-insns-single=50` (or `-finline-limit=100`) makes the plain source EXACT too, and gets libstdc++'s placement `operator new` inlined in `_Rb_tree::_M_insert` (0x08805418, 15 words left, an earlier load of the allocator global). So Sony's `-Os` may keep larger inline limits, or LBP built with an inline-limit option: a candidate profile refinement for the owner, measured on a sample before adoption. zlib `inflate` with gcc404x-Os and `order[]` written element by element: first 49 words EXACT (as with MOVE_RATIO 32), then register allocation differs (a2/v0) and the size is 2 words over. `inflate_fast` with gcc404x-Os (strings as symbols): 339/343 words, prologue saves and two stack spills (`wsize`, `whave`) differ. The 13 unmatched newlib functions stay unmatched at gcc404x-O2 (lbpnewlib tag n6).
- 2026-10-27 (inline-limit measurement, PROPOSED, not adopted): on the fixed sample `-Os --param max-inline-insns-single=50` and `-Os -finline-limit=100` (gcc404x driver) give 291 EXACT / 2,942 words left, identical to gcc404x-Os (the sample is C, where the inline limit rarely matters). On LBP's C++ code it decides: the 186 matched `-Os` static-init sources with `always_inline` removed are EXACT 186/186 under either option and 0/186 under stock gcc404x-Os; the 613 other gcc404x-Os sources stay EXACT 613/613 under the option (no regression). Proposal for the owner: a profile `gcc404x-Os` + `--param max-inline-insns-single=50` (Sony's `-Os` apparently does not lower the inline limit to 5 as GNU 4.0 does), after which the `always_inline` markers can be dropped from the sources. The exact threshold is not pinned down (any value that inlines the holder ctor/dtor works here); `_Rb_tree::_M_insert` 0x08805418 stays 15 words off either way. Logs: try/inl-*.log, try/cceval/results/gcc404x-Os-{mis50,il100}.json.
- 2026-10-27 (inline-limit profile ADOPTED, owner decision): named profile `gcc404xi-Os` in `tools/lbpmatch.py` = gcc404x-Os + `--param max-inline-insns-single=50` (GNU 4.0's `-Os` lowers `max-inline-insns-single`/`-auto` to 5 in `opts.c`; Sony's `-Os` evidently does not). Re-measured with the profile's exact flags: fixed sample 291 EXACT / 2,942 words left (`try/cceval/results/gcc404xi-Os.json`), as gcc404x-Os. All 799 gcc404x-Os sources moved to `cc=gcc404xi-Os`; the `always_inline` markers of the 186 `-Os` static-init sources (holder ctor/dtor, and the abort-calling destructor in `static_init_08804508.cpp`) removed; `lbpmatch.py --all` = EXACT 4,762/4,762.
- 2026-10-27 (Inline limit and exceptions: PRIMARY `gcc404xs-*`). New evidence after the adoption of `gcc404xi-Os` separates the inline-limit hypotheses. GCC 4.0.4's `opts.c` lowers both `max-inline-insns-single` and `max-inline-insns-auto` to 5 under `-Os` (defaults 450 / 90); `--param max-inline-insns-single=50` raises only the first, `-finline-limit=100` both to 50. Measured on new near misses (gcc404x driver, `-fno-exceptions`): `std::_Rb_tree::_M_erase` 0x08805b6c (recursion inlined 8 levels via `max-inline-recursive-depth-auto`, a non-inline template member) is EXACT with auto >= 50 and not with single=50 alone; `_Rb_tree::erase(const key&)` 0x088107a0 (equal_range inlined) is EXACT with `-finline-limit=150`, `=200` or the defaults 450/90, and not with `=100` or `=120`; the ref-counted-pointer lookup 0x0893ddec needs single > 30. So Sony's `-Os` keeps GCC's default inline limits (it does not apply the `optimize_size` lowering). Second finding: `.eh_frame` holds FDEs only for 0x08b88058-0x08b9c99c (libsupc++/libstdc++), none for game code, so the game's C++ was built with `-fno-exceptions`; with exceptions on, any C++ source whose temporaries have destructors gets landing pads (`_Unwind_Resume`, e.g. 0x0893ddec). Profile `gcc404xs-<O>` = `gcc404x-<O>` + `-fno-exceptions`, and at `-Os` also `--param max-inline-insns-single=450 --param max-inline-insns-auto=90` (no effect on C). Evidence for making it primary: on the fixed sample, per function identical words left to gcc404x-Os, gcc404xi-Os and `-finline-limit=100 -fno-exceptions` (291 EXACT / 2,942; `try/cceval/results/gcc404x-Os.json`, `gcc404xi-Os.json`, `gcc404x-Os-il100-noeh.json`, `gcc404x-Os-dflinl-noeh.json`); all 799 gcc404xi-Os sources EXACT under it and under `-finline-limit=100 -fno-exceptions`; the 101 C++ `gcc404x-O2` sources EXACT with `-fno-exceptions`. Those 900 sources moved to `gcc404xs-Os`/`gcc404xs-O2`; `gcc404xi-Os` stays defined (the owner's original refinement). Still open: `_Rb_tree::_M_insert` 0x08805418 is 13 words off under every variant (retail loads the allocator's heap global after computing insert_left; ours hoists it to the entry during sched1 interblock).
- 2026-10-27 ~19:30Z (owner decision): `gcc404xs-*` is LBP's PRIMARY profile (gcc404x + `-fno-exceptions` + GCC's default inline limits at `-Os`); `gcc404xi-Os` stays defined as the fallback; `gcc402x-*`, `gcc402t-*`, `gcc410-*` and `gcc171-*` stay secondary. Same decision: lbpmatch places GCC 4.0's vague-linkage data sections (`.gnu.linkonce.{b,sb,d,s,r}.<symbol>`: the local static of an inline function, a template's static data member) at the symbol's address from `symbols/<module>.txt`. Such objects are weak, and a weak object's load may trap, so the scheduler does not move it speculatively: this is what keeps the heap-pointer load of `_Rb_tree::_M_insert` (0x08805418) below the insert_left branches in retail.
- 2026-10-28 (ins from $zero, ADOPTED): the fourth Allegrex change in the 4.0.4x build (an `and` with a contiguous clear mask that andi cannot encode -> `ins rd,$zero,pos,len`): fixed sample 295 EXACT / 2,827 words left (291 / 2,942 before; low-run-only variant 293 / 2,844); all 1,172 sources built by 4.0.4x EXACT under it; zlib `inflateSync` 0x08ad59dc EXACT from the stock source. Section "ins from $zero".
- 2026-10-29 (unaligned word access, ADOPTED): the Sony kit assembler rejects lwl/lwr/swl/swr on `-march=allegrex` unless given its own `-mulsw` option (default `-mno-ulsw`); GCC 4 profiles now pass `-Wa,-mulsw`; migrate-toolchain re-proved all 5,490 proofs EXACT. First lanes on the 288 affected functions: lbp10a 14 EXACT of 21 attempted (stream/chunk writers via `__builtin_memcpy(p, &v, 4)`, packed-struct assignment), lbp10c 6 of 15 (libgcc `read_encoded_value` 0x08b8d144 from the stock packed union). Open: retail block copies with a runtime `(src|dst)&3` dispatch (0x08b1c914, 0x08893d40, 0x08b47140) that GCC 4.0.4 never emits. Section "Unaligned word access".
- 2026-11-04 (lbp-decomp lane, OPEN, needs a measured toolchain decision): retail has no `mtc1` hazard. It puts `mtc1 zero,$fN` in a `jr ra` or branch delay slot (`return 0.0f;` 0x08a09cf0-0x08a09d10 is `jr ra; mtc1 zero,$f0`, 2 words; 0x08a090f8 `beqz; mtc1`) and compares immediately after it (`mtc1 zero,$f0; c.lt.s $f0,$f1` in 0x088b68a4, 0x08a145ac, 0x08a14a18). gcc404xs inserts a nop or reorders in every one of these, so `return 0.0f;` is 3 words. Same class as 0x088c4f14 and 0x088fb3e8 (lbp4a/lbp6b). The Allegrex FPU interlocks `mtc1`; a fifth Allegrex change (no `mtc1`/`mfc1` delay) is the candidate, to be measured on the fixed sample.
- 2026-11-04 (lbp-decomp lane, OPEN): in 0x088c58b0 and 0x088e99e0 (3 words each, everything else EXACT) retail schedules `mov.s $f12,$f20` before the `lui`/`lw` of the pointer argument of the same call (`jal 0x08960ad0` with the `lw` in the delay slot); gcc404xs puts the load first and `mov.s` in the slot. Not changed by argument order, `int` padding or gcc402x/gcc402t/gcc410. Probably the FP-move latency of the scheduling model.
- 2026-11-04 (lbp-decomp lane, OPEN, first inline-VFPU trial): retail puts VFPU instructions in delay slots (0x08a10d50 is `jr ra; lv.q C000.q,16(a0)`, 2 words; 0x089ecf0c `lui; jr ra; lv.q`). GCC never fills a delay slot with an `asm` statement, so `__asm__("lv.q C000, %0" : : "m"(...))` builds `lv.q; jr ra; nop` at gcc404xs-O2 and -Os (volatile or not, `"m"` or `"r"`). Either the original compiler knew VFPU instructions as real insns, or its reorg accepted single-instruction asm. Until the kit does one of these, functions that end in a VFPU delay slot cannot be EXACT with inline asm.
