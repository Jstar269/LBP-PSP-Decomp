# PSP relocation tables

A PSP executable (`EBOOT.BIN`, the Sharing Version's `hstp_trans_main.prx`) is a relocatable module: the firmware loads
it at an address it chooses and then patches every word that holds an address. The list of those words is the
relocation table. This page describes the format, the order the original link left the entries in, and how
`tools/hstbuild.py` regenerates the tables and uses them for the shifted build (gate 5 of hst-decomp's [COMPLETION.md](https://github.com/Jstar269/hst-decomp/blob/main/docs/COMPLETION.md)).
The code is `tools/hstreloc.py`; its tests are in `tools/test_hstbuild.py`.

## Format A (section type and program header type `0x700000A0`)

The two game modules keep one table per relocated section (`.rel.text`, `.rel.lib.ent`, `.rel.lib.stub`,
`.rel.rodata.sceModuleInfo`, `.rel.rodata.sceResident`, `.rel.data`; `.rel.sceStub.text` is empty). Each entry is
8 bytes, little-endian:

| Field | Bits | Meaning |
| --- | --- | --- |
| `r_offset` | 32 | the site: the relocated word, relative to the base of the offset segment |
| `r_info` | 0-7 | the type: `R_MIPS_32` (2), `R_MIPS_26` (4), `R_MIPS_HI16` (5), `R_MIPS_LO16` (6) |
| | 8-15 | the offset segment: index of the `PT_LOAD` the site lies in |
| | 16-23 | the address segment: index of the `PT_LOAD` whose load base is added to the target |
| | 24-31 | zero |

The layout was checked on every entry of both game modules (`hstreloc.decode_a` refuses a table that breaks it): bits
24-31 are always zero, both segment fields are 0 (the modules have one `PT_LOAD`, linked at address 0), and only the
four types above occur. Nakagawa Recomp's project-authored loader (`src/rt/prx_loader.c`, `apply_format_a`)
applies the same layout; it was read for the format only.

How the loader moves a word by the load base S of the address segment:

- `R_MIPS_32`: word + S.
- `R_MIPS_26`: the jump target `(site & 0xf0000000) | (field << 2)` + S, written back into the 26-bit field.
- `R_MIPS_HI16`: no addend of its own. The loader takes the run of HI16 entries and the next entry that is not HI16
  (its partner, always a `R_MIPS_LO16`), forms `target = (immediate << 16) + sext16(partner's immediate)` and writes
  the carry-adjusted high half of `target + S`. A table must therefore keep every run of HI16 entries immediately in
  front of the LO16 it belongs to.
- `R_MIPS_LO16`: low half of `sext16(immediate) + S`.

## Format B (program header type `0x700000A1`)

The four Sony library modules have no section headers and two `PT_LOAD` segments. Their table is a compressed command
stream: a header (two zero bytes, the flag-index and kind-index widths), a flag table and a kind table, then 16-bit
commands (flag index, address segment, kind index, signed displacement), some with 16- or 32-bit extensions. A flag
with bit 0 clear sets the current offset segment and offset; with bit 0 set it relocates one word, bits 1-2 choosing
the offset form and bits 3-5 the HI16 addend form (none, the previous HI16's, a 16-bit value). Words are relative to
their address segment: a word holds `target - (link address of that segment)`. `hstreloc.decode_b` decodes it; the
build does not regenerate it yet.

## The order of the entries

### `.rel.text`

Measured on both game modules (EBOOT: 109,130 entries, of which 29,362 HI16; Sharing: 119,273, 33,776 HI16):

1. **One block per function, the functions in address (link) order.** Leaving the HI16 entries aside, every function's
   entries are contiguous and the blocks follow each other in ascending address order: 0 functions split, 0 blocks out
   of order (7,168 functions with relocations in EBOOT, 7,213 in Sharing). This is the original linker copying each
   object's relocation list in link order.
2. **Inside a block, the compiler's emission order.** It is close to address order but not equal: 6,363 (EBOOT) of
   the 79,768 non-HI16 neighbours are inverted. 5,312 of the inversions are a word in a branch delay slot listed
   just before its branch or jump: Metrowerks emits relocations in its instruction order from before the delay slots
   are filled. The remaining 1,051 come from later code motion (an instruction the scheduler moved past others keeps
   its earlier place in the list). A simple rule (address order, a delay-slot word first) predicts the whole order
   of 6,514 of the 7,172 blocks; the rest is the compiler's own order and is known only from the compiler. The
   whole-module build compares every compiled source's object with its retail block: all 3,774 Metrowerks blocks of
   the public EBOOT build are reproduced by their objects' own order.
3. **A HI16 sits immediately in front of its partner LO16**, in the partner's block. 29,312 of the 29,362 HI16 entries
   are directly followed by their LO16. The other 50 are the exceptions of the observed rule: they form 38 runs
   (33 of two entries, 4 of three, 1 of ten) in which several HI16 share one LO16. In 35 runs every `lui` is the
   same instruction (same register, same immediate) and that register is the LO16's base: two or more paths load
   the same high half and meet at one use, so each of those `lui` needs the same partner. The remaining runs, and
   all 154 HI16 entries (EBOOT and Sharing alike) that sit in front of a LO16 of another function, are in functions
   the compiler-family predictor (`tools/hstfamily.py`) places in `psp-gcc` code (the Sony SDK, p(Metrowerks) below
   0.15). That fits the GNU assembler's `mips_frob_file` of the binutils generation of the Sony kit: it takes the
   HI16 relocations that are not followed by their own LO16 from the last to the first and moves each in front of the
   first LO16 of the section with the same symbol and offset that no other HI16 has claimed yet (a second pass allows
   several HI16 per LO16, giving runs), so pairs of identical `lui` in different functions of one object exchange
   partners. A simulation of that algorithm (target address standing in for symbol and offset, runs of consecutive
   gcc functions standing in for objects) reproduces the whole `.rel.text` order of 61 of the 84 gcc regions of EBOOT
   and 2,334 of their 2,458 HI16 partners; the rest is not explained yet (object boundaries and the symbol stand-in
   are the likely gaps).

The build treats a HI16 as part of its partner's block, so the table is exactly one block per tile in address order
(`hstreloc.block_owner`, `runs_of`, `is_address_ordered`).

### `.rel.data`

`.rel.data` is not in address order. The original linker emitted the data relocations object by object in link
order, and for each object the relocations of each of its data sections (initialised data, read-only data, exception
tables, the static-initialiser pointer and the code it points to) in the object's order, some of them in descending
address order. The table is therefore a merge of increasing address streams, one per output region, cut at object
boundaries: 14,603 entries in 633 address runs that sort into 7 increasing streams. Reproducing that order by rule
needs to know which object every piece of data belonged to, which the unmatched data does not record. The build
follows the scaffolding's run plan (the sequence of tile blocks in the retail table) for `.rel.data` and counts all of
its entries as scaffolding, also those whose content a compiled object supplies.

## What the whole-module build does with them

- **Relocatable scaffolding.** Every scaffolding word that a retail relocation covers (in unmatched functions and in
  data blobs alike) has its relocated field cleared and gets a `.reloc` of the same type against a target symbol
  `__hst_r_<target>`, defined in `syms.ld` as tile symbol + offset (`__hst_at_<tile start>` is defined by the linker
  script at every tile). The link writes the word, and it moves with the layout. Format-B words are defined relative
  to their address segment. The names a compiled object leaves undefined and the parts of an object that are not
  linked are defined the same way.
- **Generated tables.** The build links with `psp-ld -q` and writes each format-A table from the link's records: a
  compiled source's block from its object's relocation records in the object's order, every other block from the
  scaffolding's records, the sites at their linked addresses. It checks the tables against the relocations `ld -q`
  kept, and against the retail tables. A compiled block whose object order differs from the retail block (for
  example a gcc object whose HI16 sits in another function's block) is taken from the scaffolding and counted.
- **Accounting.** Table entries from compiled objects (8 bytes each, `.rel.text` only, because its order is the
  address rule) count for their source in `t2_source_bytes`; all other entries are
  `scaffolding.relocation_entry_bytes`.
- **The container.** ELF header, program header, section header table, `.shstrtab` and the placement of every part
  are generated from the build's sections by the module's container profile: EBOOT puts the load content right after
  the program header, then the section header table, the relocation tables, `.shstrtab` and `.comment` (file padded to
  4), with `sh_link` 3 and `sh_info` = target index + 2 in its relocation sections; Sharing aligns the load content to
  16 and orders `.comment`, `.shstrtab`, the section header table, the relocation tables, with `sh_link` 0 and
  `sh_info` = target index. `.comment` (the original toolchain's name) is copied and counted in
  `container.bytes_from_retail`.
- **The shifted build** (`--shift N`): the same objects linked with N bytes of padding ahead of the code and N more
  ahead of the data (N a multiple of the module's largest section alignment, 128 for the game modules), compared
  byte for byte with `hstreloc.expected_shift`, which moves the retail image with the retail tables alone (a word by
  the move of its target's region; a standalone LO16's region from the HI16 of its function that loads its base
  register, refusing any ambiguity). Sections, tables (`hstreloc.shifted_table`), the `ld -q` records, the entry point
  and the whole file must all agree for `shifted_build_passes`. The runtime half of gate 5 (the shifted module loads
  and runs a documented scenario) is not run by the build; reports say `shifted_runtime: "not run"`.
