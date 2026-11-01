#!/usr/bin/env python3
"""The promotion gate: the only writer of the accepted ledger and the two locks (formats: tools/lbpproof.py).

Ported from hst-decomp's tools/hstpromote.py (same design), adapted to tools/lbpmatch.py: GCC profiles run in WSL,
SNC natively on the host, MWCC with wibo; placed .rodata/.data/.sdata and .bss/.sbss; `.gnu.linkonce` data objects
placed at their symbol-map addresses.

    python tools/lbppromote.py lock --reason TEXT        create the target and toolchain locks (refuses when they exist)
    python tools/lbppromote.py seed [--baseline JSON] [--report FILE]
                                                         the first ledger, from a full verification run of src/
    python tools/lbppromote.py promote [PATH ...] [--paths-from FILE] [--all] [--strict]
                                       [--promoted-out FILE] [--json FILE]
    python tools/lbppromote.py verify [--since REV | --sample N] [--json FILE]
                                                         re-run accepted proofs and compare (never writes)
    python tools/lbppromote.py migrate-toolchain --reason TEXT
    python tools/lbppromote.py retarget --reason TEXT
    python tools/lbppromote.py retract MODULE START --reason TEXT
  common: --parallel N (concurrent compiles, default 4; at most 4 while other sessions compile), --keep-run,
          --lock-wait SECONDS, --json FILE

VERIFICATION RUN. Every proof comes from one, in a fresh directory build/proof/<run id>/ (never reused, so no artifact
of an earlier build can be read):
  1. pre-checks: a regular .c/.cpp file under src/; a header the matcher parses; no `extra=`; a compiler profile the
     toolchain lock records (cc=, or the locked primary profile for a header without cc=); a module the target lock
     records; a range that starts on a function start and ends on a function end of the locked inventory (no
     partial-function claim); the lbpgate check passes (including LBP's inline-VFPU rule, `asm=vfpu`).
  2. tools/lbpmatch.py on the candidates (LBPMATCH_BUILD_ROOT=<run>/match; its EXACT semantics are unchanged).
  3. this tool's own compile of every candidate, from the profile the toolchain lock records (GCC in WSL from a copy
     of the source in a native temporary directory, SNC natively from the source's directory, MWCC with wibo): the
     object must be byte-identical to the matcher's (the compilers are deterministic), so the matcher cannot
     substitute an object. The one exception is GCC's random seed: a file whose static initialiser has no public name
     gets `_GLOBAL__I_<file>_<8 hex>_<8 random hex>` names, so the last eight digits of such a name in a string table
     may differ (lbpproof.normalized_object; nothing else may), and the proof records the normalized object's SHA-256.
  4. on the matcher's artifacts, with tools/lbpproof.py's own ELF reader: each was written during this run; the code
     sections of the object (.text, .text.*, .gnu.linkonce.t.*, laid out in file order from the linked .text address,
     as ld's default script does) put the function at its retail address, and it is linked there once; the linked code
     is the object's code with only relocation fields filled in; the claimed range lies inside the object's code; the
     bytes equal the retail bytes, read through the module's own program headers from the image the target lock pins;
     the same for each placed .rodata/.data/.sdata (header keys) and each `.gnu.linkonce` data object the matcher
     placed (at its symbol-map address; every one the compared code refers to must be placed); a placed .bss/.sbss is
     linked at its header address; the link script defines exactly the object's undefined names, each at the address
     the symbol map (or its address name) gives, with placeholders only for names the compared code does not
     reference. A source with same-file helpers (header `helpers=`, an mwcc profile) is checked function by function:
     every function of the object is the target or a listed helper (each on function boundaries of the locked
     inventory), alone in its own .text section, linked once at its own retail address, the object's code with only
     relocation fields filled in, and equal to the retail bytes there; the proof claims only the header's range.
  5. the source is unchanged since the run started.
A source that passes all of it gets a proof; any failure is a refusal that names the reason.

PROMOTE. Preflight (any failure refuses everything): the target lock matches the live target, the toolchain lock the
live toolchain (for the locked profiles), the ledger and journal load and validate, every proof is bound to the locked
digests. Candidates:
  * every accepted proof that no longer holds for the tree (its source changed or vanished, a header it includes
    changed, a name it links or an object it places resolves differently): always reverified, whatever the arguments;
  * the named sources (default: every source in src/ without a proof); --all reverifies every proof as well.
V_new = the proofs that still hold + the new ones. A new proof that overlaps another owner's bytes is refused (each
byte has one owner, so a clone or a multi-function source counts once). The ledger is written only when V_new covers
every byte V_accepted covers, compared as byte ranges per module, not as totals; otherwise the promotion is REFUSED, the
ledger is left untouched and the lost ranges are named. --strict also refuses when any candidate is refused.
Exit status: 0 written or nothing to do; 3 written (or unchanged) while some candidates were refused; 1 refused;
2 usage error. --promoted-out lists the named sources that hold a valid proof afterwards.

SEED. Only when no ledger exists: a verification run of every source in src/. Where proofs overlap, the larger range
owns the bytes and a contained source is reported as subsumed (not a proof); a partial overlap is refused. --baseline
compares the run with an earlier `lbpmatch.py --all --json` file and reports every difference.

VERIFY re-runs accepted proofs (--since REV: those that differ from the ledger at REV; --sample N: a random sample;
default all) and compares the object, the compared bytes and the dependencies with the proof. Exit 1 on any
difference. It never writes.

MIGRATE-TOOLCHAIN (the qualified migration): when the live toolchain differs from the lock (a compiler, kit, binutils,
include/, profile, primary-profile or comparison-code change), the live toolchain is hashed for the locked profiles
plus every profile src/ now uses and the primary; then every accepted proof that the change can affect is reverified
with it (all of them for a compiler, flag, primary or comparison-code change; none when profiles were only added; the
dependents for an include/-only change; the gate alone for an lbpgate-only change). Only if all hold are the lock
(with the reason and the changes in its history) and the rebound ledger written.
RETARGET: when the live target differs from the lock (a boundary changed, ...), every proof is checked against the
new target (inventory alignment, the retail bytes of every compared range); only if all hold is the lock rewritten
with the reason. RETRACT removes one proof explicitly, with a reason in the journal; nothing else can remove accepted
bytes, and `lbpproof.py check --base` reports any other loss.
"""
from __future__ import annotations

import argparse
import bisect
import json
import os
import random
import re
import shutil
import subprocess
import sys
import time
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))
import lbpgate  # noqa: E402
import lbpmatch as M  # noqa: E402
import lbpproof as P  # noqa: E402
from lbpproof import ProofError, hx, unhx  # noqa: E402

ROOT = P.ROOT
CMDLINE_BUDGET = 24000  # characters of source paths per lbpmatch.py call (Windows limits a command line to 32767)
PLACED = ("rodata", "data", "sdata")
NOBITS_PLACED = ("bss", "sbss")
PLACEHOLDER = "/* not referenced by the compared function */"
# input sections ld's default script (psp-ld --verbose) puts into each placed output section, by the first matching
# rule; the object's inputs of an output are laid out in file order
CODE_INPUT = re.compile(r"\.text|\.stub|\.text\..+|\.gnu\.linkonce\.t\..+")
EARLY_CODE = re.compile(r"\.text\.(?:unlikely|exit|startup|hot|sorted)(?:\..*)?|\.text\..*_unlikely")
OUTPUT_INPUTS = {"rodata": re.compile(r"\.rodata|\.rodata\..+|\.gnu\.linkonce\.r\..+"),
                 "data": re.compile(r"\.data|\.data\..+|\.gnu\.linkonce\.d\..+"),
                 "sdata": re.compile(r"\.sdata|\.sdata\..+|\.gnu\.linkonce\.s\..+")}
AMBIGUOUS_DATA = re.compile(r"\.data\.rel\.ro.*|\.gnu\.linkonce\.d\.rel\.ro.*|\.rodata\.sce.*|\.sdata2.*")
LINKONCE_DATA = re.compile(r"\.gnu\.linkonce\.(?:b|sb|d|s|r)\.(?P<symbol>.+)")
PLACED_LINKONCE = re.compile(r"\.lbp_lo\d+")


def say(msg: str) -> None:
    print(msg, flush=True)


# --------------------------------------------------------------------------------------------------------------
# lock and preflight

