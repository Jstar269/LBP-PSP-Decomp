#!/usr/bin/env python3
"""Mutation suite for the promotion gate (tools/lbppromote.py, tools/lbpproof.py); ported from hst-decomp's
tools/test_hstpromote.py.

Every sandbox test works on a DISPOSABLE SANDBOX, never on the real tree: a temporary directory with copies of the
comparison code, the symbol maps, include/, the data files, a reduced copy of the campaign ledger (the same modules,
segments and functions; the retail modules are read from the configured campaign, read-only) and a few real matched
sources (one per compiler kind: Sony psp-gcc 1.7.1, GCC 4.1.0s with rodata=, GCC 4.0.4x with bss= and GCC's
random-seed static-init names, a header without cc= (the primary profile), a C++ source whose inline function's
local static (.gnu.linkonce.b) is placed at its symbol-map address, SNC run natively), its own target and
toolchain locks and its own ledger, seeded by a real verification run. Each mutation must be REFUSED and must leave
the ledger and the journal byte-identical; the positive controls show that the gate still promotes genuine work, so a
gate that refused everything would fail them. (hst-decomp's helpers= cases need an mwcc profile, which no LBP source
uses; they are not ported.)

    python tools/test_lbppromote.py [-v] [-k PATTERN]

The sandbox tests need lbp-config.json (toolchain, campaign) and WSL; they are skipped without them. The UnitChecks
need neither. Takes several minutes (each case hashes the toolchain and most compile).
"""
from __future__ import annotations

import json
import os
import re
import shutil
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

REAL = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REAL / "tools"))
import lbpproof as P  # noqa: E402

TOOL_FILES = ("lbpmatch.py", "lbpgate.py", "lbpconfig.py", "lbpproof.py", "lbppromote.py", "lbplane.py", "lbplint.py")
G171 = "src/eboot/FUN_08804f24.c"                   # Sony psp-gcc 1.7.1 (-B), 8 bytes
SNC = "src/eboot/FUN_08b1526c.c"                    # SNC (native), an address-named call
ROD = "src/eboot/FUN_088827cc.c"                    # GCC 4.1.0s with rodata= (a string)
BSS = "src/eboot/static_init_088ad9a0.cpp"          # GCC 4.0.4x C++, bss= and rodata=, random-seed _GLOBAL__I_ names
STATIC = "src/eboot/static_init_088747dc.cpp"       # GCC 4.0.4x C++ static initialiser (random-seed names)
NOCC = "src/eboot/vcall_slot3_0894c3f8.cpp"         # no cc=: the primary profile
LINKONCE = "src/eboot/map_insert_08805418.cpp"      # a .gnu.linkonce.b object placed at its symbol-map address
SEEDED = (G171, SNC, ROD, BSS, STATIC, NOCC, LINKONCE)
NEW = "src/eboot/FUN_08804f1c.c"                    # genuine, not seeded (the function before G171): promoted by controls
NEW_BIG = "src/eboot/FUN_08b15b58.c"                # genuine SNC source, not seeded
ALT_PRIMARY = "gcc404xi-Os"                         # a profile that also matches NOCC (and is not in the sandbox lock)


def _rmtree(path: Path) -> None:
    def onexc(func, p, _exc):
        os.chmod(p, stat.S_IWRITE)
        func(p)
    shutil.rmtree(path, onexc=onexc)


def have_toolchain() -> bool:
    try:
        import lbpmatch as M
        M.campaign_root()
        M.gcc_home()
        M.gcc4_home()
        M.snc_exe()
        return (REAL / "lbp-config.json").exists() and shutil.which("wsl.exe") is not None
    except BaseException:  # noqa: BLE001 - lbpconfig raises SystemExit when unset
        return False


class Sandbox:
    def __init__(self, root: Path):
        self.root = root

    def run(self, tool: str, *args: str, env: dict | None = None) -> subprocess.CompletedProcess:
        e = dict(os.environ)
        e.update(env or {})
        return subprocess.run([sys.executable, str(self.root / "tools" / tool), *args], cwd=self.root,
                              capture_output=True, text=True, env=e)

    def gate(self, *args: str, env: dict | None = None) -> subprocess.CompletedProcess:
        return self.run("lbppromote.py", *args, env=env)

    def promote(self, *args: str, env: dict | None = None) -> subprocess.CompletedProcess:
        return self.run("lbppromote.py", "promote", *args, env=env)

    def check(self, *args: str, env: dict | None = None) -> subprocess.CompletedProcess:
        return self.run("lbpproof.py", "check", *args, env=env)

    def state(self) -> tuple[bytes, bytes]:
        return (self.root / "data" / "accepted-ledger.jsonl").read_bytes(), \
            (self.root / "data" / "accepted-journal.jsonl").read_bytes()

    def text(self, rel: str) -> str:
        return (self.root / rel).read_text(encoding="utf-8")

    def write(self, rel: str, text: str) -> None:
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_bytes(text.encode("utf-8"))

    def edit(self, rel: str, old: str, new: str, count: int = 1) -> None:
        t = self.text(rel)
        assert old in t, f"{old!r} not in {rel}"
        self.write(rel, t.replace(old, new, count))

    def git(self, *args: str) -> str:
        return subprocess.run(["git", "-c", "user.name=test", "-c", "user.email=test@invalid", *args], cwd=self.root,
                              capture_output=True, text=True, check=True).stdout

    def config(self, **changes: str) -> None:
        cfg = json.loads((self.root / "lbp-config.json").read_text(encoding="utf-8"))
        cfg.update(changes)
        (self.root / "lbp-config.json").write_text(json.dumps(cfg, indent=1), encoding="utf-8")

    def proofs(self) -> list[dict]:
        return [json.loads(x) for x in (self.root / "data" / "accepted-ledger.jsonl").read_text().splitlines()[1:]]

    def py(self, code: str) -> subprocess.CompletedProcess:
        """Run Python with the sandbox's tools first on the path (for checks below the CLI)."""
        return subprocess.run([sys.executable, "-c", f"import sys; sys.path.insert(0, {str(self.root / 'tools')!r})\n" + code],
                              cwd=self.root, capture_output=True, text=True)


