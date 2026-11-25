#!/usr/bin/env python3
"""Licensing bookkeeping: REUSE coverage check and the generated annotation blocks (newlib, zlib, libstdc++).

    python tools/lbpreuse.py check              # every tracked file is covered by exactly one effective annotation
    python tools/lbpreuse.py generate           # print the generated annotations and the newlib source table
    python tools/lbpreuse.py generate --write   # rewrite the generated parts of REUSE.toml and third_party/newlib/README.md

Ported from hst-decomp's hstreuse.py; the library block (zlib, GNU libstdc++/libsupc++, LIBRARY_CLASSES) is new here.

The repository follows the REUSE specification (3.x): REUSE.toml maps path globs to SPDX-License-Identifier and
SPDX-FileCopyrightText, the licence texts are in LICENSES/. This tool is the project's own check, for machines that
do not have the `reuse` tool installed. `check` does what `reuse lint` is meant to establish:

  * every file git tracks matches at least one [[annotations]] path glob of REUSE.toml, and the LAST matching
    annotation is the effective one (the order of the file decides, so the newlib annotations come after the
    generic src/** one and carry precedence = "override");
  * every licence named by an effective annotation has a text in LICENSES/<id>.txt (warns about unused texts);
  * the generated newlib block of REUSE.toml and the table in third_party/newlib/README.md are current, and the
    paths they cover are exactly the sources tools/lbporigin.py classifies as `newlib`.

Files REUSE itself does not annotate are skipped as the tool does: LICENSES/, licence files named LICENSE* or
COPYING* and *.license files.

The newlib annotations are derived from the sources themselves: each newlib-derived source in src/ is the upstream
file preprocessed with its comments kept, so the notices that appear in the file (from the upstream file and from
the newlib headers it includes) say which licences apply. A source whose upstream file has no notice of its own
(NO_OWN_NOTICE) falls under newlib's default copyright, "Red Hat Incorporated" in COPYING.NEWLIB.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tomllib
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbporigin

ROOT = lbporigin.ROOT
PROJECT_COPYRIGHT = "2026 Jstar269 and contributors"

# sha256 of the newlib release tarballs the sources were reconstructed from (PROVENANCE.json beside them).
TARBALLS = {
    "1.13.0": "c8562ef73ced13940829faa5f6a8ce4fd1d44e45388b3043ce58baedf03830d9",
    "1.14.0": "7db55671132c2943dbbdadfaa954915c23dbc329168575a42bb940ef4bf50a23",
    "1.15.0": "c4496102d38c59d1a47ddd5481af35caa1f65b76e2a94d9607737e17fd9e4465",
}
# Upstream files that carry no copyright or licence comment of their own (found by scanning each upstream file
# of the tarballs for "copyright", "permission to use" and "redistribution"). They fall under the default
# copyright of newlib's COPYING.NEWLIB: Red Hat Incorporated.
NO_OWN_NOTICE = {
    ("1.13.0", "libc/string/strcasecmp.c"),
    ("1.14.0", "libc/search/bsearch.c"), ("1.14.0", "libc/stdlib/abort.c"), ("1.14.0", "libc/stdlib/mbrtowc.c"),
    ("1.14.0", "libc/stdlib/mbtowc_r.c"), ("1.14.0", "libc/stdlib/wcrtomb.c"), ("1.14.0", "libc/stdlib/wcsrtombs.c"),
    ("1.14.0", "libc/string/memchr.c"), ("1.14.0", "libc/string/memcmp.c"), ("1.14.0", "libc/string/strcat.c"),
    ("1.14.0", "libc/string/strchr.c"), ("1.14.0", "libc/string/strcpy.c"), ("1.14.0", "libc/string/strncmp.c"),
    ("1.14.0", "libc/string/strrchr.c"),
    # found the same way for LittleBigPlanet's newlib sources (2026-10-28)
    ("1.13.0", "libc/ctype/isalnum.c"), ("1.13.0", "libc/ctype/isalpha.c"), ("1.13.0", "libc/ctype/isspace.c"),
    ("1.14.0", "libc/errno/errno.c"), ("1.14.0", "libc/signal/signal.c"), ("1.14.0", "libc/stdlib/mbrlen.c"),
    ("1.14.0", "libc/string/strncat.c"), ("1.14.0", "libc/string/strstr.c"),
}
RED_HAT_DEFAULT = {   # the default copyright of COPYING.NEWLIB in each release
    "1.13.0": "Copyright (c) 1994, 1997, 2001, 2002, 2003, 2004 Red Hat Incorporated",
    "1.14.0": "Copyright (c) 1994, 1997, 2001, 2002, 2003, 2004, 2005 Red Hat Incorporated",
}
# The identifiers in LICENSES/ for the newlib notices, in the order they are written in an expression.
ID_ORDER = ["LicenseRef-Newlib-UCB-BSD-4-Clause", "LicenseRef-Newlib-UCB-BSD-3-Clause",
            "LicenseRef-Newlib-UCB-Advertising-Notice", "LicenseRef-Newlib-Gay-AT-T", "LicenseRef-Newlib-Sun-fdlibm",
            "LicenseRef-Newlib-Red-Hat", "LicenseRef-Newlib-OAR", "LicenseRef-Newlib-Birgmeier"]
# The licence of the project's own part of a near miss in wip/ (the generic src/**, wip/** annotation of REUSE.toml).
PROJECT_LICENCE = "CC0-1.0"

LIB_NOTE = re.compile(r"Library source: newlib (?P<ver>\d+\.\d+\.\d+) (?P<dir>\w+)/(?P<file>\w+)\.c function (?P<fn>\w+)")
ADAPTED_NOTE = re.compile(r"Library source: newlib (?P<dir>libm) (?P<file>\w+)\.c function (?P<fn>\w+)")
IDENTIFIED_NOTE = re.compile(r"Identified as newlib (?P<dir>libm) (?P<file>\w+)\.c \((?P<fn>\w+)")
CLONE_NOTE = re.compile(r"^/\* Clone of (src/\S+) \(lbpclone\.py tag \w+\)")
COMMENT = re.compile(r"/\*.*?\*/", re.S)

BEGIN_TOML, END_TOML = "# BEGIN generated newlib annotations", "# END generated newlib annotations"
BEGIN_LIBS, END_LIBS = "# BEGIN generated library annotations", "# END generated library annotations"
# The other open-source libraries LittleBigPlanet links, by origin class (tools/lbporigin.py): licence expression and
# copyright statements of the reconstructed sources. zlib sources keep zlib's notice (the zlib licence requires it).
# libstdc++/libsupc++ of GCC 4.0 is GPL-2.0-or-later with the libstdc++ runtime exception; the reconstructions are
# distributed under GPL-3.0-or-later, which that licence permits (THIRD_PARTY.md). They derive from GPL code, so the
# change of the project's own work to CC0-1.0 (2026-11-23) does not apply to them.
LIBRARY_CLASSES = {
    "zlib": ("Zlib", ["Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler"]),
    "libstdc++": ("GPL-3.0-or-later", ["Copyright (C) 1997-2005 Free Software Foundation, Inc.", PROJECT_COPYRIGHT]),
}
# Origin classes of GPL-derived sources that have no entry in LIBRARY_CLASSES yet. No source has such a class today;
# when one appears it needs its library's terms there first, because the generic src/** annotation is CC0-1.0 and
# must not cover GPL-derived code. `check` refuses such a source.
UNMAPPED_GPL_CLASSES = ("libgcc",)
BEGIN_MD, END_MD = "<!-- BEGIN generated source table -->", "<!-- END generated source table -->"
README = ROOT / "third_party" / "newlib" / "README.md"
REUSE_TOML = ROOT / "REUSE.toml"
LICENSES = ROOT / "LICENSES"


# ---- notices -----------------------------------------------------------------------------------------------------
def families(text: str) -> dict[str, set[str]]:
    """{licence id: {copyright statements}} for the newlib notices found in the comments of a source."""
    found: dict[str, set[str]] = defaultdict(set)
    for comment in COMMENT.findall(text):
        flat = re.sub(r"\s*\n\s*\*?\s*", " ", comment)
        if "Martin Birgmeier" in flat:
            found["LicenseRef-Newlib-Birgmeier"].add("Copyright (c) 1993 Martin Birgmeier")
        elif "On-Line Applications Research" in flat:
            found["LicenseRef-Newlib-OAR"].add("Copyright (c) 1989-2000 On-Line Applications Research Corporation (OAR)")
        elif "David M. Gay" in flat:
            found["LicenseRef-Newlib-Gay-AT-T"].add("Copyright (c) 1991 by AT&T")
        elif "Sun Microsystems" in flat:
            found["LicenseRef-Newlib-Sun-fdlibm"].add("Copyright (C) 1993 by Sun Microsystems, Inc.")
        elif "Regents of the University of California" in flat and "Copyright" in flat:
            years = re.search(r"Copyright \(c\) ([\d, ]+?)\s+(?:The )?(Regents of the University of California)", flat)
            if years is None:
                continue
            holder = f"Copyright (c) {years.group(1)} " + ("The " if "The Regents" in flat else "") + years.group(2)
            if "All advertising materials mentioning features or use of this software" in flat:
                family = "LicenseRef-Newlib-UCB-BSD-4-Clause"
            elif "Neither the name of the University nor the names of its contributors" in flat:
                family = "LicenseRef-Newlib-UCB-BSD-3-Clause"
            elif "provided that the above copyright notice and this paragraph are" in flat:
                family = "LicenseRef-Newlib-UCB-Advertising-Notice"
            else:
                raise SystemExit(f"unrecognised University of California notice: {flat[:200]}")
            found[family].add(holder)
    return found


# Notices of other projects' code that families() does not map to a newlib licence; a source carrying one (or a newlib
# notice) outside the classes with their own terms is not the project's own work, so CC0-1.0 must not cover it.
OTHER_NOTICES = {"Free Software Foundation": "GNU/FSF", "GNU General Public License": "GNU GPL",
                 "Jean-loup Gailly": "zlib"}


def third_party_notices(text: str) -> list[str]:
    """The third-party notices found in the comments of a source: newlib licence ids and OTHER_NOTICES names."""
    try:
        found = sorted(families(text))
    except SystemExit as e:   # a University of California notice families() cannot place is still a notice
        found = [str(e)[:60]]
    comments = " ".join(re.sub(r"\s+", " ", c) for c in COMMENT.findall(text))
    return found + [name for marker, name in OTHER_NOTICES.items() if marker in comments]


class Entry:
    def __init__(self, path: Path):
        self.rel = lbporigin.rel(path)
        self.text = path.read_text(encoding="utf-8", errors="replace")
        lines = self.text.split("\n", 3)
        self.kind, self.exemplar = "direct", None
        clone = CLONE_NOTE.match(lines[1])
        if clone:
            self.kind, self.exemplar = "clone", clone.group(1)
        head = "\n".join(lines[:3])
        xclone = lbporigin.XCLONE_NOTE.match(lines[1])
        if xclone and not LIB_NOTE.search(head):
            # a copy of an hst-decomp newlib source whose clone note took the place of the library note: the note is
            # read from the exemplar in hst-decomp (hst_root); the notices are in this file's own text
            self.kind, self.exemplar = "hst-decomp clone", "hst-decomp " + xclone.group(1)
            exemplar = Path(lbporigin.CFG.require("hst_root")) / xclone.group(1)
            head = "\n".join(exemplar.read_text(encoding="utf-8", errors="replace").split("\n", 3)[:3])
        m = LIB_NOTE.search(head)
        if m:
            self.ver, self.function = m["ver"], m["fn"]
            self.upstream = m["dir"].replace("_", "/", 1) + "/" + m["file"] + ".c"
        else:
            m = ADAPTED_NOTE.search(head) or IDENTIFIED_NOTE.search(self.text[:3000])
            if m is None:
                raise SystemExit(f"{self.rel}: cannot read the newlib note")
            self.ver, self.function = None, m["fn"]
            self.upstream = f"libm/math/{m['file']}.c"
            self.kind = "hand-adapted" if self.rel.startswith("src/") else "near miss"
        found = families(self.text)
        copyrights = {c for cs in found.values() for c in cs}
        ids = set(found)
        if self.ver and (self.ver, self.upstream) in NO_OWN_NOTICE:
            ids.add("LicenseRef-Newlib-Red-Hat")
            copyrights.add(RED_HAT_DEFAULT[self.ver])
        self.ids = [i for i in ID_ORDER if i in ids]
        self.copyrights = sorted(copyrights)
        if set(ids) - set(ID_ORDER) or not self.ids:
            raise SystemExit(f"{self.rel}: no recognised newlib notice")
        self.in_src = self.rel.startswith("src/")

    @property
    def expression(self) -> str:
        return " AND ".join(self.ids + ([] if self.in_src else [PROJECT_LICENCE]))

    @property
    def statements(self) -> list[str]:
        return self.copyrights + ([] if self.in_src else [PROJECT_COPYRIGHT])


def tracked_classes() -> dict[str, str]:
    """Origin classes of the sources git tracks (scratch files under wip/ that are not tracked are left out, so the
    generated blocks are the same in every checkout of the same commit)."""
    classes, _warnings = lbporigin.classify()
    tracked = set(tracked_files())
    return {p: c for p, c in classes.items() if p in tracked}


def newlib_entries() -> list[Entry]:
    return [Entry(ROOT / p) for p, c in tracked_classes().items() if c == "newlib"]


def render_toml(entries: list[Entry]) -> str:
    groups: dict[tuple, list[str]] = defaultdict(list)
    for e in entries:
        groups[(e.expression, tuple(e.statements))].append(e.rel)
    out = [BEGIN_TOML + " (tools/lbpreuse.py generate --write); do not edit by hand",
           "# One annotation per distinct set of notices. The files are the upstream newlib files preprocessed with",
           "# their comments kept, so the licences of the notices inside them (and of the newlib headers they include)",
           "# apply; precedence = \"override\" makes these win over the generic src/** annotation above."]
    for (expression, statements), paths in sorted(groups.items()):
        out += ["", "[[annotations]]", "path = ["] + [f'  "{p}",' for p in sorted(paths)] + ["]"]
        out += ['precedence = "override"', "SPDX-FileCopyrightText = ["] + [f'  "{s}",' for s in statements] + ["]"]
        out += [f'SPDX-License-Identifier = "{expression}"']
    return "\n".join(out) + "\n" + END_TOML + "\n"


def render_table(entries: list[Entry]) -> str:
    rows = ["| Source | Kind | Newlib | Upstream file | Function | Terms (LICENSES/) | Tarball SHA-256 |",
            "|---|---|---|---|---|---|---|"]
    for e in sorted(entries, key=lambda e: e.rel):
        if e.ver:
            version, sha = e.ver, TARBALLS[e.ver]
        else:   # libm/math/kf_rem_pio2.c and ef_pow.c are byte-identical in 1.13.0, 1.14.0 and 1.15.0
            version, sha = "1.13.0, 1.14.0, 1.15.0 (identical; not recorded)", "all three"
        terms = ", ".join(i.removeprefix("LicenseRef-Newlib-") for i in e.ids)
        rows.append(f"| `{e.rel}` | {e.kind} | {version} | `newlib/{e.upstream}` | `{e.function}` | {terms} | "
                    f"{'`' + sha + '`' if sha != 'all three' else sha} |")
    return "\n".join(rows) + "\n"


def splice(text: str, begin: str, end: str, block: str) -> str:
    i, j = text.index(begin), text.index(end) + len(end)
    return text[:i] + block.rstrip("\n") + text[j:]


def library_paths() -> dict[str, list[str]]:
    out: dict[str, list[str]] = defaultdict(list)
    for p, c in tracked_classes().items():
        if c in LIBRARY_CLASSES:
            out[c].append(p)
    return out


def unmapped_gpl_sources(classes: dict[str, str]) -> list[str]:
    """The sources of a GPL-derived origin class that LIBRARY_CLASSES does not map to its library's terms."""
    return sorted(p for p, c in classes.items() if c in UNMAPPED_GPL_CLASSES and c not in LIBRARY_CLASSES)


