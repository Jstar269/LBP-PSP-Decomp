#!/usr/bin/env python3
"""Random-mutation search (lbpperm.py's mutations and weighted score) seeded with the cheapest near misses in wip/.

    python tools/lbpnearperm.py --tag n1 [--max-left 6] [--classes register_colouring,instruction_schedule,...]
                                [--levers permute,frame,types,expression] [--profiles gcc404xs-Os,...]
                                [--rounds 8] [--mutants 16] [--parallel 2] [--seed 1] [--limit N]
                                [--no-profile-phase] [--report try/nearmiss-report.json] [wip/file ...]

Port of hst-decomp's tools/hstnearperm.py. lbpperm.py starts from generated candidates under try/gen-*; this tool
starts from the files in wip/ that tools/lbpnearmiss.py measured (try/nearmiss-report.json), so a near miss that a
lane or an earlier permuter run parked is mutated again from its best-known text. Each round compiles `--mutants`
variants of every unfinished seed with lbpmatch.py (into try/nearperm-<tag>/build), keeps the best by lbpperm's
weighted score (register-only 5, moved 60, missing/extra 100) and stops a seed the moment a variant is EXACT.

Seeds must have one of --classes, one of --levers (default: the levers whose calls and control flow already agree
with retail), at most --max-left words left, and (with --profiles) one of those cc= profiles. A file whose calls or
branches differ (lever body or structure) or whose only difference is a symbol (referent) is not a permuter problem,
so it is left out unless it is named on the command line. Files src/ already covers, addresses reserved by an
unfinished lane batch (lbpqueue.reservations), and headers with extra= or helpers= are never seeded.

Profile phase (the GCC analogue of hstnearperm's MWCC pragma phase; --no-profile-phase skips it): after the rounds,
the best text of every open seed is compiled once under each other LOCKED profile of data/toolchain-manifest.json,
only cc= changed (never extra flags). It runs last so that a source under the seed's own profile wins; report.json
lists the profiles that lowered the score without reaching zero (`profile_improved`), which are leads about the unit.

Nothing is installed: exact variants go to try/nearperm-<tag>/exact/ (with the seed's name and a trailing `//` line
naming the run and, for the profile phase, the cc=) for a person to review and pass through lbpmatch.py and
lbppromote.py; the best non-exact variants go to try/nearperm-<tag>/best/. A variant that reads worse than its seed
(casts, duplicated conditions) should not be installed.
"""
from __future__ import annotations

import argparse
import json
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpmatch as M  # noqa: E402
import lbpnearmiss as N  # noqa: E402
import lbpperm as P  # noqa: E402

ROOT = M.ROOT
DEFAULT_LEVERS = ("permute", "frame", "types", "expression")
DEFAULT_CLASSES = ("register_colouring", "instruction_schedule", "frame_layout", "branch_shape", "unknown",
                   "constant_materialisation", "type_signedness")
NEVER = ("superseded", "blocked", "tool_state", "exact_now")


def select(rows: list[dict], classes: set, levers: set, max_left: int, profiles: set | None, max_words: int,
           chosen: set) -> list[dict]:
    """The report rows to seed, cheapest first (code bytes per word left). Named files skip the class, lever, words
    and profile filters."""
    out = []
    for row in sorted(rows, key=lambda r: (-r["yield"], r["wip"])):
        if chosen:
            if row["wip"] in chosen:
                out.append(row)
            continue
        if row["class"] in NEVER or row["class"] not in classes:
            continue
        if row["words_left"] is None or row["words_left"] > max_left or row["words"] > max_words:
            continue
        if row.get("lever", next(iter(levers), None)) not in levers:
            continue
        if profiles is not None and row.get("cc") not in profiles:
            continue
        out.append(row)
    return out