def minimal_campaign(dest: Path) -> None:
    """The campaign files the tools read, reduced to the fields they use (the same modules, segments, functions)."""
    import lbpmatch as M
    src = M.campaign_root()
    led = json.loads((src / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
    small = {"modules": [{"name": m["name"], "input": m["input"], "input_sha256": m.get("input_sha256"),
                          "load_segments": m["load_segments"],
                          "functions": [{"start": f["start"], "end": f["end"], "boundary_status": f.get("boundary_status")}
                                        for f in m["functions"]]} for m in led["modules"]]}
    (dest / "ledger").mkdir(parents=True)
    (dest / "inputs").mkdir()
    (dest / "ledger" / "campaign.json").write_text(json.dumps(small), encoding="utf-8")
    sel = json.loads((src / "inputs" / "revision-selection.json").read_text(encoding="utf-8-sig"))
    sel["roots"] = {k: str((src / "inputs").resolve()) if Path(v).resolve() == (src / "inputs").resolve() else v
                    for k, v in sel["roots"].items()}
    (dest / "inputs" / "revision-selection.json").write_text(json.dumps(sel), encoding="utf-8")


BASE: Path | None = None
WORK: Path | None = None


def setUpModule() -> None:
    global BASE, WORK
    if not have_toolchain():
        return       # the sandbox classes skip themselves; UnitChecks still run
    WORK = Path(tempfile.mkdtemp(prefix="lbppromote-test-"))
    BASE = WORK / "base"
    (BASE / "tools").mkdir(parents=True)
    for f in TOOL_FILES:
        shutil.copy2(REAL / "tools" / f, BASE / "tools" / f)
    shutil.copytree(REAL / "symbols", BASE / "symbols")
    shutil.copytree(REAL / "include", BASE / "include")
    (BASE / "data").mkdir()
    for f in ["modules.json", *(p.name for p in (REAL / "data").glob("functions-*.txt"))]:
        shutil.copyfile(REAL / "data" / f, BASE / "data" / f)
    for rel in SEEDED:
        (BASE / rel).parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(REAL / rel, BASE / rel)
    minimal_campaign(BASE / "campaign")
    cfg = json.loads((REAL / "lbp-config.json").read_text(encoding="utf-8"))
    cfg["campaign_root"] = str(BASE / "campaign")
    cfg.pop("ext_root", None)
    (BASE / "lbp-config.json").write_text(json.dumps(cfg, indent=1), encoding="utf-8")
    (BASE / "try").mkdir()
    (BASE / "docs").mkdir()
    (BASE / "docs" / "LANE-NOTES.md").write_text("# Lane notes\n", encoding="utf-8")
    (BASE / ".gitignore").write_text("/build/\n/try/\n/campaign/\n/lbp-config.json\n__pycache__/\n/ext/\n", encoding="utf-8")
    (BASE / ".gitattributes").write_text("* -text\n", encoding="utf-8")
    sb = Sandbox(BASE)
    r = sb.gate("lock", "--reason", "mutation-suite sandbox")
    assert r.returncode == 0, r.stdout + r.stderr
    r = sb.gate("seed")
    assert r.returncode == 0, r.stdout + r.stderr
    sb.git("init", "-q")
    sb.git("add", "-A")
    sb.git("commit", "-qm", "seeded sandbox")


def tearDownModule() -> None:
    if WORK is not None and WORK.exists() and not os.environ.get("LBPPROMOTE_TEST_KEEP"):
        _rmtree(WORK)


class GateCase(unittest.TestCase):
    def setUp(self) -> None:
        if BASE is None:
            self.skipTest("no configured toolchain/campaign (lbp-config.json) or no WSL")
        self.dir = WORK / self.id().split(".")[-1]
        shutil.copytree(BASE, self.dir)
        self.sb = Sandbox(self.dir)
        self.sb.config(campaign_root=str(self.dir / "campaign"))   # this copy's own campaign files
        self.before = self.sb.state()

    def tearDown(self) -> None:
        if not os.environ.get("LBPPROMOTE_TEST_KEEP"):
            _rmtree(self.dir)

    # -- assertions
    def assertRefused(self, r: subprocess.CompletedProcess, *needles: str) -> None:
        out = r.stdout + r.stderr
        self.assertEqual(r.returncode, 1, out)
        self.assertIn("REFUSED", out)
        for n in needles:
            self.assertRegex(out, n)
        self.assertEqual(self.sb.state(), self.before, "a refused promotion changed the ledger or journal")

    def assertCandidateRefused(self, r: subprocess.CompletedProcess, rel: str, needle: str) -> None:
        out = r.stdout + r.stderr
        self.assertEqual(r.returncode, 3, out)
        self.assertRegex(out, rf"not promoted: {re.escape(rel)}: .*{needle}")
        self.assertNotIn(rel, {p["source"] for p in self.sb.proofs()})

    def assertCheckFails(self, *needles: str, args: tuple = ()) -> None:
        r = self.sb.check(*args)
        out = r.stdout + r.stderr
        self.assertEqual(r.returncode, 1, out)
        for n in needles:
            self.assertRegex(out, n)

    def copy(self, *rels: str) -> None:
        for rel in rels:
            shutil.copyfile(REAL / rel, self.dir / rel)


class Controls(GateCase):
    """The gate still accepts genuine work (a gate that refuses everything fails here)."""

    def test_seeded_sandbox_checks_clean(self) -> None:
        r = self.sb.check("--base", "HEAD")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        proofs = {p["source"]: p for p in self.sb.proofs()}
        self.assertEqual(sorted(proofs), sorted(SEEDED))
        self.assertEqual(proofs[NOCC]["cc"], "gcc404xs-Os")                     # the locked primary, recorded
        self.assertEqual(proofs[SNC]["cc"], "snc127-O2")
        self.assertIn(".rodata", proofs[ROD]["compared"])
        self.assertIn(["FUN_08b9eb88", "0x08b9eb88"], proofs[SNC]["deps"]["symbols"])
        self.assertEqual(proofs[LINKONCE]["deps"]["placed_symbols"], [["_ZZ9game_heapvE4heap", "0x08be84a0"]])
        lock = json.loads((self.dir / "data" / "toolchain-manifest.json").read_text())["core"]
        self.assertEqual(sorted(lock["profiles"]), sorted({"gcc171-O2", "snc127-O2", "gcc410-Os", "gcc404xs-Os"}))
        self.assertIn("snc", lock["programs"])
        self.assertIn("gcc4_home:sony-as", lock["programs"])

    def test_nothing_to_do(self) -> None:
        r = self.sb.promote(G171)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("unchanged", r.stdout)
        self.assertEqual(self.sb.state(), self.before)

    def test_new_genuine_source_is_promoted(self) -> None:
        self.copy(NEW, NEW_BIG)
        r = self.sb.promote(NEW, NEW_BIG, "--promoted-out", "try/promoted.txt")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertTrue({NEW, NEW_BIG} <= {p["source"] for p in self.sb.proofs()})
        self.assertEqual((self.dir / "try" / "promoted.txt").read_text().split(), [NEW, NEW_BIG])
        self.assertEqual(self.sb.check("--base", "HEAD").returncode, 0)
        cov = self.sb.run("lbpproof.py", "coverage", "--json", "try/cov.json")
        self.assertEqual(cov.returncode, 0, cov.stdout + cov.stderr)
        doc = json.loads((self.dir / "try" / "cov.json").read_text())
        self.assertEqual(doc["modules"]["EBOOT.BIN"]["accepted_range_bytes"],
                         sum(int(p["end"], 16) - int(p["start"], 16) for p in self.sb.proofs() if p["module"] == "EBOOT.BIN"))

    def test_verify_reproduces_every_proof(self) -> None:
        # including the GCC static initialisers, whose objects differ run to run in the random-seed digits only
        v = self.sb.gate("verify")
        self.assertEqual(v.returncode, 0, v.stdout + v.stderr)
        self.assertIn(f"{len(SEEDED)} proof(s) checked, 0 different", v.stdout)

    def test_rename_keeps_every_byte(self) -> None:
        self.sb.git("mv", ROD, "src/eboot/commtexture_088827cc.c")
        r = self.sb.promote("src/eboot/commtexture_088827cc.c")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        sources = {p["source"] for p in self.sb.proofs()}
        self.assertNotIn(ROD, sources)
        self.assertIn("src/eboot/commtexture_088827cc.c", sources)
        self.assertEqual(self.sb.check().returncode, 0)

    def test_unused_symbol_append_reverifies_nothing(self) -> None:
        with (self.dir / "symbols" / "EBOOT.BIN.txt").open("a", encoding="utf-8", newline="\n") as fh:
            fh.write("unused_name_for_the_test = 0x08801234;\n")
        r = self.sb.promote(G171)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("0 to reverify", r.stdout)

    def test_explicit_retraction_is_recorded_not_silent(self) -> None:
        r = self.sb.gate("retract", "EBOOT.BIN", "0x08804f24", "--reason", "test: the source is wrong")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        last = json.loads(self.sb.state()[1].splitlines()[-1])
        self.assertEqual((last["event"], last["reason"]), ("retract", "test: the source is wrong"))
        self.assertEqual(self.sb.check("--base", "HEAD").returncode, 0)   # a recorded retraction is allowed
        self.assertNotEqual(self.sb.gate("retract", "EBOOT.BIN", "0x08b1526c", "--reason", " ").returncode, 0)


class Wiring(GateCase):
    """The calls the lane harvest (tools/lbplane.py) and a commit check make."""

    def test_empty_paths_file_reverifies_only_stale_proofs(self) -> None:
        (self.dir / "try" / "changed.txt").write_text("")
        r = self.sb.promote("--paths-from", "try/changed.txt", "--promoted-out", "try/verified.txt")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("0 new candidate(s)", r.stdout)
        self.assertEqual((self.dir / "try" / "verified.txt").read_text(), "")

    def test_staged_check_follows_the_index(self) -> None:
        self.copy(NEW)
        self.assertEqual(self.sb.promote(NEW).returncode, 0)
        r = self.sb.check("--base", "HEAD", "--staged")      # nothing staged yet
        self.assertEqual(r.returncode, 1, r.stdout)
        self.assertIn(f"{NEW}: proven but not in the index", r.stdout)
        self.sb.git("add", NEW, "data/accepted-ledger.jsonl", "data/accepted-journal.jsonl")
        r = self.sb.check("--base", "HEAD", "--staged")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        v = self.sb.gate("verify", "--since", "HEAD")        # the newly proven source is re-run and agrees
        self.assertEqual(v.returncode, 0, v.stdout + v.stderr)
        self.assertIn("1 proof(s) checked, 0 different", v.stdout)
        with (self.dir / NEW).open("a", encoding="utf-8") as fh:
            fh.write("/* edited after staging */\n")
        self.assertCheckFails("source changed", args=("--staged",))

    def test_harvest_promotes_and_removes_what_the_gate_refuses(self) -> None:
        lane = self.dir / "ext" / "lane1"
        for d in ("src/eboot", "symbols", "wip", "docs"):
            (lane / d).mkdir(parents=True)
        (lane / "LANE.json").write_text(json.dumps({"lane": "lane1", "batch": "t.json", "items": 2}), encoding="utf-8")
        (lane / "docs" / "LANE-NOTES.md").write_text("# Lane notes\n", encoding="utf-8")
        shutil.copyfile(REAL / NEW_BIG, lane / NEW_BIG)
        # an EXACT two-function span that reaches into the accepted G171: the matcher passes it, the gate refuses it
        # (a byte has one owner), and harvest removes it from src/ again
        run = "src/eboot/FUN_08804f1c_run.c"
        (lane / run).write_text(
            "/* LBPMATCH module=EBOOT.BIN start=0x08804f1c end=0x08804f2c symbol=FUN_08804f1c lang=c99 cc=gcc171-O2 */\n"
            "/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human "
            "review. */\nvoid FUN_08804f1c(void) { }\nvoid FUN_08804f24_twin(void) { }\n", encoding="utf-8")
        self.sb.config(ext_root=str(self.dir / "ext"))
        r = self.sb.run("lbplane.py", "harvest", "lane1")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn(f"REFUSED by the promotion gate (removed from src/): {run}: overlap", r.stdout)
        self.assertFalse((self.dir / run).exists())
        self.assertIn(NEW_BIG, {p["source"] for p in self.sb.proofs()})
        self.assertIn('"promoted": 1', r.stdout)
        self.assertIn('"installed": 1', r.stdout)

    def test_harvest_with_a_global_refusal_keeps_the_sources_unproven(self) -> None:
        lane = self.dir / "ext" / "lane2"
        for d in ("src/eboot", "symbols", "wip", "docs"):
            (lane / d).mkdir(parents=True)
        (lane / "LANE.json").write_text(json.dumps({"lane": "lane2", "batch": "t.json", "items": 1}), encoding="utf-8")
        shutil.copyfile(REAL / NEW, lane / NEW)
        with (self.dir / "tools" / "lbpgate.py").open("a", encoding="utf-8") as fh:
            fh.write("# a comparison-code change: the promotion refuses everything until it is migrated\n")
        self.sb.config(ext_root=str(self.dir / "ext"))
        r = self.sb.run("lbplane.py", "harvest", "lane2")
        self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
        self.assertIn("PROMOTION REFUSED: toolchain-drift", r.stdout)
        self.assertTrue((self.dir / NEW).exists())
        self.assertEqual(self.sb.state(), self.before)


class Migrations(GateCase):
    def test_gate_only_change_reruns_the_gate_without_recompiling(self) -> None:
        with (self.dir / "tools" / "lbpgate.py").open("a", encoding="utf-8") as fh:
            fh.write("# a comment: still a change of the comparison code\n")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"tools/lbpgate\.py changed")
        r = self.sb.gate("migrate-toolchain", "--reason", "test: gate comment")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("migrated (gate): 0 proofs reverified", r.stdout)
        self.assertEqual(self.sb.check().returncode, 0)
        hist = json.loads((self.dir / "data" / "toolchain-manifest.json").read_text())["history"]
        self.assertEqual((hist[-1]["event"], hist[-1]["reason"]), ("migrate-toolchain", "test: gate comment"))

    def test_header_change_reverifies_its_dependents_only(self) -> None:
        (self.dir / "include" / "lbppromote_test.h").write_text("#define LBPPROMOTE_TEST 1\n")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", "include/lbppromote_test.h added")
        r = self.sb.gate("migrate-toolchain", "--reason", "test: a new header")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("migrated (include): 0 proofs reverified", r.stdout)
        self.assertEqual(self.sb.check().returncode, 0)

    def test_compiler_flag_change_reverifies_everything_and_refuses(self) -> None:
        self.sb.edit("tools/lbpmatch.py", 'SNC_PROFILES = {f"snc127-{o}": f"-{o}"', 'SNC_PROFILES = {f"snc127-{o}": "-O0"')
        r = self.sb.gate("migrate-toolchain", "--reason", "test: -O0")
        self.assertRefused(r, "do not hold under the new toolchain")
        self.assertIn("migration scope: full", r.stdout)
        self.assertIn(f"FAILS under the new toolchain: {SNC}", r.stdout)

    def test_a_new_profile_is_added_by_migration_without_recompiling(self) -> None:
        rel = "src/eboot/FUN_08804f1c_o1.c"
        self.sb.write(rel, (REAL / NEW).read_text(encoding="utf-8").replace("cc=gcc171-O2", "cc=gcc171-O1", 1))
        self.assertCandidateRefused(self.sb.promote(rel), rel, "not a compiler profile the toolchain lock records")
        self.assertEqual(self.sb.state(), self.before)
        c = self.sb.check()
        self.assertIn("src/ uses profile(s) the toolchain lock does not record: gcc171-O1", c.stdout)
        r = self.sb.gate("migrate-toolchain", "--reason", "test: a source uses gcc171-O1")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("migration scope: added", r.stdout)
        self.assertIn("migrated (added): 0 proofs reverified", r.stdout)
        r = self.sb.promote(rel)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn(rel, {p["source"] for p in self.sb.proofs()})

    def test_primary_change_with_a_new_profile_reverifies_every_proof(self) -> None:
        # what a measured compiler change does: tools/lbpmatch.py changes (a profile, the primary); every accepted proof
        # is reverified with the live toolchain, a header without cc= with the new primary, and all must hold
        self.sb.edit("tools/lbpmatch.py", 'PRIMARY_PROFILE = "gcc404xs-Os"', f'PRIMARY_PROFILE = "{ALT_PRIMARY}"')
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"primary profile: gcc404xs-Os -> " + ALT_PRIMARY)
        r = self.sb.gate("migrate-toolchain", "--reason", "test: a new primary profile")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("migration scope: full", r.stdout)
        self.assertIn(f"migrated (full): {len(SEEDED)} proofs reverified", r.stdout)
        proofs = {p["source"]: p for p in self.sb.proofs()}
        self.assertEqual(proofs[NOCC]["cc"], ALT_PRIMARY)
        lock = json.loads((self.dir / "data" / "toolchain-manifest.json").read_text())["core"]
        self.assertEqual(lock["primary"], ALT_PRIMARY)
        self.assertIn(ALT_PRIMARY, lock["profiles"])
        self.assertEqual(self.sb.check("--base", "HEAD").returncode, 0)

    def test_primary_change_that_breaks_a_proof_is_refused(self) -> None:
        self.sb.edit("tools/lbpmatch.py", 'PRIMARY_PROFILE = "gcc404xs-Os"', 'PRIMARY_PROFILE = "gcc404xs-O0"')
        r = self.sb.gate("migrate-toolchain", "--reason", "test: an -O0 primary")
        self.assertRefused(r, "do not hold under the new toolchain")
        self.assertIn(f"FAILS under the new toolchain: {NOCC}", r.stdout)


