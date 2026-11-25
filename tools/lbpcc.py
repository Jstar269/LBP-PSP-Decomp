#!/usr/bin/env python3
"""Compile one scratch C/C++ file with a compiler profile of lbpmatch.py and print its disassembly (no header, no link).

    python tools/lbpcc.py probe.c [--cc gcc404xs-Os] [--lang c99|c++] [--build 219] [--extra "-fno-inline"]

For quick probes (does this spelling fold? which vtable offset does this virtual call use?) where lbpmatch.py
would need an LBPMATCH header and a retail address. `--cc` takes any GCC profile of lbpmatch.py or `mwcc219`
(default: the primary profile); a GCC probe compiles as lbpmatch.py compiles a source, with `-I include`, so the
project headers are found. SNC (`snc127-*`) runs natively on Windows and is not offered here. The relocations are
printed, so symbol references read as `DAT_xxx` against each lui/lo pair instead of linked numbers. MWCC builds
other than 219 are for experiments only: no source may depend on one. Exits 1 when the compile writes no object.
Adopted from the ds10 lane's asmdump.sh.
"""
from __future__ import annotations

import argparse
import shlex
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", type=Path)
    ap.add_argument("--lang", choices=("c99", "c++"), help="default: c++ for .cpp, c99 otherwise")
    ap.add_argument("--build", default=M.PROFILE["build"], help="mwccpsp build under the native kit (default 219)")
    ap.add_argument("--extra", default="", help="extra compiler flags")
    ap.add_argument("--cc", default=M.PRIMARY_PROFILE, help="compiler profile (default: the primary profile "
                    f"{M.PRIMARY_PROFILE}); a GCC profile of lbpmatch.py or mwcc219")
    args = ap.parse_args()
    if args.cc in M.SNC_PROFILE_NAMES:
        ap.error(f"--cc {args.cc}: SNC runs natively on Windows, not in WSL; check an SNC source with lbpmatch.py")
    if args.cc not in M.GCC_PROFILE_NAMES and args.cc != "mwcc219":
        ap.error(f"unknown compiler profile --cc {args.cc} (a GCC profile of lbpmatch.py, or mwcc219)")
    src = args.source.resolve()
    lang = args.lang or ("c++" if src.suffix == ".cpp" else "c99")
    out = M.ROOT / "build" / "lbpcc"
    out.mkdir(parents=True, exist_ok=True)
    obj_path = out / f"{src.stem}.o"
    obj_path.unlink(missing_ok=True)       # an object left by an earlier probe must not pass for this one
    obj = shlex.quote(M.to_wsl(obj_path))
    if args.cc in M.GCC_PROFILE_NAMES:
        g = M.gcc_profiles()[args.cc]
        bprefix = f"-B {g['B']} " if g["B"] else ""
        xlang = "-x c++" if lang == "c++" else "-x c"
        log = out / f"{src.stem}.log"
        # as lbpmatch.py: a copy in a native temporary directory (the i386 compilers cannot stat files on /mnt/c), with
        # ./include linked to a native copy of include/
        job = M.gcc_job(src, log, obj_path, f"{g['driver']} {bprefix}-c {g['flags']} {xlang} -I include {args.extra} "
                                            f"-o obj.o {shlex.quote(src.name)}", "lbpcc")
        status = M.run_wsl(M.gcc_stage_lines(M.ROOT / "include")
                           + [f"( {job} )", f"cat {shlex.quote(M.to_wsl(log))}",
                              f"[ -f {obj} ] && {M.pspdev_bin()}/psp-objdump -dr {obj} | tail -n +6",
                              'rm -rf "${LBPINC:?}"'], "lbpcc")
        if status == M.STAGE_FAILED:
            raise SystemExit("could not copy include/ into WSL (message above); nothing compiled")
    else:
        M.run_wsl([f"cd {shlex.quote(M.to_wsl(src.parent))} && MWCIncludes={shlex.quote(M.to_wsl(M.ROOT / 'include'))} "
                   f"$N/wibo $N/builds/{args.build}/mwccpsp.exe {M.PROFILE['flags']} -lang {lang} {args.extra} "
                   f"-o {obj} {shlex.quote(src.name)} && {M.pspdev_bin()}/psp-objdump -dr {obj} | tail -n +6"],
                  "lbpcc", M.native_dir())
    if not obj_path.exists():
        print(f"lbpcc: {args.source} did not compile with {args.cc} (the compiler's messages are above)", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
