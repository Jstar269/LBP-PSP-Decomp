# Readability lane brief

A readability lane rewrites sources that ALREADY match, so that a person can read them: meaningful names, real
types, structs, named constants, short comments. The bytes must not change: every file you touch must still be
`EXACT`. Your lane is an isolated copy of the repository (its root holds `LANE.json`); the main session takes your
rewrites with `tools/lbplane.py rewrite-harvest` (a maintainer tool, not in the public tree), which re-verifies each one with the main tree's matcher and the
promotion gate and keeps the old text for anything that does not hold. Read `AGENTS.md` and `CONTRIBUTING.md` first;
their integrity rules apply unchanged.

## Your batch

`try/agent-queue/<batch>.json` lists `items`, worst first. Each names `source` (the file in this lane to rewrite),
`address`/`end`, its readability `score` (`python tools/lbpreadable.py <file>`: Ghidra-style names, `undefined`
types, offset casts, the pasted typedef block) and `clones`: how many other sources are byte-identical copies of it
at other addresses. A rewrite of an item with clones is re-derived onto every clone automatically, so those items
matter most and have one extra rule (below).

## Per file

1. Copy the original aside first: `cp src/eboot/<f> try/lane-work/<f>.orig`.
2. Read the file, its disassembly (`python tools/lbpdis.py 0xADDR`) and its callers/callees (grep `src/` for the
   address). Decide what each value is from how the code uses it.
3. Edit the file in place, then `python tools/lbpmatch.py src/eboot/<f>` must print `EXACT`. If an edit breaks it,
   undo that edit (types and declaration order can change code; names never do). About 8 compiles per file; if you
   cannot keep it EXACT, restore the `.orig` copy and record a SKIP.
4. Append one line to `PROGRESS.md` in the lane root:
   `0xADDR8 DONE src/eboot/<file> <score before>-><score after>` or `0xADDR8 SKIP <reason>`.

## What to change

- **Locals and parameters:** names that say what they hold (`count`, `node`, `self`, `buffer`, `index`), from their
  use in this function. Replace `param_1`, `iVar1`, `uVar2`, `local_10`, `auStack_20`.
- **Types:** `undefined4` -> `int`/`unsigned int`/a pointer type as the code uses it; `undefined1` -> `unsigned char`
  or `char` (signedness matters: `lb` vs `lbu`); `code *` -> a function pointer type. Remove the pasted typedef block
  when nothing needs it any more.
- **Structs:** replace `*(int *)(p + 0x14)` with a struct member. Name a field by what it holds when the code shows it
  (`count`, `next`), otherwise by its offset (`unk_14`). Keep the layout exact (padding arrays `char pad[0x10]` where
  needed).
- **Shared headers:** a struct (or enum, or constant) that two or more sources of your batch use goes into a NEW
  header `include/lbp/<topic>.h` (a short topic name: `refptr.h`, `resource.h`), included as
  `#include "lbp/<topic>.h"`. Never edit a header that already exists under `include/lbp/` (create a new one instead);
  a lane that edits one has the sources using it refused. Headers hold declarations only (no code).
- **Constants:** a magic number with a clear meaning becomes an `enum` or `#define` with a name.
- **Comments:** short why-comments where the code is not obvious (one line; no essays). Keep the second-line
  provenance note true and add to it: `; readability pass (lane <lane>, <your model>)` before its closing `*/`.
- **Function names:** keep `FUN_xxxxxxxx` and the header's `symbol=` unless there is evidence for a name (a string
  the function prints, a library API it is, a name in `symbols/`). With evidence, rename it to a name that KEEPS the
  address suffix (`ResourceCache_find_0893ddec`), change `symbol=` in the header to match, and append
  `name_0893ddec = 0x0893ddec;   # <file> (lane <lane>)` to `symbols/EBOOT.BIN.txt` only if another source should
  call it by that name. Callees: you may call a function by a name already in `symbols/`; never invent one.

## Never

- Change what the code does, the header's `module`/`start`/`end`/`cc=`, or add steering tricks (dummy locals,
  `volatile`, `goto`, duplicated conditions) to keep a rewrite EXACT; a rewrite that needs one is not worth it.
- Rename an external symbol in an item that has `clones` (its clones call other addresses): rename only locals,
  parameters, types, struct fields and headers there. Its comments may name only addresses the code itself
  references (a clone's comment gets its own addresses; a sibling the code does not call cannot be mapped).
- Touch files that are not in your batch (except new headers under `include/lbp/` and `symbols/` appends), edit
  `tools/`, run `lbplane.py`/`lbpqueue.py`/`lbpclone.py`/`lbppromote.py`, use git, or send any of this code anywhere.
- Invent semantics: a name is a claim about the program. When unsure, use a plain role name (`value`, `ptr`,
  `unk_14`), not a guess.

## Finish

Re-run `python tools/lbpmatch.py` on every file you changed (all must be `EXACT`; restore any that is not), then write
`REPORT.md` in the lane root: files done / skipped, the score before and after (`python tools/lbpreadable.py
<files>`), new headers, symbol lines added, and anything the main session should know.