class SourceMutations(GateCase):
    def test_alter_one_instruction_producing_expression(self) -> None:
        self.sb.edit(SNC, "= 0x400;", "= 0x401;")
        r = self.sb.promote(G171)          # whatever is named, a changed accepted source is reverified
        self.assertRefused(r, r"LOST EBOOT\.BIN \[0x08b1526c, 0x08b152a8\)", "mismatch")
        self.assertCheckFails("source changed")

    def test_alter_a_placed_string(self) -> None:
        self.sb.edit(ROD, '"commtexture"', '"commtexturf"')
        self.assertRefused(self.sb.promote(), r"LOST EBOOT\.BIN \[0x088827cc, 0x088827d8\)", "SECTION_MISMATCH")

    def test_delete_a_matched_source(self) -> None:
        (self.dir / BSS).unlink()
        self.assertRefused(self.sb.promote(), r"LOST EBOOT\.BIN \[0x088ad9a0, 0x088ad9c8\)", "source missing")
        self.assertCheckFails("source missing")

    def test_shorten_a_header_range(self) -> None:
        self.sb.edit(SNC, "end=0x08b152a8", "end=0x08b152a4")
        self.assertRefused(self.sb.promote(SNC), r"LOST EBOOT\.BIN \[0x08b1526c, 0x08b152a8\)")

    def test_claim_a_partial_function(self) -> None:
        self.copy(NEW_BIG)
        self.sb.edit(NEW_BIG, "end=0x08b15b8c", "end=0x08b15b88")
        self.assertCandidateRefused(self.sb.promote(NEW_BIG), NEW_BIG, "partial-function claim")

    def test_add_an_extra_flag_to_an_accepted_source(self) -> None:
        self.sb.edit(G171, "cc=gcc171-O2 */", 'cc=gcc171-O2 extra="-O2" */')
        self.assertRefused(self.sb.promote(), "extra-flags")

    def test_extra_flag_on_a_new_source_never_counts(self) -> None:
        self.copy(NEW)
        self.sb.edit(NEW, " */", ' extra="-O2" */')
        self.assertCandidateRefused(self.sb.promote(NEW), NEW, "extra-flags")
        self.assertEqual(self.sb.state(), self.before)

    def test_count_a_clone_twice(self) -> None:
        shutil.copyfile(self.dir / SNC, self.dir / "src/eboot/FUN_08b1526c_again.c")
        self.assertCandidateRefused(self.sb.promote("src/eboot/FUN_08b1526c_again.c"),
                                    "src/eboot/FUN_08b1526c_again.c", "overlap")
        self.assertEqual(self.sb.state(), self.before)

    def test_instruction_emitting_asm_that_really_matches(self) -> None:
        img = self.sb.py("import lbpproof as P\nt = P.load_target_manifest()\n"
                         "print(P.image_range('EBOOT.BIN', 0x08804f1c, 0x08804f24, t, True).hex())")
        self.assertEqual(img.returncode, 0, img.stderr)
        b = bytes.fromhex(img.stdout.strip())
        w = [int.from_bytes(b[i:i + 4], "little") for i in (0, 4)]
        rel = "src/eboot/asm_08804f1c.c"
        self.sb.write(rel, "/* LBPMATCH module=EBOOT.BIN start=0x08804f1c end=0x08804f24 symbol=FUN_08804f1c lang=c99 "
                           "cc=gcc171-O2 */\n"
                           "/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human review. */\n"
                           f'__asm__(".text\\n.globl FUN_08804f1c\\nFUN_08804f1c:\\n.word 0x{w[0]:08x}\\n.word 0x{w[1]:08x}\\n");\n')
        m = self.sb.run("lbpmatch.py", rel)            # the bytes do match: only the gate stops it
        self.assertIn("EXACT_FAILS_GATE", m.stdout, m.stdout + m.stderr)
        self.assertCandidateRefused(self.sb.promote(rel), rel, "gate")
        self.assertEqual(self.sb.state(), self.before)

    def test_vfpu_asm_without_the_header_key_is_refused(self) -> None:
        rel = "src/eboot/vfpu_08804f1c.c"
        self.sb.write(rel, "/* LBPMATCH module=EBOOT.BIN start=0x08804f1c end=0x08804f24 symbol=FUN_08804f1c lang=c99 "
                           "cc=gcc171-O2 */\n/* Hand-written reconstruction from the disassembly; exact match verified; "
                           "names and types await human review. Original inline VFPU asm: test. */\n"
                           'void FUN_08804f1c(void) { __asm__ volatile ("vzero.s S000"); }\n')
        self.assertCandidateRefused(self.sb.promote(rel), rel, "gate")


