#!/usr/bin/env python3
"""Machine-specific settings (tool and data locations) for the LBP decomp tools.

Settings are read from `lbp-config.json` in the repository root. That file is gitignored: copy
`lbp-config.example.json` to `lbp-config.json` and fill in your own paths. Every key can also be set, or
overridden, by an environment variable `LBP_<KEY>` (the key in upper case, e.g. `LBP_CAMPAIGN_ROOT`); the
environment wins over the file. Keys whose name starts with an underscore in the JSON file are comments.

A tool asks for a setting only when it needs it (`require`, `host_path`, `wsl_path`), so `--help` and the
purely local tools work without any configuration. An unset or unusable setting stops the tool with a
message that names the key, the file and the environment variable.

Two kinds of path exist, because the tools run on the host and drive the compilers inside WSL:
  host  a path on the machine running the tools (Windows: `C:\\...`), used with Python's `Path`
  wsl   a path as seen from inside WSL (`/mnt/c/...`, `$HOME/...`), pasted verbatim into `sh` command lines

    python tools/lbpconfig.py list          every key with its value and where the value came from
    python tools/lbpconfig.py get <key>     print one value (for shell scripts); exit 1 if unset
    python tools/lbpconfig.py check         list, then test that every configured path exists (WSL ones via wsl.exe)
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import NamedTuple

ROOT = Path(__file__).resolve().parents[1]
CONFIG_FILE = ROOT / "lbp-config.json"
ENV_PREFIX = "LBP_"


class Key(NamedTuple):
    kind: str   # "host" (must exist), "host-new" (host path that may be created by the tool) or "wsl"
    probe: str  # path below the value that `check` looks for ("" = the value itself)
    doc: str


KEYS: dict[str, Key] = {
    "campaign_root": Key("host", "ledger/campaign.json",
                         "Campaign directory holding ledger/campaign.json (function list and load segments) and "
                         "inputs/revision-selection.json (where your decrypted retail modules live). Read by every "
                         "tool that compares against, or lists, retail bytes."),
    "pspdev_bin": Key("wsl", "psp-ld",
                      "WSL directory with the PSPDEV binutils psp-ld, psp-objcopy and psp-objdump "
                      "(lbpmatch.py links with them, lbpdiff.py disassembles with them)."),
    "mwcc_native": Key("wsl", "wibo",
                       "WSL directory (on the native Linux filesystem, not /mnt/c) holding the wibo binary and "
                       "builds/<build>/mwccpsp.exe, e.g. builds/219/mwccpsp.exe."),
    "gcc_home": Key("wsl", "psp-gcc-1.3.1/bin/psp-gcc",
                    "WSL directory holding the decomp.me Sony psp-gcc kits: psp-gcc-1.3.1/ and psp-gcc-1.7.1/ "
                    "(used for sources with cc=gcc131-* / gcc171-*)."),
    "newlib_home": Key("wsl", "",
                       "WSL directory with the newlib source trees <version>/newlib-<version>/newlib and the "
                       "generated include directories inc-<version> (lbpnewlib.py preprocesses library sources)."),
    "newlib_obj_root": Key("host", "",
                           "Host-visible path of the library objects built from newlib, one directory per "
                           "configuration such as 1.14.0-171-O2 (lbplib.py index). From Windows a WSL directory is "
                           "\\\\wsl.localhost\\<distribution>\\home\\<user>\\...\\obj."),
    "ext_root": Key("host-new", "",
                    "Maintainers only: host directory for the isolated workspaces of external agent lanes "
                    "(not needed to build or verify)."),
    "gcc4_home": Key("wsl", "",
                     "WSL directory holding the open-source GCC 4.x PSP cross compilers this project builds from the "
                     "GNU and pspdev sources (toolchain/gcc4/README.md): psp-gcc-4.0.4c/ (the primary profile), "
                     "psp-gcc-4.0.2x/, psp-gcc-4.0.2h/, psp-gcc-4.1.0s/ and the others the profiles name."),
    "decryptor": Key("host", "",
                     "Host path of the PSP game decryptor script that tools/lbpintake.py runs on your own disc image "
                     "(it decrypts the game's modules into the gitignored inputs/; never firmware)."),
    "snc_home": Key("host", "pspsnc.exe",
                    "Host directory of the SN Systems SNC kit for PSP (decomp.me pspsnc_1.2.7503.0: pspsnc.exe and its "
                    "helpers). Sources with cc=snc127-* compile with it, natively on Windows."),
    "hst_root": Key("host", "tools/hstmatch.py",
                    "Optional, read-only: a checkout of the Hot Shots Tennis: Get a Grip decomp (hst-decomp). "
                    "lbpxclone.py uses its exact sources as exemplars for code both games share (SDK, libraries)."),
    "hst_campaign_root": Key("host", "ledger/campaign.json",
                             "Optional, read-only: hst-decomp's campaign directory (its ledger/campaign.json and "
                             "inputs/revision-selection.json), from which lbpxclone.py reads HST's retail bytes."),
    "nakagawa_root": Key("host", "tools/import_audit_baseline.json",
                         "Optional: a checkout of Nakagawa Recomp (github.com/Jstar269/nakagawa-recomp), whose runtime is "
                         "the platform layer of the port milestones. lbpmilestones.py reads its reviewed import audit "
                         "(tools/import_audit_baseline.json) to measure the platform-layer milestone."),
    "mwcc_builds_dir": Key("host", "",
                           "Host directory with the decomp.me mwccpsp builds, <build>/mwccpsp.exe "
                           "(lbpsample.py records its digest)."),
    "wibo_exe": Key("host", "",
                    "Host path of the wibo binary (lbpsample.py records its digest)."),
    "ghidra_headless": Key("host", "",
                           "Ghidra's support/analyzeHeadless.bat (ghidra/run_commit_decompile.ps1)."),
    "ghidra_allegrex": Key("host", "",
                           "The ghidra-allegrex processor extension directory, copied once into ghidra/settings/ "
                           "(ghidra/import_module.ps1)."),
    "java_home": Key("host", "bin",
                     "JDK that Ghidra runs on (JAVA_HOME for ghidra/run_commit_decompile.ps1)."),
}


class ConfigError(SystemExit):
    """A setting is missing or unusable. Derives from SystemExit so that an uncaught error ends a tool with
    the message and exit status 1 instead of a traceback (it is not an `Exception`, so `except Exception`
    blocks in the tools do not swallow it)."""

    def __init__(self, message: str):
        super().__init__(f"lbp-config: {message}")


_file_cache: dict[str, str] | None = None


def env_name(key: str) -> str:
    return ENV_PREFIX + key.upper()


def _file_values() -> dict[str, str]:
    global _file_cache
    if _file_cache is not None:
        return _file_cache
    values: dict[str, str] = {}
    if CONFIG_FILE.exists():
        try:
            data = json.loads(CONFIG_FILE.read_text(encoding="utf-8-sig"))
        except ValueError as e:
            raise ConfigError(f"{CONFIG_FILE} is not valid JSON: {e}") from None
        if not isinstance(data, dict):
            raise ConfigError(f"{CONFIG_FILE} must contain a JSON object")
        for k, v in data.items():
            if k.startswith("_"):
                continue
            if k not in KEYS:
                raise ConfigError(f"unknown key {k!r} in {CONFIG_FILE} (known keys: {', '.join(KEYS)})")
            if not isinstance(v, str):
                raise ConfigError(f"key {k!r} in {CONFIG_FILE} must be a string")
            if v.strip():
                values[k] = v.strip()
    _file_cache = values
    return values


def source(key: str) -> str:
    """'env', 'file' or 'unset'."""
    if key not in KEYS:
        raise KeyError(f"unknown lbp-config key {key!r}")
    if os.environ.get(env_name(key), "").strip():
        return "env"
    return "file" if key in _file_values() else "unset"


def get(key: str) -> str | None:
    """The configured value, or None when neither the environment nor the file sets it."""
    if source(key) == "env":
        return os.environ[env_name(key)].strip()
    return _file_values().get(key)


def require(key: str) -> str:
    value = get(key)
    if value is None:
        raise ConfigError(
            f"setting '{key}' is not set. Add \"{key}\" to {CONFIG_FILE} (copy lbp-config.example.json) or set the "
            f"environment variable {env_name(key)}.\n  {key}: {KEYS[key].doc}")
    return value


def host_path(key: str) -> Path:
    """A host path setting as a Path; must exist unless the key's kind is 'host-new'."""
    assert KEYS[key].kind.startswith("host"), key
    p = Path(os.path.expanduser(require(key)))
    if KEYS[key].kind == "host" and not p.exists():
        raise ConfigError(f"setting '{key}' points to {p}, which does not exist (set in {CONFIG_FILE} or "
                          f"{env_name(key)}).\n  {key}: {KEYS[key].doc}")
    return p


