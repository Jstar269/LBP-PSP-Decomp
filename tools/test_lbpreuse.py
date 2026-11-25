#!/usr/bin/env python3
"""Tests for the licensing bookkeeping (lbpreuse.py): glob semantics, notice families, the library block."""
from __future__ import annotations

import io
import json
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbporigin  # noqa: E402
import lbpreuse as R  # noqa: E402


class ReuseTests(unittest.TestCase):
    def test_globs_follow_reuse(self) -> None:
        self.assertTrue(R.glob_regex("src/**").match("src/eboot/a.c"))
        self.assertTrue(R.glob_regex("docs/*.md").match("docs/GUIDE.md"))
        self.assertFalse(R.glob_regex("docs/*.md").match("docs/public/README.md"))

    def test_licence_files_are_skipped_as_reuse_does(self) -> None:
        self.assertTrue(R.reuse_ignored("LICENSE"))
        self.assertTrue(R.reuse_ignored("LICENSES/Zlib.txt"))
        self.assertFalse(R.reuse_ignored("third_party/newlib/newlib-1.14.0-COPYING.NEWLIB"))

    def test_newlib_notice_families(self) -> None:
        text = "/* Copyright (c) 1993 Martin Birgmeier\n * All rights reserved. */\n/* by David M. Gay */\n"
        found = R.families(text)
        self.assertEqual(set(found), {"LicenseRef-Newlib-Birgmeier", "LicenseRef-Newlib-Gay-AT-T"})

    def test_library_block_names_each_class_once(self) -> None:
        block = R.render_libs({"zlib": ["src/eboot/zlib_b.c", "src/eboot/zlib_a.c"], "libstdc++": ["src/eboot/x.c"]})
        self.assertTrue(block.startswith(R.BEGIN_LIBS) and block.rstrip().endswith(R.END_LIBS))
        self.assertEqual(block.count("[[annotations]]"), 2)
        self.assertLess(block.index("zlib_a.c"), block.index("zlib_b.c"))
        self.assertIn('SPDX-License-Identifier = "Zlib"', block)

    def test_gpl_derived_reconstructions_keep_the_gpl(self) -> None:
        # the project's own work is CC0-1.0 (2026-11-23); what derives from GPL code is not
        self.assertEqual(R.PROJECT_LICENCE, "CC0-1.0")
        self.assertEqual(R.LIBRARY_CLASSES["libstdc++"][0], "GPL-3.0-or-later")
        block = R.render_libs({"libstdc++": ["src/eboot/x.c"]})
        self.assertIn('SPDX-License-Identifier = "GPL-3.0-or-later"', block)
        self.assertNotIn("CC0", block)

    def test_a_gpl_derived_class_without_terms_is_named(self) -> None:
        classes = {"src/eboot/b.c": "libgcc", "src/eboot/a.c": "libgcc", "src/eboot/g.c": "game",
                   "src/eboot/s.c": "libstdc++"}
        self.assertEqual(R.unmapped_gpl_sources(classes), ["src/eboot/a.c", "src/eboot/b.c"])
        self.assertEqual(R.unmapped_gpl_sources({"src/eboot/g.c": "game"}), [])

    def test_every_licence_named_in_the_library_classes_has_a_text(self) -> None:
        for expression, _statements in R.LIBRARY_CLASSES.values():
            for lid in R.license_ids(expression):
                self.assertTrue((R.LICENSES / f"{lid}.txt").is_file(), lid)


GAME = "/* LBPMATCH module=EBOOT.BIN start=0x08900000 end=0x08900008 symbol=FUN_08900000 lang=c99 */\n"
SUN_NOTICE = ("/*\n * ====================================================\n"
              " * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\n *\n"
              " * Developed at SunPro, a Sun Microsystems, Inc. business.\n"
              " * Permission to use, copy, modify, and distribute this\n"
              " * software is freely granted, provided that this notice\n * is preserved.\n */\n")
STRCPY = ("/* LBPMATCH module=EBOOT.BIN start=0x08b9d000 end=0x08b9d040 symbol=strcpy lang=c99 cc=gcc171-O2 */\n"
          "/* Library source: newlib 1.14.0 libc_string/strcpy.c function strcpy, preprocessed (comments kept). */\n"
          "char *strcpy(char *d, const char *s);\n")
BASE_TOML = """version = 1

[[annotations]]
path = ["src/**", "wip/**"]
SPDX-FileCopyrightText = "2026 Jstar269 and contributors"
SPDX-License-Identifier = "CC0-1.0"

[[annotations]]
path = ["REUSE.toml", "data/**", "third_party/newlib/README.md"]
SPDX-FileCopyrightText = "2026 Jstar269 and contributors"
SPDX-License-Identifier = "CC0-1.0"

{begin}
{end}

{begin_libs}
{end_libs}
"""