THUNK_D1 = "src/eboot/obj_dtor_thunk_0886c11c.cpp"     # GCC 4.0.4x C++, -Wa,-g: thunk + D1 in .gnu.linkonce.t
THUNK_D0 = "src/eboot/obj_deldtor_thunk_0886c1b8.cpp"  # the same file's thunk + deleting destructor (D0)


class ThunkPlacement(GateCase):
    """A this-adjusting thunk and the inline destructor after it, in GCC's .gnu.linkonce.t.<D1> section, linked at the
    thunk's retail address; the class's vtable placed at its symbol-map address and not compared (2026-11-21)."""

    def setUp(self) -> None:
        super().setUp()
        self.copy(THUNK_D1, THUNK_D0)
        r = self.sb.gate("migrate-toolchain", "--reason", "test: thunk sources use gcc404xsg-Os")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("migration scope: added", r.stdout)
        self.before = self.sb.state()

    def test_thunk_and_destructor_are_promoted(self) -> None:
        r = self.sb.promote(THUNK_D1, THUNK_D0)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        proofs = {p["source"]: p for p in self.sb.proofs()}
        for rel, start, size in ((THUNK_D1, "0x0886c11c", 156), (THUNK_D0, "0x0886c1b8", 164)):
            p = proofs[rel]
            self.assertEqual((p["cc"], p["start"]), ("gcc404xsg-Os", start))
            self.assertEqual(list(p["compared"]), [".text"])                     # the vtable is not claimed
            self.assertEqual((p["compared"][".text"]["addr"], p["compared"][".text"]["size"]), (start, size))
            self.assertIn(["_ZTV12Obj_0886c09c", "0x08bb0228"], p["deps"]["placed_symbols"])
            self.assertEqual(p["deps"]["placed_symbols"], [["_ZTV12Obj_0886c09c", "0x08bb0228"]])
            self.assertIn(["_ZTV12Mid_08849d04", "0x08baec58"], p["deps"]["symbols"])   # not emitted here: external
        self.assertIn(["_ZdlPv", "0x08a9e0cc"], proofs[THUNK_D0]["deps"]["symbols"])
        self.assertEqual(self.sb.check().returncode, 0, self.sb.check().stdout)

    def test_thunk_without_the_assembler_g_is_refused(self) -> None:
        rel = "src/eboot/obj_dtor_thunk_nog_0886c11c.cpp"
        self.sb.write(rel, self.sb.text(THUNK_D1).replace("cc=gcc404xsg-Os", "cc=gcc404xs-Os", 1))
        # the thunk is `j; addiu` (8 bytes) there: the claimed range runs past its section, nothing places it
        self.assertCandidateRefused(self.sb.promote(rel), rel, "MISSING_SYMBOLS|MISMATCH")

    def test_moved_vtable_is_refused(self) -> None:
        self.sb.edit("symbols/EBOOT.BIN.txt", "_ZTV12Obj_0886c09c = 0x08bb0228;", "_ZTV12Obj_0886c09c = 0x08bb0230;")
        self.assertCandidateRefused(self.sb.promote(THUNK_D1), THUNK_D1, "MISMATCH")

    def test_unmapped_vtable_is_refused(self) -> None:
        self.sb.edit("symbols/EBOOT.BIN.txt", "_ZTV12Obj_0886c09c = 0x08bb0228;", "")
        self.assertCandidateRefused(self.sb.promote(THUNK_D1), THUNK_D1, "UNPLACED_SECTION")

    def test_claim_of_the_body_without_its_thunk_is_refused(self) -> None:
        rel = "src/eboot/obj_dtor_body_0886c128.cpp"
        self.sb.write(rel, self.sb.text(THUNK_D1).replace(
            "start=0x0886c11c end=0x0886c1b8 symbol=_ZThn12_N12Obj_0886c09cD1Ev",
            "start=0x0886c128 end=0x0886c1b8 symbol=_ZN12Obj_0886c09cD1Ev", 1))
        self.assertCandidateRefused(self.sb.promote(rel), rel, "partial-function claim")

    def test_destructor_body_changed_is_refused(self) -> None:
        rel = "src/eboot/obj_dtor_thunk_elems_0886c11c.cpp"
        self.sb.write(rel, self.sb.text(THUNK_D1).replace("Elem_0886b790 elems[16];", "Elem_0886b790 elems[15];", 1))
        self.assertCandidateRefused(self.sb.promote(rel), rel, "MISMATCH")


