#!/usr/bin/env python3
"""The capability milestones of data/milestones.json (docs/COMPLETION.md) and their record, docs/milestones.json.

    python tools/lbpmilestones.py [--tree DIR]                   print every milestone's progress (nothing is written)
    python tools/lbpmilestones.py --write [--tree DIR] [--date D] write DIR/docs/milestones.json
    python tools/lbpmilestones.py --check [--tree DIR]            exit 1 when the record disagrees with the figures

DIR is a public tree (default: the tree this file sits in); tools/lbppublish.py calls write_tree() for the tree it
builds and tree_problems() in --audit. The record keeps, for every milestone, the first date it measured 100%
(`reached`), and that date is never removed: a reached milestone that later measures below 100%, or can no longer be
measured, stays in the record with `regressed` true (shown as REGRESSED), so a regression is visible instead of
rewriting history. The record is dated by the newest accepted proof the figures count (`--date` overrides), so it
changes only when the figures do.

Each measure reads its own evidence and never estimates:

  inventory  the campaign ledger (lbp-config `campaign_root`): function bytes whose boundary is reviewed over all
             function bytes; reached only when the ledger also has a region inventory with every region reviewed and
             no open candidate span. The public tree has no campaign: this measure is then not available here and
             the record keeps what the maintainer's run measured
  t1         data/public-figures.json: matched code bytes of the public scope over the public scope of the named
             modules; a module with no published figures (Sony code kept out of this repository) is not counted
  t2, t3     the same figures' whole-module build (tools/lbpbuild.py, `t2`) and shifted build (`t3`): code bytes of the
             public scope produced from source in a byte-identical image over the public scope; reached only when
             every byte of it is, and not measured while the figures say so
  t4         the strict tier of tools/lbpprogress.py (0 until review, gate 7, is recorded)
  portable   not measured yet: this project has no host-compile check (hst-decomp's tools/hstport.py)
  platform   the PSP system functions the game executable imports (its .lib.stub table, read from the owner's copy
             through the campaign) looked up in Nakagawa Recomp's reviewed import audit (lbp-config `nakagawa_root`,
             tools/import_audit_baseline.json); only a dedicated handler reviewed "complete" counts. Not available here
             without both
  evidence   an entry of data/milestones.json "evidence" naming a recorded scenario

--check recomputes every measure that is available here and compares it, the reached dates and the REGRESSED flags
with the record; a measure that is not available here is reported as skipped, never guessed.
"""
from __future__ import annotations

import argparse
import datetime
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpprogress
import lbpreport

ROOT = Path(__file__).resolve().parent.parent
DEFS_REL = "data/milestones.json"
RECORD_REL = "docs/milestones.json"
DEFS_SCHEMA = "lbp-decomp.milestones.v1"
RECORD_SCHEMA = "lbp-decomp.milestone-record.v1"
CAP = 99.999        # the most a milestone whose criterion does not fully hold can show
DATE = re.compile(r"\d{4}-\d{2}-\d{2}")


class Unavailable(str):
    """The reason a measure cannot be taken on this machine (its evidence is not here); not a measurement."""


def share(done: int, whole: int) -> float:
    """done/whole in percent, 100.0 only when done == whole (rounding never reaches 100% early)."""
    if not whole:
        return 0.0
    return 100.0 if done >= whole else min(round(100 * done / whole, 3), CAP)


def names_of(spec, all_names: list[str]) -> list[str]:
    return list(all_names) if spec == "all" else list(spec)


# --------------------------------------------------------------------------------------------------------------
# measures

def measure_t1(names: list[str], figures: dict) -> dict:
    have = [n for n in names if n in figures]
    if not have:
        return {"progress": None, "detail": "no published figures for " + ", ".join(names)}
    done = sum(figures[n]["public_t1_bytes"] for n in have)
    scope = sum(figures[n]["public_scope_bytes"] for n in have)
    kept = [n for n in names if n not in figures]
    return {"progress": share(done, scope),
            "detail": f"{done:,} of {scope:,} code bytes of the public scope"
                      + (f" ({', '.join(kept)}: Sony code kept out of this repository, not counted)" if kept else "")}