def reserved_addresses() -> set[int]:
    try:
        import lbpqueue as Q
        return Q.reservations(ROOT, Q.lane_dirs(), Q.QUEUE)[0]
    except Exception as e:  # noqa: BLE001 - no queue state is not an error here
        print(f"lane reservations unavailable ({type(e).__name__}: {e})", file=sys.stderr)
        return set()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", type=Path)
    ap.add_argument("--tag", required=True)
    ap.add_argument("--report", type=Path, default=N.REPORT_JSON)
    ap.add_argument("--max-left", type=int, default=6, help="seed only files with at most this many words left")
    ap.add_argument("--classes", default=",".join(DEFAULT_CLASSES))
    ap.add_argument("--levers", default=",".join(DEFAULT_LEVERS))
    ap.add_argument("--profiles", default="", help="seed only files with one of these cc= profiles (default: any)")
    ap.add_argument("--rounds", type=int, default=8)
    ap.add_argument("--mutants", type=int, default=16)
    ap.add_argument("--parallel", type=int, default=2)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--max-words", type=int, default=1200)
    ap.add_argument("--no-profile-phase", action="store_true")
    ap.add_argument("--sideways", type=float, default=P.SIDEWAYS)
    ap.add_argument("--restart", type=float, default=P.RESTART)
    args = ap.parse_args()
    import lbplane as L
    import lbpsweep as S
    rng = random.Random(args.seed)
    work = ROOT / "try" / f"nearperm-{args.tag}"
    if work.exists():
        raise SystemExit(f"{work} exists; use a new tag")
    report = json.loads(args.report.read_text(encoding="utf-8"))
    chosen = {N.rel(f if f.is_absolute() else ROOT / f) for f in args.files}
    profiles = set(args.profiles.split(",")) if args.profiles else None
    rows = select(report["files"], set(args.classes.split(",")), set(args.levers.split(",")), args.max_left, profiles,
                  args.max_words, chosen)
    covered = L.covered(ROOT)
    reserved = reserved_addresses()
    seeds, skipped = [], {"reserved": 0, "not_a_seed": 0}
    for row in rows:
        got = P.wip_seed(ROOT / row["wip"], covered, 1 << 30)
        if got is None:
            skipped["not_a_seed"] += 1
            continue
        if got[0][1] in reserved:
            skipped["reserved"] += 1
            continue
        seeds.append((row, got))
    if args.limit:
        seeds = seeds[:args.limit]
    print(f"seeds={len(seeds)} skipped={skipped} classes={args.classes} levers={args.levers} max_left={args.max_left}",
          flush=True)
    state, ends, owner = {}, {}, {}
    for row, (key, text) in seeds:
        if key in state:
            continue
        h = S.header_of(text)              # wip_seed checked that line 1 is the whole header
        ends[key] = h["end"]
        state[key] = P.new_entry(text, row["wip"], Path(row["wip"]).suffix, h["symbol"])
    run = P.make_runner(work / "build", args.parallel, ends)
    exact_dir = work / "exact"
    exact: dict = {}

    def on_exact(key: tuple, path: Path, profile: str) -> None:
        out = P.write_proposal(args.tag, exact_dir, path, state[key]["origin"], profile, tool="lbpnearperm.py")
        exact[state[key]["origin"]] = {"proposal": N.rel(out), "profile": profile or None}
        print(f"EXACT {key[0]} 0x{key[1]:08x} {state[key]['origin']}{' [cc=' + profile + ']' if profile else ''} "
              f"-> {N.rel(out)}", flush=True)

    # the seeds themselves, so that every later score is measured the same way
    base_files = []
    for idx, (key, st) in enumerate(state.items()):
        path = work / "seed" / f"{args.tag}_s{idx:04d}_{key[1]:08x}{st['suffix']}"
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(st["text"], encoding="utf-8", newline="\n")
        base_files.append(path)
        owner[str(path.resolve())] = key
    compiles = len(base_files)
    accepted: dict = {}
    for path_text, (s, w, _status) in sorted(run(base_files, work / "seed", owner).items()):
        key = owner[path_text]
        st = state[key]
        st.update(score=s, words=w, best_score=s, best_words=w)
        if s == 0:
            accepted[key] = Path(path_text)
            on_exact(key, Path(path_text), "")
    keys = sorted(state, key=lambda k: state[k]["score"])
    seed_scores = {state[k]["origin"]: [state[k]["score"], state[k]["words"]] for k in keys}
    print("seed scores (weighted/words): " + " ".join(f"{state[k]['origin'].removeprefix('wip/')}="
                                                      f"{state[k]['score']}/{state[k]['words']}" for k in keys[:40]),
          flush=True)
    _acc, n = P.search(work, args.tag, state, keys, args.rounds, args.mutants, rng, run, on_exact, args.sideways,
                       args.restart, accepted)
    compiles += n
    better: dict = {}
    if not args.no_profile_phase:
        locked, primary = P.locked_profiles()
        _exact, better, n = P.profile_phase(work, args.tag, state, keys, accepted, run, locked, primary, on_exact)
        compiles += n
    summary = {}
    (work / "best").mkdir(parents=True, exist_ok=True)
    for key in keys:
        if key in accepted:
            continue
        st = state[key]
        (work / "best" / Path(st["origin"]).name).write_text(st["best_text"], encoding="utf-8", newline="\n")
        summary[st["origin"]] = {"seed": seed_scores[st["origin"]], "best": [st["best_score"], st["best_words"]]}
    (work / "report.json").write_text(json.dumps({
        "tag": args.tag, "seeds": len(state), "compiles": compiles, "exact": exact, "profile_improved": better,
        "best_of_the_rest": summary}, indent=1) + "\n", encoding="utf-8")
    print(json.dumps({"seeds": len(state), "exact": len(exact), "compiles": compiles}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