def render_libs(paths: dict[str, list[str]]) -> str:
    out = [BEGIN_LIBS + " (tools/lbpreuse.py generate --write); do not edit by hand",
           "# zlib and GNU libstdc++/libsupc++ sources, by origin class (tools/lbporigin.py)."]
    for cls in sorted(paths):
        expression, statements = LIBRARY_CLASSES[cls]
        out += ["", f"# {cls}", "[[annotations]]", "path = ["] + [f'  "{p}",' for p in sorted(paths[cls])] + ["]"]
        out += ['precedence = "override"', "SPDX-FileCopyrightText = ["] + [f'  "{s}",' for s in statements] + ["]"]
        out += [f'SPDX-License-Identifier = "{expression}"']
    return "\n".join(out) + "\n" + END_LIBS + "\n"


def newlib_command(write: bool) -> int:
    entries = newlib_entries()
    toml, table, libs = render_toml(entries), render_table(entries), render_libs(library_paths())
    if not write:
        print(toml)
        print(libs)
        print(table)
        return 0
    text = splice(REUSE_TOML.read_text(encoding="utf-8"), BEGIN_TOML, END_TOML, toml)
    REUSE_TOML.write_text(splice(text, BEGIN_LIBS, END_LIBS, libs), encoding="utf-8", newline="\n")
    README.write_text(splice(README.read_text(encoding="utf-8"), BEGIN_MD, END_MD,
                             BEGIN_MD + "\n\n" + table + "\n" + END_MD), encoding="utf-8", newline="\n")
    print(f"wrote {len(entries)} newlib-derived sources into REUSE.toml and {README.relative_to(ROOT).as_posix()}")
    return 0