def measure_build(names: list[str], figures: dict, tier: str) -> dict:
    """t2 or t3 from the figures: public-scope code bytes produced from source in a byte-identical build."""
    have = [n for n in names if n in figures]
    measured = [n for n in have if isinstance(figures[n].get(tier), dict)]
    what = "whole-module build" if tier == "t2" else "shifted build"
    if not measured:
        why = next((figures[n].get(tier) for n in have if isinstance(figures[n].get(tier), str)), "not measured")
        return {"progress": None, "detail": f"{what}: {why}"}
    done = sum(figures[n][tier][f"public_{tier}_bytes"] for n in measured)
    scope = sum(figures[n]["public_scope_bytes"] for n in have)
    whole = len(measured) == len(have) and all(figures[n][tier].get("image_identical") is True for n in measured)
    progress = share(done, scope) if whole else min(share(done, scope), CAP)
    missing = [n for n in have if n not in measured]
    return {"progress": progress,
            "detail": f"{done:,} of {scope:,} code bytes of the public scope from source in the {what}"
                      + (f"; not measured for {', '.join(missing)}" if missing else "")}


def measure_inventory(campaign: dict | None) -> dict | Unavailable:
    if campaign is None:
        return Unavailable("needs the campaign ledger (campaign_root)")
    total = reviewed = 0
    regions_total = regions_reviewed = 0
    have_regions = False
    open_candidates = 0
    for m in campaign["modules"]:
        for f in m.get("functions", []):
            size = f["end"] - f["start"]
            total += size
            if f.get("boundary_status") == "reviewed":
                reviewed += size
        if "regions" in m:
            have_regions = True
            for r in m["regions"]:
                regions_total += r["end"] - r["start"]
                if r.get("review_status") == "reviewed":
                    regions_reviewed += r["end"] - r["start"]
        open_candidates += sum(1 for c in m.get("function_candidates", []) if c.get("boundary_status") != "reviewed")
    complete = reviewed == total and have_regions and regions_reviewed == regions_total and not open_candidates
    progress = share(reviewed, total) if complete else min(share(reviewed, total), CAP)
    detail = f"{reviewed:,} of {total:,} function bytes have a reviewed boundary"
    detail += (f"; {regions_reviewed:,} of {regions_total:,} region bytes reviewed" if have_regions
               else "; the ledger has no region inventory yet (data and gaps are not classified)")
    if open_candidates:
        detail += f"; {open_candidates} candidate spans open"
    return {"progress": progress, "detail": detail}


def stub_imports(image: bytes) -> list[tuple[str, int]]:
    """(library, NID) of every function a PSP module imports, from its .lib.stub table (ELF32 little-endian)."""
    shoff, = struct.unpack_from("<I", image, 0x20)
    phoff, = struct.unpack_from("<I", image, 0x1C)
    phentsize, phnum, shentsize, shnum, shstrndx = struct.unpack_from("<HHHHH", image, 0x2A)
    sections = [struct.unpack_from("<IIIIIIIIII", image, shoff + i * shentsize) for i in range(shnum)]
    loads = [struct.unpack_from("<IIIIIIII", image, phoff + i * phentsize) for i in range(phnum)]
    names = sections[shstrndx][4]

    def cstr(off: int) -> str:
        return image[off:image.index(b"\0", off)].decode("latin-1")

    def file_offset(addr: int) -> int:
        for kind, off, vaddr, _paddr, filesz, _memsz, _flags, _align in loads:
            if kind == 1 and vaddr <= addr < vaddr + filesz:
                return off + addr - vaddr
        raise ValueError(f"address 0x{addr:08x} is in no loaded segment")

    stub = next((s for s in sections if cstr(names + s[0]) == ".lib.stub"), None)
    if stub is None:
        return []
    out = []
    off, end = stub[4], stub[4] + stub[5]
    while off < end:
        libname, _version, _attr, words, _nvars, nfuncs, nids, _stubs = struct.unpack_from("<IHHBBHII", image, off)
        lib = cstr(file_offset(libname))
        out += [(lib, struct.unpack_from("<I", image, file_offset(nids) + 4 * k)[0]) for k in range(nfuncs)]
        off += max(words, 1) * 4
    return out