class PromoteLock:
    """try/promote.lock (mkdir): one writer at a time."""

    def __init__(self, root: Path, wait: float):
        self.path = root / "try" / "promote.lock"
        self.wait = wait

    def __enter__(self):
        self.path.parent.mkdir(parents=True, exist_ok=True)
        deadline = time.time() + self.wait
        told = False
        while True:
            try:
                self.path.mkdir()
                break
            except FileExistsError:
                if time.time() >= deadline:
                    info = (self.path / "owner.json").read_text(encoding="utf-8") if (self.path / "owner.json").exists() else "?"
                    raise ProofError("lock", f"another promotion holds {self.path} ({info.strip()}); remove the directory "
                                             "only when no lbppromote.py is running") from None
                if not told:
                    say(f"waiting for {self.path} ...")
                    told = True
                time.sleep(5)
        (self.path / "owner.json").write_text(json.dumps({"pid": os.getpid(), "utc": P.utc_now(), "argv": sys.argv[1:4]}),
                                              encoding="utf-8")
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.path, ignore_errors=True)


class Context:
    """The locks, verified against the live target and toolchain, and the ledger, validated and bound."""

    def __init__(self, root: Path = ROOT, need_ledger: bool = True, check_toolchain: bool = True):
        self.root = root
        data = root / "data"
        self.target_path, self.toolchain_path = data / P.TARGET_PATH.name, data / P.TOOLCHAIN_PATH.name
        self.ledger_path, self.journal_path = data / P.LEDGER_PATH.name, data / P.JOURNAL_PATH.name
        self.target = P.load_target_manifest(self.target_path)
        self.live_target = P.verify_target(self.target, P.live_target(root))
        self.tool = P.load_toolchain_manifest(self.toolchain_path)
        if check_toolchain:
            P.verify_toolchain(self.tool, P.live_toolchain(root, P.locked_profile_names(self.tool)))
        self.ledger = P.load_ledger(self.ledger_path, self.journal_path, required=need_ledger)
        if self.ledger is not None:
            P.check_binding(self.ledger, self.tool["core_digest"], self.target["target_digest"])
        self.inventories = {m["name"]: P.inventory(m["name"], self.target) for m in self.target["modules"]}
        self.primary = self.tool["core"]["primary"]

    def write(self, proofs: list[dict], event: dict) -> dict:
        return P.write_ledger(proofs, event, self.tool["core_digest"], self.target["target_digest"], self.ledger_path,
                              self.journal_path, self.root)


# --------------------------------------------------------------------------------------------------------------
# the verification run

@dataclass
class Candidate:
    path: Path
    rel: str
    sha: str = ""
    header: dict | None = None


def candidates_for(rels, tree: dict[str, Path]) -> list[Candidate]:
    return [Candidate(tree[r], r) for r in sorted(rels) if r in tree]


def q(s: str) -> str:
    return "'" + s.replace("'", "'\\''") + "'"


