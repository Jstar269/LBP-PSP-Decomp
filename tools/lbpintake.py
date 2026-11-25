#!/usr/bin/env python3
"""Intake of the owner's disc: extract the image, identify the disc, decrypt its modules, describe them.

    python tools/lbpintake.py --archive "<...>.7z" | --iso <image.iso>  [--decryptor <psp_game_decryptor.py>]

Steps (each skipped when its output already exists; nothing is overwritten):
  1. extract the .7z (7-Zip) into inputs/iso/;
  2. read PSP_GAME/PARAM.SFO from the image (DISC_ID, DISC_VERSION, TITLE, PSP_SYSTEM_VER) and list the image's
     executable files (EBOOT.BIN, BOOT.BIN, *.PRX, *.PBP) with sizes -> inputs/disc.json;
  3. run the PSP game decryptor (setting `decryptor`, or --decryptor) read-only (python -B, so it writes nothing next to itself) with its output root
     inputs/decrypt/ and its private report inputs/decrypt-report.json (no --include-pbp: the nested-PBP route stages
     its DATA.PSP under the executable's name, see docs/HANDOFF.md); a failing decryptor stops the intake (exit 1)
     and what it wrote is moved to inputs/decrypt.failed/, so the next run tries again;
  4. describe every decrypted module (tools/lbpmodinfo.py) -> inputs/modinfo.json, and check each is a 32-bit
     little-endian MIPS ELF/PRX (exit 1 when there is none).

Everything stays under the gitignored inputs/. The decryptor never decrypts firmware (owner rule).
"""
from __future__ import annotations

import argparse
import json
import shutil
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpconfig as CFG  # noqa: E402
import lbpmodinfo as MI  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
INPUTS = ROOT / "inputs"
SEVEN_ZIP = r"C:\Program Files\7-Zip\7z.exe"


def parse_sfo(b: bytes) -> dict:
    if b[:4] != b"\0PSF":
        raise ValueError("not a PARAM.SFO")
    _magic, _ver, key_off, data_off, count = struct.unpack_from("<IIIII", b, 0)
    out = {}
    for i in range(count):
        k_off, fmt, length, _max, d_off = struct.unpack_from("<HHIII", b, 20 + 16 * i)
        key = b[key_off + k_off:b.index(b"\0", key_off + k_off)].decode("ascii")
        raw = b[data_off + d_off:data_off + d_off + length]
        out[key] = struct.unpack("<I", raw[:4])[0] if fmt == 0x0404 else raw.rstrip(b"\0").decode("utf-8", "replace")
    return out


def seven_zip(*args: str, capture: bool = True) -> subprocess.CompletedProcess:
    return subprocess.run([SEVEN_ZIP, *args], capture_output=capture, check=False)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    src = ap.add_mutually_exclusive_group(required=True)
    src.add_argument("--archive", type=Path)
    src.add_argument("--iso", type=Path)
    ap.add_argument("--decryptor", type=Path, default=None,
                    help="the PSP game decryptor script (default: the `decryptor` setting of lbp-config.json)")
    ap.add_argument("--inputs", type=Path, default=None, help="output directory (default: the gitignored inputs/)")
    args = ap.parse_args()
    if args.decryptor is None:
        args.decryptor = CFG.host_path("decryptor")
    global INPUTS
    INPUTS = (args.inputs or INPUTS).resolve()
    INPUTS.mkdir(parents=True, exist_ok=True)
    iso = args.iso
    if args.archive:
        out = INPUTS / "iso"
        isos = sorted(out.glob("*.iso")) if out.exists() else []
        if not isos:
            out.mkdir(parents=True, exist_ok=True)
            r = seven_zip("x", "-y", f"-o{out}", str(args.archive))
            if r.returncode != 0:
                print(r.stdout.decode(errors="replace")[-2000:], r.stderr.decode(errors="replace")[-2000:])
                raise SystemExit("7-Zip extraction failed")
            isos = sorted(out.glob("*.iso"))
        if len(isos) != 1:
            raise SystemExit(f"expected one .iso in {out}, found {len(isos)}")
        iso = isos[0]
    print(f"image: {iso} ({iso.stat().st_size} bytes)")

    disc_json = INPUTS / "disc.json"
    if not disc_json.exists():
        sfo = seven_zip("e", "-so", str(iso), "PSP_GAME/PARAM.SFO").stdout
        listing = seven_zip("l", "-slt", str(iso)).stdout.decode("utf-8", "replace")
        files, cur = [], {}
        for line in listing.splitlines():
            if line.startswith("Path = "):
                cur = {"path": line[7:].replace("\\", "/")}
            elif line.startswith("Size = ") and cur:
                cur["size"] = int(line[7:] or 0)
                p = cur["path"].upper()
                if p.endswith((".BIN", ".PRX", ".PBP", ".ELF")) and "/" in p:
                    files.append(cur)
                cur = {}
        disc = {"image": iso.name, "image_size": iso.stat().st_size, "param_sfo": parse_sfo(sfo), "executables": files}
        disc_json.write_text(json.dumps(disc, indent=1) + "\n", encoding="utf-8")
    disc = json.loads(disc_json.read_text(encoding="utf-8"))
    sfo = disc["param_sfo"]
    print(f"disc: {sfo.get('DISC_ID')} version {sfo.get('DISC_VERSION')} ({sfo.get('TITLE')}), "
          f"firmware {sfo.get('PSP_SYSTEM_VER')}, category {sfo.get('CATEGORY')}")
    for f in disc["executables"]:
        print(f"  {f['path']:60} {f['size']:>10}")

    stage = INPUTS / "decrypt"
    if not stage.exists():
        cmd = [sys.executable, "-B", str(args.decryptor), str(iso), "--manual", "--apply", "--role", "all",
               "--module-scope", "all", "--include-protected-modules", "--user-data-root", str(stage),
               "--private-report", str(INPUTS / "decrypt-report.json"), "--verbose"]
        r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(args.decryptor.parent))
        (INPUTS / "decrypt.log").write_text(r.stdout + r.stderr, encoding="utf-8")
        print("\n".join(line for line in r.stdout.splitlines() if line.startswith(("ITEM", "PSP_DECRYPTOR"))))
        if r.returncode != 0:
            # a failed run's output is no result: set it aside, so that it is not described now and the next run
            # does not skip this step as done
            failed = INPUTS / "decrypt.failed"
            if failed.exists():
                shutil.rmtree(failed)
            if stage.exists():
                stage.rename(failed)
            raise SystemExit(f"the decryptor failed (exit {r.returncode}); its output is in {INPUTS / 'decrypt.log'}"
                             + (f", what it wrote in {failed}" if failed.exists() else ""))
    reports = []
    for path in sorted(p for p in stage.rglob("*") if p.is_file()):
        try:
            rep = MI.describe(path, nids=True)
        except ValueError as e:
            print(f"  {path.relative_to(stage)}: NOT AN ELF ({e})")
            continue
        rep["path"] = str(path)
        ok = rep["machine"] == 8
        print(f"  {path.relative_to(stage)}: {rep['size']} B {rep['elf_type']} MIPS={'yes' if ok else 'NO'} "
              f"module={rep.get('module_info', {}).get('name')!r} comment={rep.get('comment')}")
        reports.append(rep)
    (INPUTS / "modinfo.json").write_text(json.dumps(reports, indent=1) + "\n", encoding="utf-8")
    if not reports:
        print(f"no decrypted module under {stage} is an ELF/PRX: nothing to describe", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
