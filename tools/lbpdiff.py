#!/usr/bin/env python3
"""Side-by-side disassembly of a source's linked bytes against the retail bytes (after lbpmatch.py)."""
from __future__ import annotations

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


def main() -> int:
    for arg in sys.argv[1:]:
        src = Path(arg).resolve()
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
    ours = []
    if ours_bin.exists():
        body = function_bytes(ours_bin, start, target.stat().st_size)
        if body != ours_bin.read_bytes():
            ours_bin = ours_bin.with_name(ours_bin.stem + ".function.bin")
            ours_bin.write_bytes(body)
        ours = disasm(ours_bin, start)
    theirs = disasm(target, start)
    print(f"== {title}  (ours | retail)")
    for i in range(max(len(ours), len(theirs))):
        a = ours[i] if i < len(ours) else ""
        b = theirs[i] if i < len(theirs) else ""
        mark = "  " if a.split(" ")[0:1] == b.split(" ")[0:1] else "**"
        print(f"{mark} {start + 4 * i:08x}  {a:<44.44} | {b}")


def function_bytes(path: Path, start: int, span: int) -> bytes:
    """Our bytes of the function at `start`, as lbpmatch.py compares them. For a source without helpers=, text.bin is
    the whole linked .text, which starts before the function when the object defines other code first (a library file
    compiled whole): lbpmatch compares from the function's address, for the header span or for the symbol's size when
    that is longer. Any other file (a helpers= section, or no linked.elf beside it) is returned as it is."""
    data = path.read_bytes()
    elf_path = path.parent / "linked.elf"
    if not elf_path.exists():
        return data
    elf = elf_path.read_bytes()
    text = M.linked_section(elf, ".text")
    if text is None or text[1] != data or not text[0] <= start < text[0] + len(data):
        return data
    size = max((s for n, v, s, i, sh in M.elf_symbols(elf) if n and v == start and sh != 0), default=0)
    return data[start - text[0]:][:max(span, size)]


if __name__ == "__main__":
    raise SystemExit(main())