class SymbolMutations(GateCase):
    def test_override_an_address_name(self) -> None:
        with (self.dir / "symbols" / "EBOOT.BIN.txt").open("a", encoding="utf-8", newline="\n") as fh:
            fh.write("FUN_08b9eb88 = 0x08b9eb8c;\n")
        r = self.sb.promote(G171)
        self.assertRefused(r, r"LOST EBOOT\.BIN \[0x08b1526c, 0x08b152a8\)", "symbol resolution changed")
        self.assertCheckFails("symbol resolution changed: FUN_08b9eb88")


    def test_move_a_placed_linkonce_object(self) -> None:
        self.sb.edit("symbols/EBOOT.BIN.txt", "_ZZ9game_heapvE4heap = 0x08be84a0;", "_ZZ9game_heapvE4heap = 0x08be84a4;")
        r = self.sb.promote(G171)
        self.assertRefused(r, r"LOST EBOOT\.BIN \[0x08805418, ", "symbol resolution changed")
        self.assertCheckFails("symbol resolution changed: _ZZ9game_heapvE4heap")

    def test_drop_a_placed_linkonce_object(self) -> None:
        self.sb.edit("symbols/EBOOT.BIN.txt", "_ZZ9game_heapvE4heap = 0x08be84a0;", "")
        self.assertRefused(self.sb.promote(G171), r"LOST EBOOT\.BIN \[0x08805418, ", "UNPLACED_SECTION")


