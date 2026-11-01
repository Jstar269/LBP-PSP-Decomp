#!/usr/bin/env python3
"""Translation units of the game program (EBOOT.BIN), recovered from its static constructors.

    python tools/lbptu.py [--check]

Writes data/translation-units.json: the game's code cut into numbered units at the exact ends the program itself marks.
tools/lbpreport.py groups the progress report by them (bytematch.dev's grid). Needs your decrypted EBOOT.BIN (the
campaign the other tools read); `--check` compares the file with a fresh recovery and writes nothing.

EVIDENCE. GCC 4.0.4 ends a translation unit that has static initialisers with three functions, in this order:
`__static_initialization_and_destruction_0`, `_GLOBAL__D_<unit>` (only when the unit has a destructor to register) and
`_GLOBAL__I_<unit>`, and it lists `_GLOBAL__I_<unit>` in .ctors (probe: a test file built with the locked
`gcc404xs-Os` profile). ld concatenates the .ctors sections in link order, the same order as the code, so each .ctors
entry in the game region is the last function of one unit, exactly; the program's own table gives every end, no
heuristic does. Between two consecutive ends lies one interval. A unit without static initialisers has no marker, so
an interval holds one unit or several, never part of one: it is an upper bound of the units, and the tool never
merges across an end. The game region's own limits (data/origin-regions.json) close the first and the last interval.
The units have no file names (the program keeps almost none), so each is named by its number and its first function:
`tu-<index>@<first address>`, with the evidence for both of its ends (tools/lbpreport.py read_units
reads the file).

Class membership (vtables and the constructors that store them) is not used to cut an interval further: a class is
not a translation unit (one file often defines several classes, and a header's inline virtuals are emitted in the
units that use them), so a cut between two classes would claim an end the program does not mark.

Only the table's leading ascending run is in link order: ld's default script appends the sorted `.ctors.<priority>`
input sections after it, so the entries that follow (two in the game region) mark no place in the link order and are
left out; the interval holding each is still an upper bound.

CHECK (the tool refuses to write when it fails): every .ctors entry it uses is a function start of
data/functions-EBOOT.BIN.txt (a missing start is a boundary bug: tools/lbpfuncs.py seeds them).
"""
from __future__ import annotations

import argparse
import bisect
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbporigin  # noqa: E402

ROOT = lbporigin.ROOT
MODULE = "EBOOT.BIN"
TU_REL = "data/translation-units.json"
TU_PATH = ROOT / TU_REL
FUNCTIONS_REL = f"data/functions-{MODULE}.txt"
REGION = "game"
ABOUT = ("Translation units of the game program recovered by tools/lbptu.py: each unit's first and last function "
         "start, its function count, and the evidence for its two ends (ctors: the unit's static constructor, its "
         "last function, listed in .ctors; region: the limit of the game region). A unit may hold several of the "
         "original files (one without static initialisers has no marker), never part of one. See that tool's "
         "docstring.")


class Refused(Exception):
    pass


def read_functions(path: Path) -> list[tuple[int, int]]:
    out = []
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if line:
            s, e = line.split()[:2]
            out.append((int(s, 16), int(e, 16)))
    return sorted(out)


def ctors(data: bytes) -> list[int]:
    """The .ctors entries of the ELF image, in table order (without the -1 and 0 terminators)."""
    import lbpmodinfo as MI
    sec = MI.Elf(data).section(".ctors")
    if sec is None:
        raise Refused("the image has no .ctors section")
    words = struct.unpack_from(f"<{sec['size'] // 4}I", data, sec["offset"])
    return [a for a in words if a not in (0, 0xFFFFFFFF)]


def linked(entries: list[int]) -> list[int]:
    """The entries in link order: the table's leading ascending run. ld's default script puts the sorted
    `.ctors.<priority>` input sections after the plain ones, so entries after the run come from another input section
    and their place says nothing of the link order."""
    n = next((k for k in range(1, len(entries)) if entries[k] <= entries[k - 1]), len(entries))
    return entries[:n]


