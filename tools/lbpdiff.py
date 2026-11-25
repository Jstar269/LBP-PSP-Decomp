#!/usr/bin/env python3
"""Side-by-side disassembly of a source's linked bytes against the retail bytes (after lbpmatch.py)."""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M


def disasm(path: Path, vma: int) -> list[str]:
    out = subprocess.run(["wsl.exe", "-e", f"{M.PSPDEV_BIN}/psp-objdump", "-D", "-z", "-b", "binary", "-m", "mips:allegrex",
                          "-EL", f"--adjust-vma=0x{vma:x}", M.to_wsl(path)], capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        parts = line.split("\t")
        if len(parts) >= 3 and parts[0].strip().endswith(":"):
            rows.append(f"{parts[1].strip()} {' '.join(p.strip() for p in parts[2:])}")
    return rows


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__ + "\n\n    python tools/lbpdiff.py <source> [<source> ...]\n\n"
                                 "Run `python tools/lbpmatch.py <source>` first: lbpdiff reads that run's build/ "
                                 "products. `**` marks a word that differs.",
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("sources", nargs="+", type=Path, help="source files with an LBPMATCH header")
    args = ap.parse_args(argv)
    for arg in args.sources:
        src = Path(arg).resolve()
        if not src.is_file():
            ap.error(f"{arg}: no such file")
        h = M.parse_header(src)
        out = M.ROOT / "build" / h["module"] / M.build_key(src)
        cache: dict = {}
        data, seg = M.module_bytes(cache, h["module"])
        off = seg["file_offset"] + h["start"] - seg["start"]
        target = out / "target.bin"
        target.write_bytes(data[off:off + h["end"] - h["start"]])
        show(src.name, out / "text.bin", target, h["start"])
        for name, start, end in h.get("helpers") or []:   # same-file helpers (helpers=), each at its own address
            ours = out / f"helper-{name}.bin"
            theirs = out / f"helper-{name}.target.bin"
            o = seg["file_offset"] + start - seg["start"]
            theirs.write_bytes(data[o:o + end - start])
            show(f"{src.name} helper {name}", ours, theirs, start)
    return 0


def show(title: str, ours_bin: Path, target: Path, start: int) -> None:
    ours = disasm(ours_bin, start) if ours_bin.exists() else []
    theirs = disasm(target, start)
    print(f"== {title}  (ours | retail)")
    for i in range(max(len(ours), len(theirs))):
        a = ours[i] if i < len(ours) else ""
        b = theirs[i] if i < len(theirs) else ""
        mark = "  " if a.split(" ")[0:1] == b.split(" ")[0:1] else "**"
        print(f"{mark} {start + 4 * i:08x}  {a:<44.44} | {b}")


if __name__ == "__main__":
    raise SystemExit(main())