class ToolchainMutations(GateCase):
    def snc_copy(self) -> Path:
        import lbpmatch as M
        d = self.dir / "snc-copy"
        shutil.copytree(M.snc_exe().parent, d)
        with (d / "sn_autoexp.dat").open("ab") as fh:
            fh.write(b"\n")
        return d

    def test_replace_a_compiler_kit_file(self) -> None:
        d = self.snc_copy()
        self.sb.config(snc_home=str(d))
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"snc: sn_autoexp\.dat")
        self.sb.config(snc_home=str(json.loads((REAL / "lbp-config.json").read_text())["snc_home"]))
        r = self.sb.promote(G171, env={"LBP_SNC_HOME": str(d)})       # the same through the environment
        self.assertRefused(r, "toolchain-drift")

    def test_edit_the_compiler_hash_in_the_lock(self) -> None:
        path = self.dir / "data" / "toolchain-manifest.json"
        doc = json.loads(path.read_text())
        doc["core"]["programs"]["snc"]["files"]["pspsnc.exe"] = "0" * 64
        path.write_text(json.dumps(doc, indent=1, sort_keys=True) + "\n")
        self.assertRefused(self.sb.promote(G171), "toolchain-manifest", "edited")
        doc["core_digest"] = P.digest(doc["core"])      # a forger who also recomputes the digest
        path.write_text(json.dumps(doc, indent=1, sort_keys=True) + "\n")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift")

    def test_add_a_flag_to_a_compiler_profile(self) -> None:
        self.sb.edit("tools/lbpmatch.py", '" -fno-exceptions")', '" -fno-exceptions -fno-inline")')
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"tools/lbpmatch\.py changed", "gcc404xs-Os")

    def test_relax_the_fake_match_gate(self) -> None:
        self.sb.edit("tools/lbpgate.py", "    return [f for f in findings if f.severity == REFUSE]", "    return []")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"tools/lbpgate\.py changed")

    def test_matcher_that_always_returns_exact(self) -> None:
        self.sb.edit("tools/lbpmatch.py", '        summary.append({"source": rel,',
                     '        status, detail = "EXACT", ""\n        summary.append({"source": rel,')
        # 1. the lock refuses the changed comparison code
        self.assertRefused(self.sb.promote(G171), "toolchain-drift", r"tools/lbpmatch\.py changed")
        # 2. even if the lying matcher is migrated in (every accepted proof is genuine, so the migration passes) ...
        r = self.sb.gate("migrate-toolchain", "--reason", "test: an attacker re-locks a lying matcher")
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.before = self.sb.state()
        # ... a changed source that no longer matches is caught by the promoter's own byte comparison
        self.sb.edit(SNC, "= 0x400;", "= 0x401;")
        self.assertRefused(self.sb.promote(G171), "independent comparison")
        # ... and a new non-matching source is refused, not promoted
        self.sb.edit(SNC, "= 0x401;", "= 0x400;")
        self.copy(NEW_BIG)
        self.sb.edit(NEW_BIG, "puVar1[99]", "puVar1[98]")
        r = self.sb.promote(NEW_BIG)
        self.assertEqual(r.returncode, 3, r.stdout + r.stderr)
        self.assertRegex(r.stdout, f"not promoted: {NEW_BIG}: .*independent comparison")

    def test_matcher_that_substitutes_the_object(self) -> None:
        # a matcher that touches the object after compiling: its own verdict would still be EXACT, the promoter's own
        # compile of the recorded profile is not byte-identical, so not even the migration of that matcher passes
        self.sb.edit("tools/lbpmatch.py", "        obj = obj_path.read_bytes()\n",
                     "        obj_path.write_bytes(obj_path.read_bytes() + b'\\0')\n        obj = obj_path.read_bytes()\n")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift")
        r = self.sb.gate("migrate-toolchain", "--reason", "test: a matcher that alters objects")
        self.assertRefused(r, "do not hold under the new toolchain")
        self.assertIn("differs from the one the recorded profile produces", r.stdout)

    def test_stale_build_artifact_after_a_source_edit(self) -> None:
        # good artifacts in the matcher's usual build directory, then the source changes
        m = self.sb.run("lbpmatch.py", str(self.dir / SNC))
        self.assertIn("EXACT 1/1", m.stdout, m.stdout + m.stderr)
        self.sb.edit(SNC, "= 0x400;", "= 0x401;")
        # (a) the promoter never reads them: every run has a fresh directory
        self.assertRefused(self.sb.promote(G171), r"LOST EBOOT\.BIN \[0x08b1526c", "mismatch")
        # (b) a matcher changed to reuse an existing object is a toolchain change
        self.sb.edit("tools/lbpmatch.py", "        for f in out.glob(\"*\"):\n            f.unlink()\n", "")
        self.assertRefused(self.sb.promote(G171), "toolchain-drift")

    def test_stale_artifact_is_refused_below_the_cli(self) -> None:
        code = f"""
import os, shutil, time, json
import lbppromote as R, lbpproof as P
ctx = R.Context()
class A: parallel = 2; keep_run = False
first = R.new_run(ctx, A)
tree = P.tree_sources()
c = R.candidates_for([{G171!r}], tree)
proofs, refused = first.verify(c)
assert proofs, refused
old = first.build_dir(c[0])
time.sleep(3)
second = R.new_run(ctx, A)
c2 = R.candidates_for([{G171!r}], tree)
second.precheck(c2[0])
dest = second.build_dir(c2[0])
shutil.copytree(old, dest)                       # yesterday's artifacts, planted where this run would look
for f in dest.iterdir():
    os.utime(f, (time.time() - 3600, time.time() - 3600))
own = second.run_own_compile(c2)
row = {{"source": {G171!r}, "module": "EBOOT.BIN", "start": "0x08804f24", "status": "EXACT"}}
try:
    second.make_proof(c2[0], row, own[{G171!r}], {{}})
    print("ACCEPTED")
except P.ProofError as e:
    print("REFUSED", e.code)
first.finish(True); second.finish(True)
"""
        r = self.sb.py(code)
        self.assertIn("REFUSED stale-artifact", r.stdout, r.stdout + r.stderr)