class Run:
    def __init__(self, root: Path, core: dict, toolchain_digest: str, target: dict, inventories: dict,
                 parallel: int, keep: bool = False):
        self.root, self.profiles, self.primary = root, core["profiles"], core["primary"]
        self.toolchain_digest = toolchain_digest
        self.target, self.inventories, self.parallel, self.keep = target, inventories, parallel, keep
        self.id = time.strftime("%Y%m%dT%H%M%SZ", time.gmtime()) + "-" + os.urandom(3).hex()
        self.dir = root / "build" / "proof" / self.id
        self.dir.mkdir(parents=True)          # FileExistsError if it existed: a run directory is never reused
        self.started = time.time()
        self.modules = {m["name"] for m in target["modules"]}

    def cc(self, h: dict) -> str:
        return h.get("cc", self.primary)

    # -- 1. pre-checks
    def precheck(self, c: Candidate) -> None:
        p = c.path
        if p.is_symlink() or not p.is_file():
            raise ProofError("source", "not a regular file")
        r = P.rel(p, self.root)
        if not r.startswith("src/") or p.suffix not in P.SOURCE_SUFFIXES:
            raise ProofError("source", "only .c/.cpp files under src/ can be promoted")
        c.sha = P.sha256(p.read_bytes())
        try:
            h = M.parse_header(p)
        except ValueError as e:
            raise ProofError("header", str(e)) from None
        if "extra" in h:
            raise ProofError("extra-flags", "an `extra=` header is an experiment and never a match")
        cc = self.cc(h)
        if cc not in self.profiles:
            raise ProofError("profile", f"cc={cc} is not a compiler profile the toolchain lock records "
                                        "(lbppromote.py migrate-toolchain adds the profiles src/ uses)")
        if h["lang"] not in ("c99", "c++"):
            raise ProofError("header", f"lang={h['lang']} is not c99 or c++")
        if h["module"] not in self.modules:
            raise ProofError("header", f"module {h['module']} is not in the target lock")
        s, e = h["start"], h["end"]
        if not s < e or s % 4 or e % 4:
            raise ProofError("range", f"bad range [{hx(s)}, {hx(e)})")
        starts, ends = P.boundaries(self.inventories[h["module"]])
        if s not in starts or e not in ends:
            raise ProofError("range", f"[{hx(s)}, {hx(e)}) does not start and end on function boundaries of the locked "
                                      "inventory (a partial-function claim)")
        if h.get("helpers"):
            if self.profiles[cc].get("compiler") != "mwcc":
                raise ProofError("profile", "helpers= needs one .text section per function (an mwcc profile)")
            for name, hs, he in h["helpers"]:
                if hs not in starts or he not in ends:
                    raise ProofError("range", f"helper {name} [{hx(hs)}, {hx(he)}) is not a function of the locked "
                                              "inventory")
        gate = lbpgate.refusals(lbpgate.check_file(p, self.root))
        if gate:
            raise ProofError("gate", f"fails the fake-match gate: {lbpgate.summary(gate)}")
        c.header = h

    def build_dir(self, c: Candidate) -> Path:
        return self.dir / "match" / c.header["module"] / M.build_key(c.path)

    # -- 2. the matcher
    def run_matcher(self, cands: list[Candidate]) -> dict[str, dict]:
        env = dict(os.environ)
        env["LBPMATCH_BUILD_ROOT"] = str(self.dir / "match")
        rows: dict[str, dict] = {}
        chunks: list[list[Candidate]] = [[]]
        used = 0
        for c in cands:
            n = len(str(c.path)) + 3
            if chunks[-1] and used + n > CMDLINE_BUDGET:
                chunks.append([])
                used = 0
            chunks[-1].append(c)
            used += n
        for k, chunk in enumerate(chunks):
            out, log = self.dir / f"match-{k:03d}.json", self.dir / f"match-{k:03d}.log"
            cmd = [sys.executable, str(self.root / "tools" / "lbpmatch.py"), "--quiet", "--parallel", str(self.parallel),
                   "--json", str(out)] + [str(c.path) for c in chunk]
            with log.open("wb") as fh:
                subprocess.run(cmd, cwd=self.root, env=env, stdout=fh, stderr=subprocess.STDOUT, check=False)
            if out.exists():
                for row in json.loads(out.read_text(encoding="utf-8")):
                    rows[row["source"]] = row
            if len(chunks) > 1:
                say(f"  matcher {k + 1}/{len(chunks)} ({sum(1 for r in rows.values() if r['status'] == 'EXACT')} EXACT so far)")
        return rows

    # -- 3. the promoter's own compile
    def run_own_compile(self, cands: list[Candidate]) -> dict[str, Path]:
        cmds, out, native_jobs = [], {}, []
        include = self.root / "include"
        for i, c in enumerate(cands):
            d = self.dir / "own" / f"{i:05d}"
            d.mkdir(parents=True)
            obj, log = d / "obj.o", d / "compile.log"
            out[c.rel] = obj
            prof = self.profiles[self.cc(c.header)]
            if prof["compiler"] == "mwcc":
                native = M.native_dir()
                cmds.append(f"cd {q(P.to_wsl(c.path.parent))} && MWCIncludes={q(P.to_wsl(include))} {native}/wibo "
                            f"{native}/builds/{prof['build']}/mwccpsp.exe {prof['flags']} {prof['lang_option']} "
                            f"{c.header['lang']} -o {q(P.to_wsl(obj))} {q(c.path.name)} > {q(P.to_wsl(log))} 2>&1")
            elif prof["compiler"] == "psp-gcc":
                lang = "-x c++" if c.header["lang"] == "c++" else "-x c"
                home = P.kit_home(prof["kit"])
                bopt = f"-B {home}/{prof['B']} " if prof.get("B") else ""
                # the i386 Sony kits cannot stat files on /mnt/c: compile a copy in a native temporary directory, with
                # ./include linked to the batch's one native copy of include/ (profile_record's include_option;
                # tools/lbpmatch.py gcc_job and gcc_batch, which say why)
                cmds.append(M.gcc_job(c.path, log, obj,
                                      f"{home}/{prof['driver']} {bopt}-c {prof['flags']} {lang} -I include -o obj.o "
                                      f"{q(c.path.name)}", "lbpproof"))
            elif prof["compiler"] == "snc":
                native_jobs.append((c, prof, obj, log))
            else:
                raise ProofError("profile", f"unknown compiler kind {prof['compiler']!r} in the toolchain lock")
        if native_jobs:
            exe = M.snc_exe().parent / native_jobs[0][1]["exe"]

            def snc(job) -> None:
                c, prof, obj, log = job
                r = subprocess.run([str(exe), "-c", *prof["flags"].split(), f"-I{include}", "-o", str(obj), c.path.name],
                                   cwd=str(c.path.parent), capture_output=True, text=True)
                log.write_text(r.stdout + r.stderr, encoding="utf-8")

            with ThreadPoolExecutor(max_workers=max(1, self.parallel)) as pool:
                list(pool.map(snc, native_jobs))
        if cmds:
            jobs = self.dir / "own.jobs"
            jobs.write_text("\n".join(cmds) + "\n", encoding="utf-8", newline="\n")
            P.run_wsl_script("\n".join(M.gcc_batch(f"tr '\\n' '\\0' < {q(P.to_wsl(jobs))} | xargs -0 -P "
                                                   f"{self.parallel} -n 1 sh -c", include)),
                             self.dir, "own-compile")
        return out

    # -- 4. independent checks of the artifacts
    @staticmethod
    def linked_defs(ln: P.Elf) -> dict[str, list[int]]:
        out: dict[str, list[int]] = {}
        for s in ln.symbols():
            if s.shndx != 0 and s.name:
                out.setdefault(s.name, []).append(s.value)
        return out

    def compare(self, c: Candidate, obj: bytes, linked: bytes, syms_ld: str, symmap: dict) -> tuple[dict, list, list]:
        h = c.header
        if h.get("helpers"):
            return self.compare_placed(c, obj, linked, syms_ld, symmap)
        start, end = h["start"], h["end"]
        span = end - start
        o, ln = P.Elf(obj, "object"), P.Elf(linked, "linked image")
        code = [s for s in o.sections if s.type == P.SHT_PROGBITS and CODE_INPUT.fullmatch(s.name)]
        if not code:
            raise ProofError("object", "the object has no code section")
        early = [s.name for s in o.sections if EARLY_CODE.fullmatch(s.name)]
        if early:
            raise ProofError("placement", f"code sections this check does not lay out: {', '.join(early)}")
        index = {s.index for s in code}
        fsym = [s for s in o.symbols() if s.name == h["symbol"] and s.shndx in index]
        if len(fsym) != 1:
            raise ProofError("symbol", f"{h['symbol']} is not defined once in a code section of the object")
        fsym = fsym[0]
        fsize = fsym.size
        if fsize > span:
            raise ProofError("range", f"{h['symbol']} is {fsize} bytes, longer than the claimed {span}")
        outs = ln.named(".text")
        if len(outs) != 1:
            raise ProofError("placement", "the linked image has no single .text section")
        lsec = outs[0]
        # ld lays the object's code inputs out in file order from the output section's address, each aligned
        place = o.layout(code, lsec.addr)
        if place[0][1] != lsec.addr:
            raise ProofError("placement", "the first code section cannot start at the linked .text address")
        where = {sec.index: addr for sec, addr in place}
        code_lo, code_hi = place[0][1], place[-1][1] + place[-1][0].size
        if code_hi > lsec.addr + lsec.size:
            raise ProofError("placement", "the object's code does not fit the linked .text section")
        if where[fsym.shndx] + fsym.value != start:
            raise ProofError("placement", f"{h['symbol']} is laid out at {hx(where[fsym.shndx] + fsym.value)}, not at "
                                          f"its retail address {hx(start)}")
        if end > code_hi:
            raise ProofError("range", "the claimed range extends past the object's code")
        image = ln.contents(lsec)
        at = lambda a, n: image[a - lsec.addr:a - lsec.addr + n]   # noqa: E731
        linked_defs = self.linked_defs(ln)
        if linked_defs.get(h["symbol"]) != [start]:
            raise ProofError("placement", f"{h['symbol']} is not linked (once) at its retail address {hx(start)}")
        for s in o.symbols():   # every named code symbol of the object sits where the layout puts it
            if s.shndx in where and s.name and s.info & 0xF not in (P.STT_SECTION, P.STT_FILE) \
                    and s.name in linked_defs and where[s.shndx] + s.value not in linked_defs[s.name]:
                raise ProofError("placement", f"{s.name} is linked at {hx(linked_defs[s.name][0])}, the object's layout "
                                              f"puts it at {hx(where[s.shndx] + s.value)}")
        prev = code_lo
        for sec, addr in place:   # the linked code is the object's code with only relocation fields filled in
            if any(at(prev, addr - prev)):
                raise ProofError("relocation", f"the linker fill before {hx(addr)} is not zero")
            bad = P.masked_difference(o.contents(sec), at(addr, sec.size), o.relocations(sec))
            if bad is not None:
                raise ProofError("relocation", f"the linked code differs from the object outside relocation fields "
                                               f"at {hx(addr + bad)}")
            prev = addr + sec.size
        ours = at(start, span)
        retail = P.image_range(h["module"], start, end, self.target, executable=True)
        if ours != retail:
            n = sum(1 for i in range(0, span, 4) if ours[i:i + 4] != retail[i:i + 4])
            raise ProofError("mismatch", f"independent comparison: {n} of {span // 4} words differ from retail")
        compared = {".text": {"addr": hx(start), "size": span, "sha256": P.sha256(ours)}}
        ranges = [(start, start + max(fsize, span))]
        refs = [(addr + r.offset, r.sym) for sec, addr in place for r in o.relocations(sec)]
        self.compare_data(h, o, ln, compared)
        placed_syms = self.compare_linkonce(h, o, ln, compared, symmap, refs, ranges)
        return compared, self.check_symbols(o, syms_ld, symmap, refs, ranges), placed_syms

    def compare_placed(self, c: Candidate, obj: bytes, linked: bytes, syms_ld: str, symmap: dict) -> tuple[dict, list, list]:
        """A helpers= source (tools/lbpmatch.py): every function of the object is the header symbol or a listed helper,
        alone at the start of its own .text section, linked once at its own retail address; the linked code there is the
        OBJECT's section (not the renamed copy the matcher linked) with only relocation fields filled in; and, followed
        by the linker's zero fill up to the end of its range, it equals the retail bytes. Each helper is recorded in
        `compared` as `.text:<name>`; the claimed range stays the target's."""
        h = c.header
        o, ln = P.Elf(obj, "object"), P.Elf(linked, "linked image")
        texts = {s.index: s for s in o.named(".text", P.SHT_PROGBITS)}
        syms = o.symbols()
        linked_defs = self.linked_defs(ln)
        compared: dict = {}
        used: dict[int, str] = {}
        refs, ranges = [], []
        for name, start, end in M.placed_functions(h):
            span = end - start
            hits = [s for s in syms if s.name == name and s.shndx in texts]
            if len(hits) != 1:
                raise ProofError("symbol", f"{name} is not defined once in a .text section of the object")
            sec = texts[hits[0].shndx]
            if hits[0].value != 0 or sec.index in used:
                raise ProofError("placement", f"{name} is not alone at the start of its own .text section")
            used[sec.index] = name
            if sec.size > span:
                raise ProofError("range", f"{name} is {sec.size} bytes, longer than its range of {span}")
            if linked_defs.get(name) != [start]:
                raise ProofError("placement", f"{name} is not linked (once) at its retail address {hx(start)}")
            lsec = next((s for s in ln.sections if s.flags & P.SHF_ALLOC and s.type == P.SHT_PROGBITS
                         and s.addr <= start and start + sec.size <= s.addr + s.size), None)
            if lsec is None:
                raise ProofError("placement", f"no linked section holds {name} at {hx(start)}")
            code = ln.contents(lsec)[start - lsec.addr:start - lsec.addr + sec.size]
            bad = P.masked_difference(o.contents(sec), code, o.relocations(sec))
            if bad is not None:
                raise ProofError("relocation", f"the linked code differs from the object outside relocation fields "
                                               f"at {hx(start + bad)}")
            for s in syms:   # every named symbol of the section sits where the placement puts it
                if s.shndx == sec.index and s.name and s.info & 0xF not in (P.STT_SECTION, P.STT_FILE) \
                        and s.name in linked_defs and start + s.value not in linked_defs[s.name]:
                    raise ProofError("placement", f"{s.name} is linked at {hx(linked_defs[s.name][0])}, its function's "
                                                  f"placement puts it at {hx(start + s.value)}")
            ours = code + bytes(span - sec.size)
            retail = P.image_range(h["module"], start, end, self.target, executable=True)
            if ours != retail:
                n = sum(1 for i in range(0, span, 4) if ours[i:i + 4] != retail[i:i + 4])
                what = "" if name == h["symbol"] else f" (helper {name})"
                raise ProofError("mismatch", f"independent comparison{what}: {n} of {span // 4} words differ from retail")
            compared[".text" if name == h["symbol"] else f".text:{name}"] = {"addr": hx(start), "size": span,
                                                                             "sha256": P.sha256(ours)}
            refs += [(start + r.offset, r.sym) for r in o.relocations(sec)]
            ranges.append((start, end))
        rest = [s for i, s in texts.items() if s.size and i not in used]
        if rest:
            raise ProofError("placement", f"the object has {len(rest)} function section(s) that are neither the target "
                                          "nor a listed helper")
        self.compare_data(h, o, ln, compared)
        placed_syms = self.compare_linkonce(h, o, ln, compared, symmap, refs, ranges)
        return compared, self.check_symbols(o, syms_ld, symmap, refs, ranges), placed_syms

    def compare_data(self, h: dict, o: P.Elf, ln: P.Elf, compared: dict) -> None:
        """Each placed .rodata/.data/.sdata (an input of that name with contents, and the header key, as the matcher
        places them): the linked output section at the header address holds the object's inputs of that output (file
        order, aligned) with only relocation fields filled in and the linker's zero fill between them, and equals the
        retail bytes; recorded in `compared`. A placed .bss/.sbss is linked at its header address."""
        for key in PLACED:
            if not any(s.size for s in o.named("." + key, P.SHT_PROGBITS)) or key not in h:
                continue       # absent, empty, or left unplaced (then it is not compared and not claimed)
            addr = int(h[key], 16)
            outs = ln.named("." + key)
            if len(outs) != 1 or outs[0].addr != addr:
                raise ProofError("placement", f".{key} is not linked at {hx(addr)}")
            ambiguous = [s.name for s in o.sections if AMBIGUOUS_DATA.fullmatch(s.name)]
            if ambiguous:
                raise ProofError("placement", f"data sections this check does not lay out: {', '.join(ambiguous)}")
            inputs = [s for s in o.sections if s.type == P.SHT_PROGBITS and OUTPUT_INPUTS[key].fullmatch(s.name)]
            lb = ln.contents(outs[0])
            prev = addr
            for sec, a in o.layout(inputs, addr):
                if any(lb[prev - addr:a - addr]):
                    raise ProofError("relocation", f"the linker fill before {hx(a)} in .{key} is not zero")
                bad = P.masked_difference(o.contents(sec), lb[a - addr:a - addr + sec.size], o.relocations(sec))
                if bad is not None:
                    raise ProofError("relocation", f"the linked .{key} differs from the object ({sec.name}) outside "
                                                   "relocation fields")
                prev = a + sec.size
            if prev - addr != len(lb):
                raise ProofError("placement", f"the linked .{key} ({len(lb)} bytes) is not the object's inputs "
                                              f"({prev - addr} bytes)")
            if lb != P.image_range(h["module"], addr, addr + len(lb), self.target, executable=False):
                raise ProofError("section-mismatch", f".{key} ({len(lb)} bytes at {hx(addr)}) differs from retail")
            compared["." + key] = {"addr": hx(addr), "size": len(lb), "sha256": P.sha256(lb)}
        for key in NOBITS_PLACED:
            if not any(s.size for s in o.named("." + key, P.SHT_NOBITS)) or key not in h:
                continue
            addr = int(h[key], 16)
            outs = ln.named("." + key)
            if len(outs) != 1 or outs[0].addr != addr:
                raise ProofError("placement", f".{key} is not linked at {hx(addr)}")

    def compare_linkonce(self, h: dict, o: P.Elf, ln: P.Elf, compared: dict, symmap: dict, refs: list,
                         ranges: list[tuple[int, int]]) -> list:
        """`.gnu.linkonce.<k>.<symbol>` data objects (tools/lbpmatch.py renames each one it places to .lbp_loN and links
        it at the symbol's address from the symbol map). Every placed output section is matched to the one object input
        of its size whose symbol the map gives that address; its linked bytes are the input's with only relocation
        fields filled in and equal retail (recorded in `compared` as `linkonce:<section>`); every linkonce object the
        compared code refers to must be one of them. Returns [symbol, address] for the dependency record."""
        inputs = [s for s in o.sections if s.size and (m := LINKONCE_DATA.fullmatch(s.name))]
        outs = [s for s in ln.sections if PLACED_LINKONCE.fullmatch(s.name)]
        placed: dict[int, tuple[str, int]] = {}
        for out in outs:
            hits = [s for s in inputs if s.size == out.size and s.type == out.type and s.index not in placed
                    and symmap.get(LINKONCE_DATA.fullmatch(s.name)["symbol"]) == out.addr]
            if len(hits) != 1:
                raise ProofError("placement", f"the linked {out.name} at {hx(out.addr)} is not one linkonce object of the "
                                              "object at its symbol-map address")
            sec = hits[0]
            sym = LINKONCE_DATA.fullmatch(sec.name)["symbol"]
            placed[sec.index] = (sym, out.addr)
            if sec.type == P.SHT_NOBITS:
                continue
            lb = ln.contents(out)
            bad = P.masked_difference(o.contents(sec), lb, o.relocations(sec))
            if bad is not None:
                raise ProofError("relocation", f"the linked {sec.name} differs from the object outside relocation fields")
            if lb != P.image_range(h["module"], out.addr, out.addr + len(lb), self.target, executable=False):
                raise ProofError("section-mismatch", f"{sec.name} ({len(lb)} bytes at {hx(out.addr)}) differs from retail")
            compared[f"linkonce:{sec.name}"] = {"addr": hx(out.addr), "size": len(lb), "sha256": P.sha256(lb)}
        syms = o.symbols()
        linkonce_index = {s.index for s in inputs}
        for addr, sym in refs:
            if any(lo <= addr < hi for lo, hi in ranges) and sym < len(syms) and syms[sym].shndx in linkonce_index \
                    and syms[sym].shndx not in placed:
                raise ProofError("placement", f"the compared code refers to {o.sections[syms[sym].shndx].name}, which "
                                              "is not linked at its symbol-map address")
        return sorted([sym, hx(addr)] for sym, addr in placed.values())

    def check_symbols(self, o: P.Elf, syms_ld: str, symmap: dict, refs: list, ranges: list[tuple[int, int]]) -> list:
        """`ranges`: the compared code ([lo, hi) linked addresses); a placeholder is allowed only for a name no
        relocation inside them uses."""
        syms = o.symbols()
        undefined = sorted({s.name for s in syms if s.shndx == 0 and s.name})
        defs: dict[str, tuple[int, bool]] = {}
        for line in syms_ld.splitlines():
            if not line.strip():
                continue
            name, sep, rest = line.partition(" = ")
            value, _, tail = rest.partition(";")
            if not sep or not P.HEX8.fullmatch(value) or tail.strip() not in ("", PLACEHOLDER) or name in defs:
                raise ProofError("symbols", f"unexpected line in the link script: {line[:80]}")
            defs[name] = (int(value, 16), tail.strip() == PLACEHOLDER)
        if sorted(defs) != undefined:
            raise ProofError("symbols", "the link script defines other names than the object's undefined ones")
        referenced = {syms[sym].name for addr, sym in refs if any(lo <= addr < hi for lo, hi in ranges)
                      and sym < len(syms) and syms[sym].shndx == 0}
        out = []
        for name in undefined:
            value, placeholder = defs[name]
            want = P.resolve(name, symmap)
            if placeholder:
                if value != 0 or want is not None or name in referenced:
                    raise ProofError("symbols", f"{name} was linked as a placeholder although "
                                                + ("it has an address" if want is not None else "the compared code uses it"))
                out.append([name, None])
            else:
                if want is None or want != value:
                    raise ProofError("symbols", f"{name} was linked at {hx(value)}, the symbol map gives "
                                                f"{hx(want) if want is not None else 'nothing'}")
                out.append([name, hx(value)])
        return out

    def make_proof(self, c: Candidate, row: dict | None, own_obj: Path, symmaps: dict) -> dict:
        h = c.header
        if row is None:
            raise ProofError("matcher", "tools/lbpmatch.py produced no result for this source (see the run's match log)")
        if row.get("status") != "EXACT":
            raise ProofError("mismatch", f"tools/lbpmatch.py: {row.get('status')} {row.get('detail', '')}".strip())
        if row.get("module") != h["module"] or row.get("start") != hx(h["start"]):
            raise ProofError("matcher", "the matcher's result names another function")
        art = self.build_dir(c)
        blobs = {}
        for name in ("obj.o", "linked.elf", "syms.ld"):
            f = art / name
            if not f.is_file():
                raise ProofError("artifact", f"the matcher left no {name}")
            if f.stat().st_mtime < self.started - 2:
                raise ProofError("stale-artifact", f"{name} predates this verification run")
            blobs[name] = f.read_bytes()
        if not own_obj.is_file():
            log = own_obj.with_name("compile.log")
            tail = log.read_text(errors="replace")[-300:] if log.exists() else ""
            raise ProofError("own-compile", f"the recorded profile produced no object: {tail.strip()}")
        # byte-identical, except GCC's random-seed digits in `_GLOBAL__I_<file>_..._<random>` names (lbpproof.py)
        normal = P.normalized_object(blobs["obj.o"], "the matcher's object")
        if normal != P.normalized_object(own_obj.read_bytes(), "the recorded profile's object"):
            raise ProofError("object", "the matcher's object differs from the one the recorded profile produces")
        symmap = symmaps.setdefault(h["module"], P.symbol_map(h["module"], self.root))
        compared, symbols, placed = self.compare(c, blobs["obj.o"], blobs["linked.elf"], blobs["syms.ld"].decode("utf-8"),
                                                 symmap)
        closure = P.include_closure(c.path, self.root)
        deps = P.deps_record(c.sha, closure, symbols, placed)
        findings = lbpgate.check_file(c.path, self.root)
        if lbpgate.refusals(findings):
            raise ProofError("gate", f"fails the fake-match gate: {lbpgate.summary(findings)}")
        if P.file_sha256(c.path) != c.sha:
            raise ProofError("source-changed", "the source changed during the verification run")
        proof = {"schema": P.PROOF_SCHEMA, "module": h["module"], "start": hx(h["start"]), "end": hx(h["end"]),
                 "symbol": h["symbol"], "source": c.rel, "source_sha256": c.sha, "cc": self.cc(h),
                 "lang": h["lang"], "deps": deps, "deps_sha256": P.digest(deps), "toolchain": self.toolchain_digest,
                 "target": self.target["target_digest"], "compared": compared,
                 "object_sha256": P.sha256(normal), "status": "EXACT",
                 "gate": {"result": "pass", "idioms": sorted({f.kind for f in findings if f.severity == lbpgate.IDIOM})},
                 "run": self.id, "verified_utc": P.utc_now()}
        proof["proof_sha256"] = P.proof_digest(proof)
        return proof

    def verify(self, cands: list[Candidate]) -> tuple[dict[str, dict], dict[str, str]]:
        proofs: dict[str, dict] = {}
        refused: dict[str, str] = {}
        ok: list[Candidate] = []
        for c in cands:
            try:
                self.precheck(c)
                ok.append(c)
            except ProofError as e:
                refused[c.rel] = str(e)
        dirs = Counter((c.header["module"], self.build_dir(c)) for c in ok)
        for c in [c for c in ok if dirs[(c.header["module"], self.build_dir(c))] > 1]:
            refused[c.rel] = "artifact: another candidate shares its build directory name (same directory and stem)"
        ok = [c for c in ok if c.rel not in refused]
        if not ok:
            return proofs, refused
        say(f"verification run {self.id}: {len(ok)} source(s) ({len(refused)} refused before compiling)")
        rows = self.run_matcher(ok)
        own = self.run_own_compile(ok)
        symmaps: dict = {}
        for c in ok:
            try:
                proofs[c.rel] = self.make_proof(c, rows.get(c.rel), own[c.rel], symmaps)
            except ProofError as e:
                refused[c.rel] = str(e)
        return proofs, refused

    def finish(self, success: bool) -> None:
        if success and not self.keep:
            shutil.rmtree(self.dir, ignore_errors=True)
        else:
            say(f"run directory kept: {self.dir}")