def recover(functions: list[tuple[int, int]], entries: list[int], region: tuple[int, int]) -> list[dict]:
    """The units of the game region: [{first, last, functions, start, end}] in address order (first and last are
    function starts). Raises Refused when the evidence disagrees with the function list."""
    starts = [s for s, _e in functions]
    known = set(starts)
    lo, hi = region
    game = [s for s in starts if lo <= s < hi]
    if not game:
        raise Refused("no function in the game region")
    mine = [a for a in linked(entries) if lo <= a < hi]
    missing = [a for a in mine if a not in known]
    if missing:
        raise Refused("static constructors inside a listed function (a missing start): "
                      + ", ".join(f"0x{a:08x}" for a in missing))
    units, first, start_kind = [], 0, "region"
    for end in mine + [None]:
        last = len(game) - 1 if end is None else bisect.bisect_left(game, end)
        if last < first:
            continue  # the region's last function is itself a constructor: nothing after it
        units.append({"first": game[first], "last": game[last], "functions": last - first + 1,
                      "start": start_kind, "end": "region" if end is None else "ctors"})
        first, start_kind = last + 1, "ctors"
    return units


def name(index: int, unit: dict) -> str:
    return f"tu-{index:03d}@{unit['first']:08x}"


def document(units: list[dict]) -> dict:
    return {"about": ABOUT, "modules": {MODULE: [
        {"name": name(i, u), "first": f"0x{u['first']:08x}", "last": f"0x{u['last']:08x}",
         "functions": u["functions"], "start": u["start"], "end": u["end"]} for i, u in enumerate(units, 1)]}}


def render(doc: dict) -> str:
    lines = ["{", f' "about": {json.dumps(doc["about"])},', ' "modules": {']
    mods = list(doc["modules"].items())
    for i, (mod, us) in enumerate(mods):
        lines.append(f"  {json.dumps(mod)}: [")
        lines.extend(f"   {json.dumps(u, separators=(',', ':'))}" + ("," if j < len(us) - 1 else "")
                     for j, u in enumerate(us))
        lines.append("  ]" + ("," if i < len(mods) - 1 else ""))
    lines += [" }", "}"]
    return "\n".join(lines) + "\n"


def game_region(root: Path = ROOT) -> tuple[int, int]:
    regions = lbporigin.load_regions(root / "data" / "origin-regions.json").get(MODULE, [])
    spans = [(s, e) for s, e, c in regions if c == REGION]
    if len(spans) != 1:
        raise Refused(f"data/origin-regions.json has {len(spans)} {REGION} regions for {MODULE}, expected one")
    return spans[0]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true", help="compare the file with a fresh recovery; write nothing")
    args = ap.parse_args()
    import lbpproof
    try:
        units = recover(read_functions(ROOT / FUNCTIONS_REL), ctors(lbpproof.module_image(MODULE)), game_region())
    except Refused as exc:
        print(f"refused: {exc}")
        return 1
    sizes = sorted(u["functions"] for u in units)
    image_entries = ctors(lbpproof.module_image(MODULE))
    print(f".ctors: {len(image_entries)} entries, {len(linked(image_entries))} in link order")
    print(f"{MODULE}: {len(units)} units, {sum(sizes)} functions; {sum(1 for u in units if u['end'] == 'ctors')} "
          f"ends from .ctors; functions per unit median {sizes[len(sizes) // 2]}, largest {sizes[-1]}")
    text = render(document(units))
    if args.check:
        same = TU_PATH.exists() and TU_PATH.read_text(encoding="utf-8") == text
        print(f"{TU_REL}: " + ("agrees" if same else "DIFFERS (run tools/lbptu.py)"))
        return 0 if same else 1
    TU_PATH.write_text(text, encoding="utf-8", newline="\n")
    print(f"wrote {TU_REL}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