class TargetMutations(GateCase):
    def test_change_the_function_inventory(self) -> None:
        path = self.dir / "data" / "functions-EBOOT.BIN.txt"
        lines = path.read_text().splitlines(keepends=True)
        path.write_text("".join(lines[:5] + lines[6:]))
        self.assertRefused(self.sb.promote(G171), "target-drift", "functions-EBOOT.BIN.txt differs")
        self.assertCheckFails("target-drift")

    def test_change_the_denominator_in_the_lock(self) -> None:
        path = self.dir / "data" / "target-manifest.json"
        doc = json.loads(path.read_text())
        doc["scope_bytes"] -= 100000
        path.write_text(json.dumps(doc, indent=1, sort_keys=True) + "\n")
        self.assertRefused(self.sb.promote(G171), "target-manifest", "edited")
        doc["target_digest"] = P.digest(P.target_identity(doc))
        path.write_text(json.dumps(doc, indent=1, sort_keys=True) + "\n")
        self.assertRefused(self.sb.promote(G171), "target-drift", "scope_bytes")

    def test_change_the_campaign_function_list(self) -> None:
        path = self.dir / "campaign" / "ledger" / "campaign.json"
        led = json.loads(path.read_text())
        mod = next(m for m in led["modules"] if m["name"] == "pspnet_ap_dialog_dummy.prx")
        mod["functions"] = mod["functions"][1:]
        path.write_text(json.dumps(led))
        self.assertRefused(self.sb.promote(G171), "target-drift", "pspnet_ap_dialog_dummy.prx")

    def test_replace_the_retail_module(self) -> None:
        sel_path = self.dir / "campaign" / "inputs" / "revision-selection.json"
        sel = json.loads(sel_path.read_text(encoding="utf-8-sig"))
        led = json.loads((self.dir / "campaign" / "ledger" / "campaign.json").read_text())
        eboot = next(m for m in led["modules"] if m["name"] == "EBOOT.BIN")
        fake = self.dir / "fake-root"
        fake.mkdir()
        for m in led["modules"]:
            src = Path(sel["roots"][m["input"]["root"]]) / m["input"]["path"]
            (fake / m["input"]["path"]).parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(src, fake / m["input"]["path"])
        img = fake / eboot["input"]["path"]
        data = bytearray(img.read_bytes())
        data[0x1000 + 0x08b15280 - 0x08804000] ^= 0xFF          # one byte inside a matched function (SNC)
        img.write_bytes(bytes(data))
        sel["roots"] = {k: str(fake) for k in sel["roots"]}
        sel_path.write_text(json.dumps(sel), encoding="utf-8")
        self.assertRefused(self.sb.promote(G171), "target-drift", "EBOOT.BIN")

    def test_retarget_refuses_a_lock_change_that_breaks_a_proof(self) -> None:
        # merge G171's function with the next one in the inventory; the proof no longer ends on a function end
        cpath = self.dir / "campaign" / "ledger" / "campaign.json"
        led = json.loads(cpath.read_text())
        mod = next(m for m in led["modules"] if m["name"] == "EBOOT.BIN")
        fns = sorted(mod["functions"], key=lambda f: f["start"])
        i = next(k for k, f in enumerate(fns) if f["start"] == 0x08804f24)
        fns[i]["end"] = fns[i + 1]["end"]
        del fns[i + 1]
        mod["functions"] = fns
        cpath.write_text(json.dumps(led))
        fpath = self.dir / "data" / "functions-EBOOT.BIN.txt"
        fpath.write_text("".join(f"{f['start']:x} {f['end']:x} {f.get('boundary_status') or 'auto'}\n" for f in fns))
        doc = json.loads((self.dir / "data" / "modules.json").read_text())
        next(m for m in doc["modules"] if m["name"] == "EBOOT.BIN")["function_count"] = len(fns)
        (self.dir / "data" / "modules.json").write_text(json.dumps(doc, indent=2) + "\n")
        self.assertRefused(self.sb.promote(G171), "target-drift")
        self.assertRefused(self.sb.gate("retarget", "--reason", "test"), "do not hold for the new target")


class LedgerMutations(GateCase):
    def ledger_lines(self) -> list[bytes]:
        return (self.dir / "data" / "accepted-ledger.jsonl").read_bytes().split(b"\n")[:-1]

    def put(self, lines: list[bytes]) -> None:
        (self.dir / "data" / "accepted-ledger.jsonl").write_bytes(b"".join(x + b"\n" for x in lines))

    def test_delete_a_proof_line_by_hand(self) -> None:
        lines = self.ledger_lines()
        self.put(lines[:1] + lines[2:])
        self.before = self.sb.state()
        self.assertRefused(self.sb.promote(G171), "ledger")
        self.assertCheckFails("ledger")

    def test_duplicate_a_proof_with_recomputed_digests(self) -> None:
        # a forger copies a proof (a clone counted twice) and recomputes the proof, body and header digests
        lines = self.ledger_lines()
        header = json.loads(lines[0])
        proofs = [json.loads(x) for x in lines[1:]]
        twin = next(p for p in proofs if p["source"] == SNC)
        dup = dict(twin, source="src/eboot/FUN_08b1526c_again.c")
        dup["proof_sha256"] = P.proof_digest(dup)
        proofs.append(dup)
        proofs.sort(key=lambda p: (p["module"], int(p["start"], 16), p["source"]))
        body = P.ledger_body(proofs)
        header.update(count=len(proofs), bytes=P.bytes_by_module(proofs), ranges_sha256=P.ranges_digest(proofs),
                      body_sha256=P.sha256(body))
        (self.dir / "data" / "accepted-ledger.jsonl").write_bytes(P.canon(header) + b"\n" + body)
        self.before = self.sb.state()
        self.assertRefused(self.sb.promote(G171), "out of order or duplicates|overlaps")
        self.assertCheckFails("ledger")

    def test_silent_removal_rewritten_through_the_writer(self) -> None:
        # a forger uses the ledger writer itself to drop a proof (a consistent ledger and journal): only the history
        # shows it, and `check --base` refuses it
        r = self.sb.py("import lbpproof as P\nled = P.load_ledger()\nproofs = [p for p in led.proofs if p['source'] != "
                       f"{BSS!r}]\nP.write_ledger(proofs, {{'event': 'promote', 'added': [], 'replaced': [], 'dropped': [], "
                       "'refused': 0}, led.header['toolchain'], led.header['target'])")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertEqual(self.sb.check().returncode, 0)       # consistent on its own (the source is merely unproven)
        self.assertCheckFails("accepted bytes at HEAD are no longer accepted", args=("--base", "HEAD"))

    def test_fabricated_proof_through_the_writer(self) -> None:
        # a forger writes a proof for a source that does not match, with every digest right: `verify --since HEAD`
        # recompiles it and refuses
        self.copy(NEW)
        self.sb.edit(NEW, "  return;", "  *(volatile int *)0x40 = 1;")     # does not match retail
        code = f"""
import json, lbpproof as P
led = P.load_ledger()
src = {NEW!r}
base = next(p for p in led.proofs if p['source'] == {G171!r})
fake = dict(base, source=src, start='0x08804f1c', end='0x08804f24', symbol='FUN_08804f1c',
            source_sha256=P.file_sha256(P.ROOT / src))
fake['deps'] = dict(base['deps'], source_sha256=fake['source_sha256'], symbols=[])
fake['deps_sha256'] = P.digest(fake['deps'])
fake['compared'] = {{'.text': {{'addr': '0x08804f1c', 'size': 8, 'sha256': '0' * 64}}}}
fake['proof_sha256'] = P.proof_digest(fake)
P.write_ledger(led.proofs + [fake], {{'event': 'promote', 'added': [], 'replaced': [], 'dropped': [], 'refused': 0}},
               led.header['toolchain'], led.header['target'])
"""
        r = self.sb.py(code)
        self.assertEqual(r.returncode, 0, r.stderr)
        v = self.sb.gate("verify", "--since", "HEAD")
        self.assertEqual(v.returncode, 1, v.stdout + v.stderr)
        self.assertIn(f"DIFFERENT {NEW}", v.stdout)

    def test_widened_range_through_the_writer(self) -> None:
        # a forger widens an accepted proof to the next function and re-chains everything: the proof no longer agrees
        # with its own source's header, so `check` refuses it
        r = self.sb.py("import lbpproof as P\nled = P.load_ledger()\nout = []\nfor p in led.proofs:\n"
                       f"    if p['source'] == {G171!r}:\n"
                       "        p = {k: v for k, v in p.items() if k != 'proof_sha256'}\n"
                       "        p['end'] = '0x08804f34'\n"
                       "        p['proof_sha256'] = P.proof_digest(p)\n"
                       "    out.append(p)\n"
                       "P.write_ledger(out, {'event': 'promote'}, led.header['toolchain'], led.header['target'])")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertCheckFails("header differs from the proof")

    def test_source_edited_during_the_run(self) -> None:
        code = f"""
import lbppromote as R, lbpproof as P
ctx = R.Context()
class A: parallel = 2; keep_run = False
run = R.new_run(ctx, A)
orig = R.Run.run_own_compile
def edited(self, cands):
    out = orig(self, cands)
    p = cands[0].path
    p.write_text(p.read_text() + "\\n/* edited while the run was compiling */\\n")
    return out
R.Run.run_own_compile = edited
proofs, refused = run.verify(R.candidates_for([{G171!r}], P.tree_sources()))
print(sorted(proofs), refused)
"""
        r = self.sb.py(code)
        self.assertIn("source-changed", r.stdout, r.stdout + r.stderr)

    def test_rewrite_the_journal(self) -> None:
        j = self.dir / "data" / "accepted-journal.jsonl"
        ev = json.loads(j.read_bytes().splitlines()[0])
        ev["subsumed"] = {"x": "y"}
        j.write_bytes(P.canon(ev) + b"\n")
        self.before = self.sb.state()
        self.assertRefused(self.sb.promote(G171), "journal")

    def test_delete_the_journal(self) -> None:
        (self.dir / "data" / "accepted-journal.jsonl").unlink()
        r = self.sb.promote(G171)
        self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
        self.assertIn("journal", r.stdout)

    def test_truncate_the_ledger(self) -> None:
        p = self.dir / "data" / "accepted-ledger.jsonl"
        p.write_bytes(p.read_bytes()[:-40])
        self.before = self.sb.state()
        self.assertRefused(self.sb.promote(G171), "ledger")

    def test_ledger_bound_to_another_toolchain(self) -> None:
        r = self.sb.py("import lbpproof as P\nled = P.load_ledger()\nproofs = [dict(p, toolchain='1' * 64) for p in led.proofs]\n"
                       "for p in proofs: p['proof_sha256'] = P.proof_digest(p)\n"
                       "P.write_ledger(proofs, {'event': 'promote'}, '1' * 64, led.header['target'])")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.before = self.sb.state()
        self.assertRefused(self.sb.promote(G171), "binding")

    def test_concurrent_promotion_is_locked_out(self) -> None:
        (self.dir / "try" / "promote.lock").mkdir()
        self.assertRefused(self.sb.promote(G171, "--lock-wait", "0"), "another promotion holds")

    def test_seed_runs_only_once(self) -> None:
        self.assertRefused(self.sb.gate("seed"), "a ledger exists")