def new_run(ctx: Context, args, core: dict | None = None, toolchain_digest: str | None = None) -> Run:
    return Run(ctx.root, core or ctx.tool["core"], toolchain_digest or ctx.tool["core_digest"], ctx.target,
               ctx.inventories, args.parallel, args.keep_run)


# --------------------------------------------------------------------------------------------------------------
# helpers

def overlaps(a: dict, b: dict) -> bool:
    am, as_, ae = P.proof_range(a)
    bm, bs, be = P.proof_range(b)
    return am == bm and as_ < be and bs < ae


class Owners:
    """Disjoint owned ranges per module, sorted, with an overlap lookup by bisection (owners never overlap, so their
    ends rise with their starts)."""

    def __init__(self, proofs=()):
        self.starts: dict[str, list[int]] = {}
        self.items: dict[str, list[tuple[int, int, dict]]] = {}
        for p in proofs:
            self.add(p)

    def clashes(self, p: dict) -> list[dict]:
        m, s, e = P.proof_range(p)
        st, it = self.starts.get(m, []), self.items.get(m, [])
        j = bisect.bisect_left(st, e) - 1
        out = []
        while j >= 0 and it[j][1] > s:
            out.append(it[j][2])
            j -= 1
        return out

    def add(self, p: dict) -> None:
        m, s, e = P.proof_range(p)
        st, it = self.starts.setdefault(m, []), self.items.setdefault(m, [])
        i = bisect.bisect_left(st, s)
        st.insert(i, s)
        it.insert(i, (s, e, p))

    def all(self) -> list[dict]:
        return [p for m in sorted(self.items) for _s, _e, p in self.items[m]]