def classify_imports(nids, audit: dict[int, dict]) -> dict[str, int]:
    """Imported NIDs by Nakagawa Recomp's audit: only a dedicated handler reviewed 'complete' counts as done."""
    counts = {"complete": 0, "partial or unreviewed": 0, "refused or fake": 0, "missing": 0}
    for nid in nids:
        row = audit.get(nid)
        if row is None:
            counts["missing"] += 1
        elif row.get("classification") != "dedicated":
            counts["refused or fake"] += 1
        elif row.get("status") == "complete":
            counts["complete"] += 1
        else:
            counts["partial or unreviewed"] += 1
    return counts


def measure_platform(imports: list[tuple[str, int]] | None, audit: dict[int, dict] | None) -> dict | Unavailable:
    if imports is None or audit is None:
        return Unavailable("needs the game executable (campaign_root) and Nakagawa Recomp's import audit "
                           "(nakagawa_root)")
    nids = sorted({nid for _lib, nid in imports})
    counts = classify_imports(nids, audit)
    return {"progress": share(counts["complete"], len(nids)) if nids else None,
            "detail": f"{counts['complete']} of {len(nids)} imported system functions complete in Nakagawa Recomp; "
                      f"{counts['partial or unreviewed']} partial or unreviewed, {counts['refused or fake']} refused "
                      f"or fake, {counts['missing']} missing"}


# --------------------------------------------------------------------------------------------------------------
# the rows and the record

def definitions(tree: Path) -> dict:
    doc = json.loads((tree / DEFS_REL).read_text(encoding="utf-8"))
    if doc.get("schema") != DEFS_SCHEMA:
        raise ValueError(f"{DEFS_REL}: schema {doc.get('schema')!r}, expected {DEFS_SCHEMA}")
    ids = [m["id"] for m in doc["milestones"]]
    if len(set(ids)) != len(ids):
        raise ValueError(f"{DEFS_REL}: two milestones share an id")
    return doc


def compute(defs: dict, figures: dict, modules_doc: dict, campaign: dict | None = None,
            imports: list[tuple[str, int]] | None = None, audit: dict[int, dict] | None = None) -> list[dict]:
    """One row per milestone: id, title, criterion, progress (0-100, None when not measured yet, or an Unavailable
    reason when its evidence is not on this machine) and detail."""
    all_names = [m["name"] for m in modules_doc["modules"]]
    evidence = {e["id"]: e for e in defs.get("evidence", [])}
    rows = []
    for m in defs["milestones"]:
        kind = m["measure"]
        if kind == "inventory":
            r = measure_inventory(campaign)
        elif kind == "t1":
            r = measure_t1(names_of(m["modules"], all_names), figures)
        elif kind in ("t2", "t3"):
            r = measure_build(names_of(m["modules"], all_names), figures, kind)
        elif kind == "t4":
            lad = lbpprogress.ladder(figures, modules_doc)
            r = {"progress": lad["strict_percent"], "detail": "every gate of docs/COMPLETION.md; no source is "
                                                              "reviewed yet (gate 7)"}
        elif kind == "portable":
            r = {"progress": None, "detail": "not measured yet: this project has no host-compile check"}
        elif kind == "platform":
            r = measure_platform(imports, audit)
        elif kind == "evidence":
            e = evidence.get(m["id"])
            r = ({"progress": 100.0, "detail": f"scenario recorded: {e['evidence']}"} if e
                 else {"progress": 0.0, "detail": "no recorded scenario yet"})
        else:
            raise ValueError(f"{DEFS_REL}: unknown measure {kind!r} for {m['id']}")
        if isinstance(r, Unavailable):
            r = {"progress": r, "detail": f"not measured here: {r}"}
        rows.append({"id": m["id"], "title": m["title"], "criterion": m["criterion"], **r})
    return rows


