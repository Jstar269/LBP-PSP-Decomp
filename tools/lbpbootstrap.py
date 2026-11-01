#!/usr/bin/env python3
"""Create the campaign directory the tools read, from your own decrypted copies of the game's modules.

    python tools/lbpbootstrap.py --out <campaign dir> --module EBOOT.BIN=<decrypted EBOOT.BIN> [--module NAME=PATH ...]

Then set `campaign_root` in lbp-config.json (or LBP_CAMPAIGN_ROOT) to <campaign dir>.

The modules this repository reconstructs are described in data/modules.json (see tools/lbpproject.py): where each
one comes from on the disc, the SHA-256 and size of its decrypted form, its load segment and the list of its function
boundaries (data/functions-<module>.txt).

For each module given, the tool checks the file against that SHA-256. A gzip-compressed image is inflated first,
because some decryption tools write the module before decompressing it. Nothing is decrypted here: decrypting your
copy is your own responsibility. On success the tool copies the checked bytes to <campaign dir>/inputs/ and writes
<campaign dir>/ledger/campaign.json (modules, load segments, functions) and
<campaign dir>/inputs/revision-selection.json (where the inputs are), the two files lbpmatch.py and the other tools
read. A module you do not give is left out; the tools then report its sources as unavailable.

Adapted from hst-decomp's tools/hstbootstrap.py (module list from data/modules.json instead of two fixed modules).
"""
from __future__ import annotations

import argparse
import gzip
import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MODULES = ROOT / "data" / "modules.json"


def load_checked(path: Path, want_sha256: str, want_size: int) -> bytes | None:
    """The module bytes when they (or their gzip-inflated form) match the expected digest, else None."""
    data = path.read_bytes()
    candidates = [data]
    if data[:2] == b"\x1f\x8b":
        try:
            candidates.append(gzip.decompress(data))
        except OSError:
            pass
    for c in candidates:
        if len(c) == want_size and hashlib.sha256(c).hexdigest() == want_sha256:
            return c
    return None


def stub_ranges(data: bytes) -> list[tuple[int, int]]:
    """[lo, hi) of the executable's import stub section (.sceStub.text): generated from the import table, never
    matched by a C source (tools/lbpfuncs.py marks the same functions `stub`). [] when the file is not such an ELF."""
    import lbpfuncs
    import lbpmodinfo as MI
    try:
        return [(lo, hi) for lo, hi, name in lbpfuncs.code_ranges(MI.Elf(data)) if name == ".sceStub.text"]
    except Exception:       # noqa: BLE001 - any parse failure: no stubs known, every function stays "auto"
        return []


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", required=True, help="campaign directory to create or update")
    ap.add_argument("--module", action="append", default=[], metavar="NAME=PATH",
                    help="a decrypted module (NAME as in data/modules.json); repeat for each module")
    args = ap.parse_args()
    given: dict[str, str] = {}
    for item in args.module:
        name, sep, path = item.partition("=")
        if not sep or not name or not path:
            ap.error(f"--module wants NAME=PATH, got {item!r}")
        given[name] = path
    if not given:
        ap.error("give at least one --module NAME=PATH")

    spec = json.loads(MODULES.read_text(encoding="utf-8"))
    known = {m["name"] for m in spec["modules"]}
    unknown = sorted(set(given) - known)
    if unknown:
        ap.error(f"unknown module(s) {', '.join(unknown)}; data/modules.json lists {', '.join(sorted(known))}")
    out = Path(args.out).resolve()
    (out / "inputs").mkdir(parents=True, exist_ok=True)
    (out / "ledger").mkdir(parents=True, exist_ok=True)

    modules, failed = [], False
    for mod in spec["modules"]:
        path = given.get(mod["name"])
        if not path:
            print(f"{mod['name']}: not given, left out")
            continue
        data = load_checked(Path(path), mod["sha256"], mod["size"])
        if data is None:
            print(f"{mod['name']}: {path} is not the expected decrypted module "
                  f"(want {mod['size']} bytes, SHA-256 {mod['sha256']}; source on the disc: {mod['origin']})")
            failed = True
            continue
        name = f"{mod['name']}.decrypted"
        (out / "inputs" / name).write_bytes(data)
        functions = []
        ff = ROOT / mod["functions_file"]
        if ff.exists():
            stubs = None
            for line in ff.read_text(encoding="utf-8").splitlines():
                if line and not line.startswith("#"):
                    cols = line.split()
                    start, end = int(cols[0], 16), int(cols[1], 16)
                    if len(cols) > 2:
                        status = cols[2]
                    else:
                        # the published list has no status column: import stubs come from the executable itself
                        stubs = stub_ranges(data) if stubs is None else stubs
                        status = "stub" if any(lo <= start < hi for lo, hi in stubs) else "auto"
                    functions.append({"start": start, "end": end, "boundary_status": status})
        if len(functions) != mod.get("function_count", len(functions)):
            raise SystemExit(f"{mod['functions_file']}: {len(functions)} functions, expected {mod['function_count']}")
        modules.append({"name": mod["name"], "input": {"root": "inputs", "path": name},
                        "input_sha256": mod["sha256"], "load_segments": mod["load_segments"], "functions": functions})
        print(f"{mod['name']}: OK ({len(functions)} functions)")
    if failed:
        return 1

    ledger = {"schema": "lbp-decomp.bootstrap-campaign.v1", "title_id": spec["title_id"], "modules": modules}
    selection = {"schema": "lbp-decomp.bootstrap-selection.v1", "roots": {"inputs": str(out / "inputs")}}
    (out / "ledger" / "campaign.json").write_text(json.dumps(ledger, indent=1) + "\n", encoding="utf-8")
    (out / "inputs" / "revision-selection.json").write_text(json.dumps(selection, indent=1) + "\n", encoding="utf-8")
    print(f"wrote {out / 'ledger' / 'campaign.json'}\nset campaign_root in lbp-config.json to: {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