def lost_ranges(old: list[dict], new: list[dict]) -> list[tuple[str, int, int]]:
    out = []
    for module in sorted({p["module"] for p in old}):
        was = [P.proof_range(p)[1:] for p in old if p["module"] == module]
        now = [P.proof_range(p)[1:] for p in new if p["module"] == module]
        out += [(module, s, e) for s, e in P.subtract(was, now)]
    return out


def read_paths(args, root: Path) -> list[str] | None:
    """The named sources; None when nothing was named (then: every unproven source). An empty --paths-from file names
    nothing, so only the accepted proofs that no longer hold are reverified."""
    names = list(args.paths or [])
    if args.paths_from:
        names += [ln.strip() for ln in Path(args.paths_from).read_text(encoding="utf-8").splitlines() if ln.strip()]
    elif not names:
        return None
    out = []
    for n in names:
        p = Path(n)
        p = p if p.is_absolute() else root / p
        try:
            out.append(P.rel(p, root))
        except ValueError:
            out.append(n)
    return out


def emit(args, doc: dict) -> None:
    if getattr(args, "json", None):
        Path(args.json).write_text(json.dumps(doc, indent=1, sort_keys=True) + "\n", encoding="utf-8")


def refuse(args, reason: str, **extra) -> int:
    say(f"REFUSED: {reason}")
    say("the accepted ledger is unchanged")
    emit(args, {"command": args.command, "status": "refused", "reason": reason, **extra})
    return 1