def record(rows: list[dict], old: dict | None, date: str) -> dict:
    """The record: each row's progress (a measure not available here keeps the old record's), the first date it was
    reached (kept forever) and the REGRESSED flag."""
    previous = {r["id"]: r for r in (old or {}).get("milestones", [])}
    out = []
    for r in rows:
        p = previous.get(r["id"], {})
        progress, detail = r["progress"], r["detail"]
        if isinstance(progress, Unavailable):
            progress, detail = (p["progress"], p["detail"]) if "progress" in p else (None, detail)
        reached = p.get("reached")
        if reached is None and progress is not None and progress >= 100.0:
            reached = date
        out.append({"id": r["id"], "title": r["title"], "progress": progress, "detail": detail, "reached": reached,
                    "regressed": bool(reached) and (progress is None or progress < 100.0)})
    return {"schema": RECORD_SCHEMA, "generated": date, "milestones": out}


def render(doc: dict) -> str:
    return json.dumps(doc, indent=1) + "\n"


def record_problems(rows: list[dict], doc: dict | None, date: str | None) -> tuple[list[str], list[str]]:
    """(problems, skipped ids): why the record does not follow from the rows measured now."""
    if doc is None:
        return [f"{RECORD_REL} is missing"], []
    out, skipped = [], []
    if doc.get("schema") != RECORD_SCHEMA:
        out.append(f"{RECORD_REL}: schema {doc.get('schema')!r}, expected {RECORD_SCHEMA}")
    generated = doc.get("generated")
    if not isinstance(generated, str) or not DATE.fullmatch(generated):
        out.append(f"{RECORD_REL}: no generated date")
    elif date is not None and generated != date:
        out.append(f"{RECORD_REL}: generated {generated}, the figures are of {date} (regenerate the record)")
    have = doc.get("milestones", [])
    if [(r.get("id"), r.get("title")) for r in have] != [(r["id"], r["title"]) for r in rows]:
        out.append(f"{RECORD_REL}: its milestones are not those of {DEFS_REL}, in that order")
        return out, skipped
    for want, got in zip(rows, have):
        rid = want["id"]
        reached = got.get("reached")
        if reached is not None and (not isinstance(reached, str) or not DATE.fullmatch(reached)
                                    or (isinstance(generated, str) and reached > generated)):
            out.append(f"{rid}: reached {reached!r} is not a date on or before the record's")
        progress = got.get("progress")
        if got.get("regressed") is not (bool(reached) and (progress is None or progress < 100.0)):
            out.append(f"{rid}: the REGRESSED flag does not follow from its progress and reached date")
        if progress is not None and progress >= 100.0 and reached is None:
            out.append(f"{rid}: at 100% without a reached date")
        if isinstance(want["progress"], Unavailable):
            skipped.append(rid)
            continue
        if progress != want["progress"] or got.get("detail") != want["detail"]:
            out.append(f"{rid}: the record says {progress!r} ({got.get('detail')}), measured {want['progress']!r} "
                       f"({want['detail']})")
    return out, skipped


# --------------------------------------------------------------------------------------------------------------
# inputs that only the maintainer's (or a contributor's own) machine has

def private_inputs(modules_doc: dict) -> dict:
    """{campaign, imports, audit} for the measures that need the owner's copy or Nakagawa Recomp; None for each that
    is not configured here. Never raises for a missing configuration."""
    out: dict = {"campaign": None, "imports": None, "audit": None}
    try:
        import lbpconfig as CFG
        import lbpmatch as M
        root = M.campaign_root()
        out["campaign"] = json.loads((root / "ledger" / "campaign.json").read_text(encoding="utf-8-sig"))
        cache: dict = {}
        imports = []
        for m in modules_doc["modules"]:
            if m.get("kind") == "game":
                imports += stub_imports(M.module_bytes(cache, m["name"])[0])
        out["imports"] = imports
        if CFG.get("nakagawa_root"):
            path = CFG.host_path("nakagawa_root") / "tools" / "import_audit_baseline.json"
            out["audit"] = {int(k, 16): v for k, v in json.loads(path.read_text(encoding="utf-8")).items()}
    except (SystemExit, OSError, ValueError, KeyError, StopIteration, struct.error):
        pass
    return out