def wsl_path(key: str) -> str:
    """A WSL path setting, verbatim (it may contain `$HOME`, expanded by the shell inside WSL)."""
    assert KEYS[key].kind == "wsl", key
    return require(key).rstrip("/") or "/"


def _wsl_exists(path: str) -> bool:
    if '"' in path:
        return False
    try:
        r = subprocess.run(["wsl.exe", "-e", "sh", "-c", f'test -e "{path}"'], capture_output=True)
    except FileNotFoundError:
        raise ConfigError("wsl.exe was not found, so the 'wsl' settings cannot be checked. The tools run on a Windows "
                          "host and reach the compilers inside WSL (docs/GUIDE.md, Requirements).") from None
    return r.returncode == 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command")
    sub.add_parser("list")
    sub.add_parser("check")
    g = sub.add_parser("get")
    g.add_argument("key", choices=sorted(KEYS))
    args = ap.parse_args()
    if args.command == "get":
        print(require(args.key))
        return 0
    check = args.command == "check"
    print(f"config file: {CONFIG_FILE} ({'present' if CONFIG_FILE.exists() else 'absent'})")
    bad = 0
    for key, spec in KEYS.items():
        value = get(key)
        status = ""
        if value is None:
            status = "UNSET"
        elif check:
            target = value.rstrip("/\\") + ("/" + spec.probe if spec.probe else "")
            if spec.kind == "wsl":
                ok = _wsl_exists(target)
            else:
                ok = Path(os.path.expanduser(target)).exists() or spec.kind == "host-new"
            status = "ok" if ok else "MISSING"
        if status in ("UNSET", "MISSING"):
            bad += 1
        print(f"{key:16} [{source(key):5}] {status:8} {value if value is not None else '(set ' + env_name(key) + ' or ' + key + ' in the config file)'}")
    if check and bad:
        print(f"{bad} setting(s) unset or missing", file=sys.stderr)
    return 1 if check and bad else 0


if __name__ == "__main__":
    raise SystemExit(main())