# ---- coverage check ----------------------------------------------------------------------------------------------
def glob_regex(pattern: str) -> re.Pattern:
    """REUSE.toml path globs: `*` matches anything but `/`, `**` matches anything including `/`."""
    out, i = [], 0
    while i < len(pattern):
        if pattern.startswith("**", i):
            out.append(".*")
            i += 2
        elif pattern[i] == "*":
            out.append("[^/]*")
            i += 1
        elif pattern[i] == "\\" and i + 1 < len(pattern):
            out.append(re.escape(pattern[i + 1]))
            i += 2
        else:
            out.append(re.escape(pattern[i]))
            i += 1
    return re.compile("".join(out) + r"\Z")


IGNORED_NAMES = (re.compile(r"^LICEN[CS]E([-.].*)?$"), re.compile(r"^COPYING([-.].*)?$"), re.compile(r".*\.license$"))


def reuse_ignored(path: str) -> bool:
    return path.startswith("LICENSES/") or any(p.match(path.rsplit("/", 1)[-1]) for p in IGNORED_NAMES)


def tracked_files() -> list[str]:
    out = subprocess.run(["git", "ls-files", "-z"], cwd=ROOT, check=True, capture_output=True,
                         env={**os.environ, "GIT_TERMINAL_PROMPT": "0"}).stdout
    return [p.decode("utf-8") for p in out.split(b"\0") if p]