def tree_rows(tree: Path, inputs: dict | None = None) -> tuple[list[dict], str | None]:
    """The rows measured for a tree, and the date of its figures (the newest accepted proof they count)."""
    figures = lbpreport.load_figures(tree / lbpreport.FIGURES_REL)
    modules_doc = json.loads((tree / lbpprogress.MODULES_REL).read_text(encoding="utf-8"))
    inputs = private_inputs(modules_doc) if inputs is None else inputs
    rows = compute(definitions(tree), figures, modules_doc, **inputs)
    dates = sorted(f["ledger_date"] for f in figures.values() if f.get("ledger_date"))
    return rows, dates[-1] if dates else None


def load_record(tree: Path) -> dict | None:
    path = tree / RECORD_REL
    return json.loads(path.read_text(encoding="utf-8")) if path.is_file() else None


def write_tree(tree: Path, date: str | None = None, inputs: dict | None = None) -> dict:
    """Write the tree's record from its figures, keeping the reached dates of the record it has; the record."""
    rows, figures_date = tree_rows(tree, inputs)
    doc = record(rows, load_record(tree), date or figures_date or datetime.date.today().isoformat())
    (tree / RECORD_REL).parent.mkdir(parents=True, exist_ok=True)
    (tree / RECORD_REL).write_text(render(doc), encoding="utf-8", newline="\n")
    return doc


def tree_problems(tree: Path, require: bool = False, inputs: dict | None = None) -> list[str]:
    """Why a tree's record disagrees with its figures ([] when it agrees). Without `require`, a tree without
    data/milestones.json or without a record is not checked (lbppublish.py --audit of a tree it did not build)."""
    if not require and not ((tree / DEFS_REL).is_file() and (tree / RECORD_REL).is_file()):
        return []
    try:
        rows, date = tree_rows(tree, inputs)
        return record_problems(rows, load_record(tree), date)[0]
    except (OSError, ValueError, KeyError, TypeError) as exc:
        return [f"{RECORD_REL}: the milestones cannot be measured ({exc})"]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tree", type=Path, default=ROOT, help="the public tree (default: the tree this file is in)")
    ap.add_argument("--date", help="the record's date (default: the newest accepted proof the figures count)")
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--write", action="store_true", help=f"write {RECORD_REL}")
    mode.add_argument("--check", action="store_true", help=f"compare {RECORD_REL} with the figures; write nothing")
    args = ap.parse_args()
    tree = args.tree.resolve()
    if not (tree / lbpreport.FIGURES_REL).is_file():
        print(f"no {tree / lbpreport.FIGURES_REL}: in the private repository build a tree first "
              "(tools/lbppublish.py --out <dir>) and pass --tree <dir>")
        return 2
    modules_doc = json.loads((tree / lbpprogress.MODULES_REL).read_text(encoding="utf-8"))
    inputs = private_inputs(modules_doc)
    if args.write:
        doc = write_tree(tree, args.date, inputs)
        print(f"wrote {RECORD_REL} ({doc['generated']})")
    rows, date = tree_rows(tree, inputs)
    have = {r["id"]: r for r in (load_record(tree) or {}).get("milestones", [])}
    for r in rows:
        p = r["progress"]
        shown = ("not here" if isinstance(p, Unavailable) else "not measured" if p is None else f"{p:7.3f} %")
        flag = "  REGRESSED" if have.get(r["id"], {}).get("regressed") else ""
        print(f"{r['id']:16} {shown:>14}  {r['detail']}{flag}")
    if not args.check:
        return 0
    problems, skipped = record_problems(rows, load_record(tree), date)
    if skipped:
        print(f"skipped (their evidence is not on this machine; the record keeps the maintainer's): {', '.join(skipped)}")
    print("milestones: " + ("the record agrees" if not problems else "the record DIFFERS"))
    for p in problems:
        print("  " + p)
    return 1 if problems else 0


if __name__ == "__main__":
    raise SystemExit(main())