def print_refusals(refused: dict[str, str], limit: int = 60) -> None:
    for i, (r, why) in enumerate(sorted(refused.items())):
        if i == limit:
            say(f"  ... and {len(refused) - limit} more")
            break
        say(f"  not promoted: {r}: {why}")


# --------------------------------------------------------------------------------------------------------------
# commands

def cmd_lock(args) -> int:
    root = ROOT
    data = root / "data"
    with PromoteLock(root, args.lock_wait):
        tpath, cpath = data / P.TARGET_PATH.name, data / P.TOOLCHAIN_PATH.name
        if tpath.exists() or cpath.exists():
            return refuse(args, "the locks exist; change them with `retarget` or `migrate-toolchain`")
        target = P.live_target(root)
        tdig = P.digest(target)
        core = P.live_toolchain(root, P.used_profiles(root))
        cdig = P.digest(core)
        now = P.utc_now()
        P.write_json(tpath, {**target, "target_digest": tdig,
                             "history": [{"utc": now, "event": "lock", "reason": args.reason, "digest": tdig}]})
        P.write_json(cpath, {"schema": P.TOOLCHAIN_SCHEMA, "core": core, "core_digest": cdig,
                             "symbol_maps": "bound per proof: each proof records the address every undefined name it "
                                            "links resolved to (and every linkonce object it places); a proof whose "
                                            "resolutions change is reverified",
                             "history": [{"utc": now, "event": "lock", "reason": args.reason, "digest": cdig,
                                          "profiles": sorted(core["profiles"])}]})
        say(f"target lock    {tdig}\ntoolchain lock {cdig} (profiles {', '.join(sorted(core['profiles']))}; primary "
            f"{core['primary']})")
        return 0


def ownership(proofs: dict[str, dict]) -> tuple[list[dict], dict[str, str], dict[str, str]]:
    """Seed ownership: larger ranges first; a range inside an owner is subsumed; a partial overlap is refused."""
    owners = Owners()
    subsumed, refused = {}, {}
    for r, p in sorted(proofs.items(), key=lambda kv: (kv[1]["module"], -(unhx(kv[1]["end"]) - unhx(kv[1]["start"])),
                                                      unhx(kv[1]["start"]), kv[0])):
        clash = owners.clashes(p)
        if not clash:
            owners.add(p)
            continue
        _m, s, e = P.proof_range(p)
        holder = next((o for o in clash if unhx(o["start"]) <= s and e <= unhx(o["end"])), None)
        if holder is not None:
            subsumed[r] = holder["source"]
        else:
            refused[r] = "overlap: partially overlaps " + ", ".join(o["source"] for o in clash)
    return owners.all(), subsumed, refused


def cmd_seed(args) -> int:
    root = ROOT
    with PromoteLock(root, args.lock_wait):
        try:
            ctx = Context(root, need_ledger=False)
        except ProofError as e:
            return refuse(args, str(e))
        if ctx.ledger is not None or ctx.journal_path.exists():
            return refuse(args, "a ledger exists; the seed runs only once (use promote)")
        tree = P.tree_sources(root)
        baseline, base_doc = None, None
        if args.baseline:
            raw = Path(args.baseline).read_bytes()
            baseline = {row["source"]: row["status"] for row in json.loads(raw)}
            base_doc = {"file": Path(args.baseline).name, "sha256": P.sha256(raw), "rows": len(baseline),
                        "exact": sum(1 for s in baseline.values() if s == "EXACT")}
        run = new_run(ctx, args)
        proofs, refused = run.verify(candidates_for(tree, tree))
        owners, subsumed, overlap = ownership(proofs)
        refused.update(overlap)
        differences = []
        if baseline is not None:
            for r in sorted(set(tree) | set(baseline)):
                b, ok = baseline.get(r), r in proofs
                if (b == "EXACT") != ok:
                    differences.append({"source": r, "baseline": b, "run": "EXACT" if ok else refused.get(r, "absent")})
        event = {"event": "seed", "run": run.id, "sources": len(tree), "proved": len(proofs), "owners": len(owners),
                 "refused": len(refused), "subsumed": dict(sorted(subsumed.items())), "baseline": base_doc,
                 "baseline_differences": len(differences)}
        ev = ctx.write(owners, event)
        report = {"command": "seed", "status": "seeded", "run": run.id, "journal_seq": ev["seq"], "owners": len(owners),
                  "refused": refused, "subsumed": subsumed, "baseline": base_doc, "baseline_differences": differences}
        if args.report:
            Path(args.report).write_text(json.dumps(report, indent=1, sort_keys=True) + "\n", encoding="utf-8")
        emit(args, report)
        say(f"seeded: {len(owners):,} proofs from {len(tree):,} sources; {len(subsumed)} subsumed, {len(refused)} refused; "
            f"baseline differences: {len(differences) if baseline is not None else 'no baseline given'}")
        for r, holder in sorted(subsumed.items()):
            say(f"  subsumed: {r} (inside {holder})")
        print_refusals(refused)
        for d in differences[:40]:
            say(f"  baseline difference: {d['source']}: baseline {d['baseline']}, run {d['run']}")
        run.finish(not refused)
        return 0 if not refused else 3


def cmd_promote(args) -> int:
    root = ROOT
    with PromoteLock(root, args.lock_wait):
        try:
            ctx = Context(root)
        except ProofError as e:
            return refuse(args, str(e))
        led = ctx.ledger
        tree = P.tree_sources(root)
        symmaps: dict = {}
        intact, stale, why = [], [], {}
        for p in led.proofs:
            problems = P.proof_problems(p, root, symmaps, ctx.inventories, ctx.primary)
            if problems or args.all:
                stale.append(p)
                why[p["source"]] = "; ".join(problems) or "--all"
            else:
                intact.append(p)
        named = read_paths(args, root)
        proven = led.by_source()
        if named is None:
            requested = {r for r in tree if r not in proven}
        else:
            requested = set(named)
            for r in sorted(requested - set(tree)):
                say(f"  not a source in src/ (skipped): {r}")
        intact_paths = {p["source"] for p in intact}
        stale_paths = {p["source"] for p in stale}
        cand = (stale_paths & set(tree)) | ((requested & set(tree)) - intact_paths)
        say(f"accepted {len(led.proofs):,}: {len(intact):,} still hold, {len(stale)} to reverify "
            f"({sum(1 for r in stale_paths if r not in tree)} with the source gone); {len(cand - stale_paths)} new candidate(s)")
        for r in sorted(stale_paths)[:20]:
            say(f"  reverify {r}: {why[r]}")
        new, refused = {}, {}
        run = None
        if cand:
            run = new_run(ctx, args)
            new, refused = run.verify(candidates_for(cand, tree))
        owned = Owners(intact)
        added, replaced = [], []
        old_by_path = {p["source"]: p for p in stale}
        for r, proof in sorted(new.items(), key=lambda kv: (kv[0] not in stale_paths, kv[1]["module"], unhx(kv[1]["start"]))):
            clash = owned.clashes(proof)
            if clash:
                refused[r] = "overlap: its range overlaps the accepted range of " + ", ".join(o["source"] for o in clash) \
                             + " (a byte has one owner)"
                continue
            owned.add(proof)
            (replaced if r in old_by_path else added).append(proof)
        owners = owned.all()
        lost = lost_ranges(led.proofs, owners)
        if lost:
            detail = []
            for module, s, e in lost:
                srcs = [p["source"] for p in led.proofs if p["module"] == module and unhx(p["start"]) < e and s < unhx(p["end"])]
                reasons = [f"{x}: {refused.get(x) or why.get(x) or 'not reverified'}" for x in srcs]
                detail.append({"module": module, "start": hx(s), "end": hx(e), "sources": srcs, "reasons": reasons})
                say(f"  LOST {module} [{hx(s)}, {hx(e)}) {e - s} bytes: " + " | ".join(reasons))
            if run:
                run.finish(False)
            return refuse(args, f"V_new does not cover V_accepted: {sum(e - s for _m, s, e in lost)} accepted bytes in "
                                f"{len(lost)} range(s) would be lost", lost=detail, refused=refused)
        if args.strict and refused:
            print_refusals(refused)
            if run:
                run.finish(False)
            return refuse(args, f"--strict: {len(refused)} candidate(s) refused", refused=refused)
        changed = bool(added or replaced) or len(owners) != len(led.proofs)
        ev = None
        if changed:
            dropped = sorted(p["source"] for p in stale if p["source"] not in {q_["source"] for q_ in replaced})
            entry = lambda p: {"module": p["module"], "start": p["start"], "end": p["end"], "source": p["source"]}  # noqa: E731
            ev = ctx.write(owners, {"event": "promote", "run": run.id if run else None,
                                    "added": [entry(p) for p in added], "replaced": [entry(p) for p in replaced],
                                    "dropped": dropped, "refused": len(refused)})
        final = {p["source"] for p in owners}
        promoted = sorted(r for r in (requested | stale_paths) if r in final)
        if args.promoted_out:
            Path(args.promoted_out).write_text("".join(r + "\n" for r in promoted), encoding="utf-8", newline="\n")
        print_refusals(refused)
        status = "promoted" if changed else "unchanged"
        say(f"{status}: {len(added)} added, {len(replaced)} re-proved, {len(refused)} refused; ledger "
            f"{len(owners):,} proofs" + (f" (journal event {ev['seq']})" if ev else ""))
        emit(args, {"command": "promote", "status": status, "run": run.id if run else None, "added": [p["source"] for p in added],
                    "replaced": [p["source"] for p in replaced], "promoted": promoted, "refused": refused,
                    "journal_seq": ev["seq"] if ev else led.header["journal_seq"]})
        if run:
            run.finish(True)
        return 3 if refused else 0