class CheckCommandTests(unittest.TestCase):
    """`lbpreuse.py check` on a throwaway git repository (no game data: synthetic sources only)."""

    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        subprocess.run(["git", "init", "-q"], cwd=self.root, check=True)
        (self.root / "LICENSES").mkdir()
        for lid in ("CC0-1.0", "LicenseRef-Newlib-Red-Hat", "LicenseRef-Newlib-COPYING-NEWLIB"):
            (self.root / "LICENSES" / f"{lid}.txt").write_text(lid + "\n", encoding="utf-8")
        (self.root / "data").mkdir()
        (self.root / "data" / "origin-regions.json").write_text(json.dumps({"module": "EBOOT.BIN", "regions": [
            {"start": "0x08804000", "end": "0x08b00000", "class": "game"},
            {"start": "0x08b00000", "end": "0x08c00000", "class": "sony-sdk"}]}), encoding="utf-8")
        (self.root / "third_party" / "newlib").mkdir(parents=True)
        (self.root / "third_party" / "newlib" / "README.md").write_text(f"{R.BEGIN_MD}\n{R.END_MD}\n", encoding="utf-8")
        (self.root / "REUSE.toml").write_text(BASE_TOML.format(begin=R.BEGIN_TOML, end=R.END_TOML,
                                                               begin_libs=R.BEGIN_LIBS, end_libs=R.END_LIBS),
                                              encoding="utf-8")
        classify = lbporigin.classify
        self.patches = [patch.object(R, "ROOT", self.root), patch.object(R, "REUSE_TOML", self.root / "REUSE.toml"),
                        patch.object(R, "LICENSES", self.root / "LICENSES"),
                        patch.object(R, "README", self.root / "third_party" / "newlib" / "README.md"),
                        patch.object(lbporigin, "classify", lambda root=self.root: classify(root, hst={})),
                        patch.object(lbporigin, "rel", lambda p, root=self.root: p.relative_to(root).as_posix())]
        for p in self.patches:
            p.start()

    def tearDown(self) -> None:
        for p in self.patches:
            p.stop()
        self.tmp.cleanup()

    def add(self, rel: str, text: str) -> None:
        path = self.root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        subprocess.run(["git", "add", "--", rel], cwd=self.root, check=True)

    def check(self, regenerate: bool = True) -> tuple[int, str]:
        out = io.StringIO()
        with redirect_stdout(out):
            subprocess.run(["git", "add", "-A", "--", "REUSE.toml", "LICENSES", "third_party", "data"],
                           cwd=self.root, check=True)
            if regenerate:
                R.newlib_command(write=True)
            rc = R.check_command()
        return rc, out.getvalue()

    def test_fixture_is_compliant(self) -> None:
        self.add("src/eboot/FUN_08900000.c", GAME + "int f(void) { return 0; }\n")
        self.add("src/eboot/strcpy_08b9d000.c", STRCPY)
        rc, out = self.check()
        self.assertEqual(rc, 0, out)

    def test_a_cc0_source_with_a_newlib_notice_is_a_problem(self) -> None:
        # upstream fdlibm text without the `Library source: newlib` note (lbpxclone.py replaces the first note of its
        # exemplar, and in CI hst_root is not set): classified by region, so REUSE.toml dedicates it under CC0-1.0
        self.add("src/eboot/FUN_08900000.c", GAME + SUN_NOTICE + "float f(float x) { return x; }\n")
        rc, out = self.check()
        self.assertEqual(rc, 1, out)
        self.assertIn("src/eboot/FUN_08900000.c", out)

    def test_a_later_annotation_cannot_replace_the_generated_newlib_licence(self) -> None:
        self.add("src/eboot/strcpy_08b9d000.c", STRCPY)
        self.check()
        for expression in ("LicenseRef-Newlib-COPYING-NEWLIB", "CC0-1.0 OR LicenseRef-Newlib-Red-Hat"):
            toml = (self.root / "REUSE.toml").read_text(encoding="utf-8").split(R.END_LIBS)[0] + R.END_LIBS + "\n"
            (self.root / "REUSE.toml").write_text(
                toml + f'\n[[annotations]]\npath = "src/eboot/strcpy_08b9d000.c"\nprecedence = "override"\n'
                f'SPDX-FileCopyrightText = "x"\nSPDX-License-Identifier = "{expression}"\n', encoding="utf-8")
            rc, out = self.check(regenerate=False)
            self.assertEqual(rc, 1, f"{expression}: {out}")
            self.assertIn("src/eboot/strcpy_08b9d000.c", out)

    def test_an_annotation_without_a_licence_is_reported_not_a_crash(self) -> None:
        self.add("src/eboot/FUN_08900000.c", GAME + "int f(void) { return 0; }\n")
        self.check()
        toml = (self.root / "REUSE.toml").read_text(encoding="utf-8")
        (self.root / "REUSE.toml").write_text(
            toml + '\n[[annotations]]\npath = "src/eboot/**"\nSPDX-FileCopyrightText = "x"\n'
            'SPDX-Licence-Identifier = "CC0-1.0"\n', encoding="utf-8")
        rc, out = self.check(regenerate=False)
        self.assertEqual(rc, 1, out)
        self.assertIn("lacks SPDX-License-Identifier", out)


if __name__ == "__main__":
    unittest.main()