def license_ids(expression: str) -> set[str]:
    return {t for t in re.split(r"[\s()]+", expression) if t and t not in ("AND", "OR", "WITH")}


def check_command() -> int:
    problems: list[str] = []
    config = tomllib.loads(REUSE_TOML.read_text(encoding="utf-8"))
    annotations = config.get("annotations", [])
    compiled = []
    for n, a in enumerate(annotations):
        paths = a.get("path", [])
        paths = [paths] if isinstance(paths, str) else paths
        if not paths:
            problems.append(f"annotation #{n + 1} lacks path")
        for key in ("SPDX-License-Identifier", "SPDX-FileCopyrightText"):
            if not a.get(key):
                problems.append(f"annotation #{n + 1} ({(paths or ['?'])[0]}...) lacks {key}")
        compiled.append([glob_regex(p) for p in paths])

    def expression(n: int) -> str:
        value = annotations[n].get("SPDX-License-Identifier", "")
        return value if isinstance(value, str) else " AND ".join(value)
    files = tracked_files()
    effective: dict[str, int] = {}
    uncovered, skipped = [], 0
    for path in files:
        if reuse_ignored(path):
            skipped += 1
            continue
        matches = [n for n, pats in enumerate(compiled) if any(p.match(path) for p in pats)]
        if not matches:
            uncovered.append(path)
        else:
            effective[path] = matches[-1]
    if uncovered:
        problems.append(f"{len(uncovered)} tracked files are not covered by any annotation (first: {uncovered[0]})")
    used = set()
    for n in set(effective.values()):
        used |= license_ids(expression(n))
    for lid in sorted(used):
        if not (LICENSES / f"{lid}.txt").is_file():
            problems.append(f"licence {lid} is used but LICENSES/{lid}.txt is missing")
    present = {p.stem for p in LICENSES.glob("*.txt")}
    for lid in sorted(present - used):
        print(f"warning: LICENSES/{lid}.txt is not used by any effective annotation")
    # the library block must be current and give every zlib/libstdc++ source its library's terms
    libs = library_paths()
    toml_text = REUSE_TOML.read_text(encoding="utf-8")
    if BEGIN_LIBS not in toml_text or splice(toml_text, BEGIN_LIBS, END_LIBS, render_libs(libs)) != toml_text:
        problems.append("the generated library block of REUSE.toml is stale (run: lbpreuse.py generate --write)")
    for cls, paths in libs.items():
        for path in paths:
            if path in effective and expression(effective[path]) != LIBRARY_CLASSES[cls][0]:
                problems.append(f"{path} is {cls} but its effective annotation is not {LIBRARY_CLASSES[cls][0]}")
    for path in unmapped_gpl_sources(tracked_classes()):
        problems.append(f"{path} is GPL-derived (a class of UNMAPPED_GPL_CLASSES) but LIBRARY_CLASSES gives it no "
                        "terms, so the CC0-1.0 src/** annotation would cover it")
    # the newlib block must be current and cover exactly the sources lbporigin calls newlib
    entries = newlib_entries()
    toml_text = REUSE_TOML.read_text(encoding="utf-8")
    if BEGIN_TOML not in toml_text or splice(toml_text, BEGIN_TOML, END_TOML, render_toml(entries)) != toml_text:
        problems.append("the generated newlib block of REUSE.toml is stale (run: lbpreuse.py generate --write)")
    readme_text = README.read_text(encoding="utf-8") if README.exists() else ""
    if BEGIN_MD not in readme_text or splice(readme_text, BEGIN_MD, END_MD,
                                             BEGIN_MD + "\n\n" + render_table(entries) + "\n" + END_MD) != readme_text:
        problems.append("the source table of third_party/newlib/README.md is stale (run: lbpreuse.py generate --write)")
    newlib_expected = {e.rel: e.expression for e in entries}
    for path, n in effective.items():
        if "LicenseRef-Newlib" in expression(n).replace("LicenseRef-Newlib-COPYING-NEWLIB", ""):
            if path not in newlib_expected:
                problems.append(f"{path} carries a newlib licence but lbporigin does not classify it newlib")
    for path, want in sorted(newlib_expected.items()):
        # the generated annotation must be the effective one: a later annotation must not replace its licence
        got = expression(effective[path]) if path in effective else ""
        if "LicenseRef-Newlib" not in got:
            problems.append(f"{path} is newlib-derived but its effective annotation has no newlib licence")
        elif got != want:
            problems.append(f"{path} is newlib-derived: its effective annotation is {got!r}, not the generated {want!r}")
    # a source that carries another project's notice is not the project's own work: it must not fall under CC0-1.0
    own_terms = set(newlib_expected) | {p for paths in libs.values() for p in paths}
    for path, n in sorted(effective.items()):
        if (path in own_terms or path.split("/", 1)[0] not in ("src", "wip", "include")
                or not path.endswith((".c", ".cpp", ".h"))):
            continue
        notices = third_party_notices((ROOT / path).read_text(encoding="utf-8", errors="replace"))
        if notices:
            problems.append(f"{path} carries a third-party notice ({', '.join(notices)}) but lbporigin gives it no "
                            f"library class, so REUSE.toml gives it {expression(n)}")
    by_expr: dict[str, int] = defaultdict(int)
    for n in effective.values():
        by_expr[expression(n)] += 1
    print(f"tracked files: {len(files)}; covered: {len(effective)}; skipped as REUSE does (licence files): {skipped}")
    for expr, count in sorted(by_expr.items(), key=lambda kv: -kv[1])[:12]:
        print(f"  {count:5d}  {expr}")
    if len(by_expr) > 12:
        print(f"  ... {len(by_expr) - 12} more distinct expressions")
    for p in problems:
        print(f"PROBLEM: {p}")
    print("lbpreuse check: " + ("COMPLIANT" if not problems else f"{len(problems)} problem(s)"))
    return 1 if problems else 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("check")
    n = sub.add_parser("generate")
    n.add_argument("--write", action="store_true")
    args = ap.parse_args()
    return check_command() if args.cmd == "check" else newlib_command(args.write)


if __name__ == "__main__":
    raise SystemExit(main())