def compare_proofs(old: dict, new: dict | None, refusal: str | None) -> str | None:
    if new is None:
        return f"no longer verifies: {refusal}"
    for k in ("module", "start", "end", "symbol", "source_sha256", "object_sha256", "compared", "deps_sha256", "cc", "lang"):
        if old.get(k) != new.get(k):
            return f"{k} differs from the proof"
    return None


def cmd_verify(args) -> int:
    root = ROOT
    try:
        ctx = Context(root)
    except ProofError as e:
        return refuse(args, str(e))
    proofs = ctx.ledger.proofs
    if args.since:
        old = P.git_file(root, args.since, "data/accepted-ledger.jsonl")
        before = set()
        if old:
            before = {json.loads(x)["proof_sha256"] for x in old.split(b"\n")[1:] if x.strip()}
        proofs = [p for p in proofs if p["proof_sha256"] not in before]
    elif args.sample:
        rng = random.Random(args.seed)
        proofs = rng.sample(proofs, min(args.sample, len(proofs)))
        say(f"sample of {len(proofs)} (seed {args.seed})")
    tree = P.tree_sources(root)
    problems = {}
    symmaps: dict = {}
    for p in proofs:
        why = P.proof_problems(p, root, symmaps, ctx.inventories, ctx.primary)
        if why:
            problems[p["source"]] = "the tree no longer holds the proof: " + "; ".join(why)
    todo = [p for p in proofs if p["source"] not in problems]
    if todo:
        run = new_run(ctx, args)
        new, refused = run.verify(candidates_for({p["source"] for p in todo}, tree))
        for p in todo:
            d = compare_proofs(p, new.get(p["source"]), refused.get(p["source"]))
            if d:
                problems[p["source"]] = d
        run.finish(not problems)
    for r, d in sorted(problems.items()):
        say(f"  DIFFERENT {r}: {d}")
    say(f"verify: {len(proofs)} proof(s) checked, {len(problems)} different")
    emit(args, {"command": "verify", "status": "ok" if not problems else "different", "checked": len(proofs),
                "different": problems})
    return 1 if problems else 0


def _only_added(a: dict, b: dict) -> bool:
    """Every entry of `a` is in `b`, unchanged (`b` may have more)."""
    return all(k in b and b[k] == v for k, v in a.items())


def migration_scope(old: dict, new: dict) -> tuple[str, set[str]]:
    """'gate' when only tools/lbpgate.py changed; 'include' when only include/ changed (with or without the gate);
    'added' when compiler profiles (and the kits they run) were only added, every locked one unchanged; 'full' for
    anything else (a compiler, kit, binutils, environment, profile, primary-profile or other comparison-code change)."""
    keys = {k for k in set(old) | set(new) if old.get(k) != new.get(k)}
    a, b = old.get("comparison_code") or {}, new.get("comparison_code") or {}
    code = {f for f in set(a) | set(b) if a.get(f) != b.get(f)}
    oi, ni = old.get("include") or {}, new.get("include") or {}
    headers = {f for f in set(oi) | set(ni) if oi.get(f) != ni.get(f)}
    if keys and keys <= {"profiles", "programs"} and _only_added(old.get("profiles") or {}, new.get("profiles") or {}) \
            and _only_added(old.get("programs") or {}, new.get("programs") or {}):
        return "added", headers
    if keys - {"comparison_code", "include"} or code - {"tools/lbpgate.py"}:
        return "full", headers
    return ("include" if headers else "gate"), headers


def cmd_migrate(args) -> int:
    root = ROOT
    with PromoteLock(root, args.lock_wait):
        try:
            ctx = Context(root, check_toolchain=False)   # bound to the lock; the live toolchain is what changed
        except ProofError as e:
            return refuse(args, str(e))
        known = P.known_profiles()
        names = ({n for n in P.locked_profile_names(ctx.tool) if n in known} | P.used_profiles(root))
        try:
            live = P.live_toolchain(root, names)
        except ProofError as e:
            return refuse(args, str(e))
        new_digest = P.digest(live)
        if new_digest == ctx.tool["core_digest"]:
            say("toolchain unchanged: nothing to migrate")
            return 0
        changes = P.describe_toolchain_diff(ctx.tool["core"], live)
        scope, changed_headers = migration_scope(ctx.tool["core"], live)
        say("toolchain changes: " + "; ".join(changes) + f" (migration scope: {scope})")
        led = ctx.ledger
        symmaps: dict = {}
        holding = {p["source"]: P.proof_problems(p, root, symmaps, ctx.inventories, ctx.primary) for p in led.proofs}
        if scope == "include":   # a changed header is a change of every source that includes it (proof_problems says so)
            holding = {r: [w for w in ws if w != "included headers changed"] for r, ws in holding.items()}
        if live["primary"] != ctx.primary:   # a header without cc= now compiles with the new primary (reverified below)
            holding = {r: [w for w in ws if not w.startswith("compiler profile differs")] for r, ws in holding.items()}
        broken = {r: w for r, w in holding.items() if w}
        if broken:
            return refuse(args, f"{len(broken)} proof(s) do not hold for the tree; promote (or restore the tree) first: "
                                + "; ".join(f"{r}: {', '.join(w)}" for r, w in list(broken.items())[:5]))
        # what the change can affect: everything, except for a gate-only change (the gate verdict of every source is
        # re-decided), an include/-only change (every source whose dependency record names a changed header, or whose
        # includes are undeterminable, is reverified) and added profiles (no accepted proof uses them)
        if scope == "full":
            todo = [p for p in led.proofs]
        elif scope in ("gate", "added"):
            todo = []
        else:
            todo = [p for p in led.proofs if "include_tree_sha256" in p["deps"]
                    or any(h in changed_headers for h, _sha in p["deps"]["headers"])]
        run = new_run(ctx, args, live, new_digest)
        tree = P.tree_sources(root)
        new, refused = run.verify(candidates_for({p["source"] for p in todo}, tree)) if todo else ({}, {})
        failures = {}
        out = []
        reverified = {p["source"] for p in todo}
        for p in led.proofs:
            if p["source"] in reverified:
                q_ = new.get(p["source"])
                if q_ is None:
                    failures[p["source"]] = refused.get(p["source"], "not reverified")
                elif P.proof_range(q_) != P.proof_range(p):
                    failures[p["source"]] = "range differs"
                else:
                    out.append(q_)
                continue
            findings = lbpgate.check_file(root / p["source"], root)
            if lbpgate.refusals(findings):
                failures[p["source"]] = f"fails the changed fake-match gate: {lbpgate.summary(findings)}"
                continue
            q_ = {k: v for k, v in p.items() if k != "proof_sha256"}
            q_["toolchain"] = new_digest
            q_["gate"] = {"result": "pass", "idioms": sorted({f.kind for f in findings if f.severity == lbpgate.IDIOM})}
            q_["rebound"] = {"event": "migrate-toolchain", "scope": scope, "from": ctx.tool["core_digest"], "utc": P.utc_now()}
            q_["proof_sha256"] = P.proof_digest(q_)
            out.append(q_)
        if failures:
            for r, w in sorted(failures.items())[:60]:
                say(f"  FAILS under the new toolchain: {r}: {w}")
            if len(failures) > 60:
                say(f"  ... and {len(failures) - 60} more")
            run.finish(False)
            return refuse(args, f"{len(failures)} accepted proof(s) do not hold under the new toolchain")
        ev = P.write_ledger(out, {"event": "migrate-toolchain", "reason": args.reason, "run": run.id,
                                  "from": ctx.tool["core_digest"], "to": new_digest, "changes": changes, "scope": scope,
                                  "reverified": len(todo), "rebound": len(out) - len(todo)},
                            new_digest, ctx.target["target_digest"], ctx.ledger_path, ctx.journal_path, root)
        doc = dict(ctx.tool)
        doc["core"], doc["core_digest"] = live, new_digest
        doc["history"] = ctx.tool.get("history", []) + [{"utc": P.utc_now(), "event": "migrate-toolchain",
                                                         "reason": args.reason, "from": ctx.tool["core_digest"],
                                                         "to": new_digest, "changes": changes, "scope": scope,
                                                         "run": run.id, "reverified": len(todo),
                                                         "journal_seq": ev["seq"],
                                                         "profiles": sorted(live["profiles"])}]
        P.write_json(ctx.toolchain_path, doc)
        say(f"migrated ({scope}): {len(todo):,} proofs reverified, {len(out) - len(todo):,} rebound (gate re-run), "
            f"toolchain {new_digest}")
        run.finish(True)
        return 0


