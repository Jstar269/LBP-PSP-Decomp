#!/usr/bin/env python3
"""Name the import stubs of a module from their NIDs, and print the symbol-map lines for them.

    python tools/lbpnids.py --module EBOOT.BIN [--corpus <nid_corpus.json>] [--append]

Each imported function is an 8-byte stub in .sceStub.text (stub table entry i of a library sits at stub_table + 8*i);
a source calls it by its system-library name. Names come from Nakagawa Recomp's NID corpus (`tools/nid_corpus.json`,
read-only): entries of the derivable classes carry no stored NID, it is sha1(name)[0:4] read little-endian; the other
classes store their NID. Unknown NIDs keep an address name. `--append` appends the new lines to symbols/<module>.txt
(append-only; names already present are skipped, and a function imported through two stub entries is named at
its first stub only, since a name binds one address).
"""
from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpconfig as CFG  # noqa: E402
import lbpmatch as M  # noqa: E402
import lbpmodinfo as MI  # noqa: E402


def nid_names(corpus: dict) -> dict[int, str]:
    derivable = set(corpus.get("derivable_classes", []))
    out: dict[int, str] = {}
    for e in corpus.get("entries", []):
        name = e.get("name")
        if not name:
            continue
        if e.get("class") in derivable:
            nid = int.from_bytes(hashlib.sha1(name.encode("ascii")).digest()[:4], "little")
        elif "nid" in e:
            nid = int(e["nid"], 16) if isinstance(e["nid"], str) else int(e["nid"])
        else:
            continue
        out.setdefault(nid, name)
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", required=True)
    ap.add_argument("--corpus", type=Path)
    ap.add_argument("--append", action="store_true")
    args = ap.parse_args()
    corpus_path = args.corpus or (CFG.host_path("nakagawa_root") / "tools" / "nid_corpus.json")
    names = nid_names(json.loads(corpus_path.read_text(encoding="utf-8")))
    data, _seg = M.module_bytes({}, args.module)
    elf = MI.Elf(data)
    mi = MI.module_info(elf)
    existing = M.load_symbols(args.module)
    lines, known, unknown, twice = [], 0, 0, 0
    named: dict[str, int] = {}
    for row in MI.imports(elf, mi, True):
        base = int(row["stub_table"], 16)
        for i, nid in enumerate(row.get("nids", [])):
            addr = base + 8 * i
            name = names.get(int(nid, 16))
            if name is None:
                unknown += 1
                continue
            known += 1
            if name in existing:
                continue
            if named.setdefault(name, addr) != addr:
                # the same function imported through a second stub entry: one name can be bound to one address
                # only (load_symbols refuses a second binding), so the later stub keeps its address name
                twice += 1
                continue
            lines.append(f"{name} = 0x{addr:08x};   # import {row['library']} NID {nid} (lbpnids)")
    print(f"{args.module}: {known} imports named, {unknown} NIDs unknown to the corpus, {len(lines)} new symbol lines"
          + (f", {twice} stubs of an already named import left unnamed" if twice else ""))
    if args.append and lines:
        with (M.ROOT / "symbols" / f"{args.module}.txt").open("a", encoding="utf-8", newline="\n") as stream:
            stream.write("".join(line + "\n" for line in lines))
    elif not args.append:
        print("\n".join(lines[:20]) + ("\n..." if len(lines) > 20 else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