class UnitChecks(unittest.TestCase):
    """Pure functions, no sandbox (no WSL, no toolchain)."""

    def test_ranges(self) -> None:
        self.assertEqual(P.merge([(8, 12), (0, 4), (4, 8)]), [(0, 12)])
        self.assertEqual(P.subtract([(0, 100)], [(10, 20), (30, 40)]), [(0, 10), (20, 30), (40, 100)])
        self.assertEqual(P.subtract([(0, 100)], [(0, 100)]), [])
        self.assertEqual(P.length([(0, 10), (5, 15)]), 15)          # a byte counts once
        self.assertEqual(P.intersect_length([(0, 10)], [(5, 20)]), 5)

    def test_masked_difference(self) -> None:
        obj = bytes.fromhex("0000000c" "00000000")                  # jal 0 ; nop (little-endian words)
        linked = bytes.fromhex("74b0000c" "00000000")               # jal with the target filled in
        self.assertIsNone(P.masked_difference(obj, linked, [P.Reloc(0, 4, 1)]))
        self.assertEqual(P.masked_difference(obj, linked, []), 0)   # without the relocation it is a change
        bad = bytes.fromhex("74b00008" "00000000")                  # j instead of jal: not a relocation fill
        self.assertEqual(P.masked_difference(obj, bad, [P.Reloc(0, 4, 1)]), 3)
        with self.assertRaises(P.ProofError):
            P.masked_difference(obj, linked, [P.Reloc(0, 99, 1)])   # an unknown relocation type is not trusted

    def test_overlapping_proofs_are_invalid(self) -> None:
        base = {"schema": P.PROOF_SCHEMA, "module": "EBOOT.BIN", "symbol": "f", "cc": "gcc404xs-Os", "lang": "c99",
                "toolchain": "a" * 64, "target": "b" * 64, "compared": {}, "object_sha256": "c" * 64,
                "status": "EXACT", "gate": {"result": "pass", "idioms": []}, "run": "r", "verified_utc": "t"}

        def proof(src, s, e):
            p = dict(base, source=src, start=P.hx(s), end=P.hx(e), source_sha256="d" * 64)
            p["deps"] = {"source_sha256": "d" * 64, "headers": [], "unresolved_includes": [], "symbols": []}
            p["deps_sha256"] = P.digest(p["deps"])
            p["proof_sha256"] = P.proof_digest(p)
            return p
        P.validate_proofs([proof("a", 0, 8), proof("b", 8, 16)])
        with self.assertRaises(P.ProofError):
            P.validate_proofs([proof("a", 0, 12), proof("b", 8, 16)])
        with self.assertRaises(P.ProofError):
            P.validate_proofs([proof("a", 0, 8), proof("a", 8, 16)])  # one source, two ranges

    def test_migration_scope(self) -> None:
        import lbppromote as R
        prof = {"gcc404xs-Os": {"flags": "-Os"}}
        prog = {"gcc4_home:psp-gcc-4.0.4x": {"sha256": "a"}}
        old = {"profiles": prof, "programs": prog, "primary": "gcc404xs-Os", "include": {"include/a.h": "1"},
               "comparison_code": {"tools/lbpgate.py": "1", "tools/lbpmatch.py": "1"}, "environment": {}}

        def new(**kw):
            return {**old, **kw}
        self.assertEqual(R.migration_scope(old, new(comparison_code={"tools/lbpgate.py": "2", "tools/lbpmatch.py": "1"}))[0],
                         "gate")
        self.assertEqual(R.migration_scope(old, new(include={"include/a.h": "2"})), ("include", {"include/a.h"}))
        added = new(profiles={**prof, "gcc171-O1": {"flags": "-O1"}}, programs={**prog, "gcc_home:psp-gcc-1.3.1": {}})
        self.assertEqual(R.migration_scope(old, added)[0], "added")
        # a changed profile, a changed kit, a new primary or a matcher change is a full reverification
        self.assertEqual(R.migration_scope(old, new(profiles={"gcc404xs-Os": {"flags": "-O2"}}))[0], "full")
        self.assertEqual(R.migration_scope(old, new(programs={"gcc4_home:psp-gcc-4.0.4x": {"sha256": "b"}}))[0], "full")
        self.assertEqual(R.migration_scope(old, {**added, "primary": "gcc171-O1"})[0], "full")
        self.assertEqual(R.migration_scope(old, new(comparison_code={"tools/lbpgate.py": "1", "tools/lbpmatch.py": "2"}))[0],
                         "full")

    def test_layout_follows_alignment(self) -> None:
        secs = [P.Section(1, ".text", 1, 6, 0, 0, 10, 0, 0, 4), P.Section(2, ".gnu.linkonce.t._Z1fv", 1, 6, 0, 0, 8, 0, 0, 8)]
        self.assertEqual([a for _s, a in P.Elf.layout(secs, 0x100)], [0x100, 0x110])


if __name__ == "__main__":
    unittest.main()