def cmd_retarget(args) -> int:
    root = ROOT
    data = root / "data"
    with PromoteLock(root, args.lock_wait):
        try:
            old = P.load_target_manifest(data / P.TARGET_PATH.name)
            tool = P.load_toolchain_manifest(data / P.TOOLCHAIN_PATH.name)
            P.verify_toolchain(tool, P.live_toolchain(root, P.locked_profile_names(tool)))
            led = P.load_ledger(data / P.LEDGER_PATH.name, data / P.JOURNAL_PATH.name)
            P.check_binding(led, tool["core_digest"], old["target_digest"])
            live = P.live_target(root)
        except ProofError as e:
            return refuse(args, str(e))
        new_digest = P.digest(live)
        if new_digest == old["target_digest"]:
            say("target unchanged: nothing to retarget")
            return 0
        changes = P.describe_target_diff(P.target_identity(old), live)
        say("target changes: " + changes)
        new_manifest = {**live, "target_digest": new_digest}
        failures = {}
        rebound = []
        inv: dict = {}
        for p in led.proofs:
            module, s, e = P.proof_range(p)
            try:
                starts, ends = P.boundaries(inv.setdefault(module, P.live_inventory(module)))
                if s not in starts or e not in ends:
                    raise ProofError("range", "not aligned to the new inventory")
                for name, rec in p["compared"].items():
                    a = unhx(rec["addr"])
                    code = name.split(":")[0] == ".text"     # ".text:<helper>": a same-file helper (helpers=)
                    if code and name != ".text" and (a not in starts or a + rec["size"] not in ends):
                        raise ProofError("range", f"helper {name[6:]} is not aligned to the new inventory")
                    got = P.image_range(module, a, a + rec["size"], new_manifest, executable=code)
                    if P.sha256(got) != rec["sha256"]:
                        raise ProofError("mismatch", f"{name} bytes differ in the new target")
            except ProofError as err:
                failures[p["source"]] = str(err)
                continue
            q_ = {k: v for k, v in p.items() if k != "proof_sha256"}
            q_["target"] = new_digest
            q_["rebound"] = {"event": "retarget", "from": old["target_digest"], "utc": P.utc_now()}
            q_["proof_sha256"] = P.proof_digest(q_)
            rebound.append(q_)
        if failures:
            for r, w in sorted(failures.items())[:60]:
                say(f"  does not hold for the new target: {r}: {w}")
            return refuse(args, f"{len(failures)} accepted proof(s) do not hold for the new target")
        ev = P.write_ledger(rebound, {"event": "retarget", "reason": args.reason, "from": old["target_digest"],
                                      "to": new_digest, "changes": changes},
                            tool["core_digest"], new_digest, data / P.LEDGER_PATH.name, data / P.JOURNAL_PATH.name, root)
        new_manifest["history"] = old.get("history", []) + [{"utc": P.utc_now(), "event": "retarget", "reason": args.reason,
                                                             "from": old["target_digest"], "to": new_digest,
                                                             "changes": changes, "journal_seq": ev["seq"]}]
        P.write_json(data / P.TARGET_PATH.name, new_manifest)
        say(f"retargeted: {len(rebound):,} proofs rebound to {new_digest}")
        return 0


def cmd_retract(args) -> int:
    root = ROOT
    with PromoteLock(root, args.lock_wait):
        try:
            ctx = Context(root)
        except ProofError as e:
            return refuse(args, str(e))
        start = int(args.start, 16)
        hit = [p for p in ctx.ledger.proofs if p["module"] == args.module and unhx(p["start"]) == start]
        if not hit:
            return refuse(args, f"no accepted proof starts at {hx(start)} in {args.module}")
        p = hit[0]
        _m, s, e = P.proof_range(p)
        say(f"RETRACTING {p['source']} [{p['start']}, {p['end']}): {e - s} accepted bytes leave the ledger; reason: {args.reason}")
        ev = ctx.write([q_ for q_ in ctx.ledger.proofs if q_ is not p],
                       {"event": "retract", "reason": args.reason,
                        "removed": [{"module": p["module"], "start": p["start"], "end": p["end"], "source": p["source"],
                                     "proof_sha256": p["proof_sha256"]}]})
        say(f"retracted (journal event {ev['seq']})")
        return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)

    def common(sp):
        sp.add_argument("--parallel", type=int, default=4, help="concurrent compiles (default 4)")
        sp.add_argument("--keep-run", action="store_true", help="keep build/proof/<run>/ after a successful run")
        sp.add_argument("--lock-wait", type=float, default=3600, help="seconds to wait for try/promote.lock")
        sp.add_argument("--json", type=Path, help="write the result as JSON")

    lk = sub.add_parser("lock")
    lk.add_argument("--reason", required=True)
    common(lk)
    sd = sub.add_parser("seed")
    sd.add_argument("--baseline", type=Path)
    sd.add_argument("--report", type=Path)
    common(sd)
    pr = sub.add_parser("promote")
    pr.add_argument("paths", nargs="*")
    pr.add_argument("--paths-from", type=Path)
    pr.add_argument("--all", action="store_true", help="reverify every accepted proof as well")
    pr.add_argument("--strict", action="store_true", help="refuse when any candidate is refused")
    pr.add_argument("--promoted-out", type=Path)
    common(pr)
    vf = sub.add_parser("verify")
    vf.add_argument("--since", help="only proofs that differ from the ledger at this git revision")
    vf.add_argument("--sample", type=int)
    vf.add_argument("--seed", type=int, default=int(time.time()) % 100000)
    common(vf)
    mg = sub.add_parser("migrate-toolchain")
    mg.add_argument("--reason", required=True)
    common(mg)
    rt = sub.add_parser("retarget")
    rt.add_argument("--reason", required=True)
    common(rt)
    rr = sub.add_parser("retract")
    rr.add_argument("module")
    rr.add_argument("start")
    rr.add_argument("--reason", required=True)
    common(rr)
    args = ap.parse_args()
    if getattr(args, "reason", "x") is not None and not str(getattr(args, "reason", "x")).strip():
        ap.error("--reason must say why")
    commands = {"lock": cmd_lock, "seed": cmd_seed, "promote": cmd_promote, "verify": cmd_verify,
                "migrate-toolchain": cmd_migrate, "retarget": cmd_retarget, "retract": cmd_retract}
    try:
        return commands[args.command](args)
    except ProofError as e:
        return refuse(args, str(e))


if __name__ == "__main__":
    raise SystemExit(main())
