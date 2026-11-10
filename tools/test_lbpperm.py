#!/usr/bin/env python3
"""Tests for the permuter (lbpperm.py): the 2026-11-02 operators, the zero-test rewrite, the weighted score on
hand-built word lists, plateau moves with a seeded RNG, mutation confined to the header symbol, the locked-profile
variants and proposals written outside src/. No compiler runs."""
from __future__ import annotations

import random
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpperm as P  # noqa: E402

HDR = "/* LBPMATCH module=EBOOT.BIN start=0x08800000 end=0x08800010 symbol=FUN_08800000 lang=c99 */"


class First:
    """A stand-in RNG: always the first choice, a fixed random()."""
    def __init__(self, r: float = 0.0) -> None:
        self.r = r

    def random(self) -> float:
        return self.r

    def choice(self, seq):
        return list(seq)[0]

    def sample(self, seq, k):
        return list(seq)[:k]


def apply(op, body: list[str], rng=None) -> list[str] | None:
    lines = ["void f(void)", "{"] + body + ["}"]
    ok = op(lines, 2, len(lines) - 1, rng or First())
    return lines[2:-1] if ok else None


class OperatorTests(unittest.TestCase):
    def test_temp(self) -> None:
        self.assertEqual(apply(P.m_temp, ["    x = (a + b) * c;"]), ["    int t0 = a + b;", "    x = t0 * c;"])
        self.assertIsNone(apply(P.m_temp, ["    x = f(a + b);"]))          # a call's argument list is not a group

    def test_cast_remove_and_add(self) -> None:
        self.assertEqual(apply(P.m_cast, ["    x = (int)y;"], First(0.0)), ["    x = y;"])
        self.assertEqual(apply(P.m_cast, ["    x = y;"], First(0.9)), ["    x = (int)y;"])

    def test_cast_leaves_cxx_scope_names_alone(self) -> None:
        self.assertIsNone(apply(P.m_cast, ["    x = Foo::bar;"], First(0.9)))

    def test_compound_both_ways(self) -> None:
        self.assertEqual(apply(P.m_compound, ["    x = x + 4;"]), ["    x += 4;"])
        self.assertEqual(apply(P.m_compound, ["    x += a + b;"]), ["    x = x + (a + b);"])

    def test_index_both_ways(self) -> None:
        self.assertEqual(apply(P.m_index, ["    y = p[i];"]), ["    y = *(p + i);"])
        self.assertEqual(apply(P.m_index, ["    y = *(p + i);"]), ["    y = p[i];"])

    def test_decl_scope_sinks_to_first_use(self) -> None:
        self.assertEqual(apply(P.m_decl_scope, ["    int a;", "    foo();", "    a = 1;", "    bar(a);"]),
                         ["    foo();", "    int a;", "    a = 1;", "    bar(a);"])

    def test_loop_form(self) -> None:
        self.assertEqual(apply(P.m_loop_form, ["    for (i = 0; i < n; i++) {", "        s += i;", "    }"]),
                         ["    i = 0;", "    while (i < n) {", "        s += i;", "      i++;", "    }"])
        self.assertEqual(apply(P.m_loop_form, ["    while (p) {", "        p = p->next;", "    }"]),
                         ["    for (; p; ) {", "        p = p->next;", "    }"])

    def test_register_add_and_remove(self) -> None:
        self.assertEqual(apply(P.m_register, ["    int a;"], First(0.9)), ["    register int a;"])
        self.assertEqual(apply(P.m_register, ["    register int a;"], First(0.0)), ["    int a;"])

    def test_field_commute(self) -> None:
        self.assertEqual(apply(P.m_field_commute, ["    x = a->b + c->d;"]), ["    x = c->d + a->b;"])

    def test_every_new_operator_is_in_the_table(self) -> None:
        ops = {f for _w, f in P.MUTATIONS}
        for f in (P.m_temp, P.m_cast, P.m_compound, P.m_index, P.m_decl_scope, P.m_loop_form, P.m_register,
                  P.m_field_commute, P.m_zero_test):
            self.assertIn(f, ops)


class ZeroTestTests(unittest.TestCase):
    def test_not_to_equals_zero(self) -> None:
        self.assertEqual(apply(P.m_zero_test, ["    if (!p) {", "    }"]), ["    if (p == 0) {", "    }"])
        self.assertEqual(apply(P.m_zero_test, ["    return !s->ready;"]), ["    return s->ready == 0;"])

    def test_equals_zero_to_not(self) -> None:
        self.assertEqual(apply(P.m_zero_test, ["    if (a && n == 0) {", "    }"]), ["    if (a && !n) {", "    }"])

    def test_not_equal_zero_dropped_in_a_condition(self) -> None:
        self.assertEqual(apply(P.m_zero_test, ["    while (x != 0) {", "    }"]), ["    while (x) {", "    }"])
        self.assertIsNone(apply(P.m_zero_test, ["    y = (x != 0);"]))      # a value, not a condition

    def test_operands_bound_by_other_operators_are_left_alone(self) -> None:
        self.assertIsNone(apply(P.m_zero_test, ["    y = a + x == 0;"]))
        self.assertIsNone(apply(P.m_zero_test, ["    y = a == !x;"]))
        self.assertIsNone(apply(P.m_zero_test, ["    y = !f(x);"]))


def addu(rd: int, rs: int, rt: int) -> int:
    return (rs << 21) | (rt << 16) | (rd << 11) | 0x21


def lw(rt: int, base: int, imm: int) -> int:
    return (0x23 << 26) | (base << 21) | (rt << 16) | (imm & 0xFFFF)


A, B, C, D = addu(2, 4, 5), lw(3, 4, 8), lw(6, 4, 12), addu(7, 6, 3)


class WeightedScoreTests(unittest.TestCase):
    def test_equal_is_zero(self) -> None:
        self.assertEqual(P.weighted_score([A, B, C], [A, B, C]), 0)

    def test_register_only(self) -> None:
        self.assertEqual(P.weighted_score([A, B, C], [addu(3, 4, 5), B, C]), P.REGALLOC)
        self.assertEqual(P.weighted_score([A, B, C], [addu(3, 4, 5), lw(2, 4, 8), C]), 2 * P.REGALLOC)

    def test_moved_instruction(self) -> None:
        self.assertEqual(P.weighted_score([A, B, C, D], [B, A, C, D]), P.REORDER)

    def test_missing_and_extra_instruction(self) -> None:
        self.assertEqual(P.weighted_score([A, B, C], [A, C]), P.INDEL)          # extra in ours
        self.assertEqual(P.weighted_score([A, C], [A, B, C]), P.INDEL)          # missing in ours

    def test_substitution_counts_once(self) -> None:
        self.assertEqual(P.weighted_score([A, B, C], [A, lw(3, 4, 0x40), C]), P.INDEL)

    def test_kinds_add_up_and_order_closeness(self) -> None:
        ours = [addu(3, 4, 5), C, B, D, A]
        theirs = [addu(2, 4, 5), B, C, D]
        self.assertEqual(P.weighted_score(ours, theirs), P.REGALLOC + P.REORDER + P.INDEL)
        # a register-only variant is closer than a reordered one, which is closer than a missing instruction
        self.assertLess(P.weighted_score([addu(3, 4, 5), B], [A, B]), P.weighted_score([B, A], [A, B]))
        self.assertLess(P.weighted_score([B, A], [A, B]), P.weighted_score([A], [A, B]))


class SearchStepTests(unittest.TestCase):
    def test_lower_always_equal_rarely_higher_never(self) -> None:
        self.assertTrue(P.accept(4, 5, First(0.99)))
        self.assertTrue(P.accept(5, 5, First(0.01)))
        self.assertFalse(P.accept(5, 5, First(0.03)))
        self.assertFalse(P.accept(6, 5, First(0.0)))

    def test_sideways_rate_with_a_seeded_rng(self) -> None:
        rng = random.Random(7)
        took = sum(P.accept(5, 5, rng, 0.02) for _ in range(5000))
        ref = random.Random(7)
        self.assertEqual(took, sum(ref.random() < 0.02 for _ in range(5000)))
        self.assertTrue(50 <= took <= 150, took)

    def test_restart_from_best(self) -> None:
        e = P.new_entry("cur", "wip/x.c", ".c", "f")
        e["best_text"] = "best"
        self.assertEqual(P.base_text(e, First(0.1), 0.15), "best")
        self.assertEqual(P.base_text(e, First(0.5), 0.15), "cur")

    def test_take_tracks_the_best(self) -> None:
        e = P.new_entry("t0", "wip/x.c", ".c", "f", 100, 3)
        self.assertTrue(P.take(e, 60, 2, "t1"))
        self.assertFalse(P.take(e, 60, 2, "t2"))                               # a sideways move
        self.assertEqual((e["text"], e["best_text"], e["best_score"]), ("t2", "t1", 60))


TWO = HDR.replace("FUN_08800000 lang", "FUN_08800010 lang") + """
int FUN_08800000(int a)
{
    int x;
    x = a + 1;
    return x;
}

int FUN_08800010(int a, int b)
{
    int y;
    int z;
    y = a + b;
    z = a * b;
    return y - z;
}
"""


class RangeTests(unittest.TestCase):
    def test_source_names(self) -> None:
        self.assertEqual(P.source_name("FUN_08800000"), "FUN_08800000")
        self.assertEqual(P.source_name("_ZN2ns3Foo3barEi"), "ns::Foo::bar")
        self.assertEqual(P.source_name("_ZNK3Foo3getEv"), "Foo::get")
        self.assertEqual(P.source_name("_ZN3FooC2Ev"), "Foo::Foo")
        self.assertEqual(P.source_name("_ZN3FooD1Ev"), "Foo::~Foo")
        self.assertIsNone(P.source_name("_ZNSt8_Rb_treeIiE5eraseEv"))

    def test_mutations_stay_in_the_header_symbol(self) -> None:
        first = TWO.split("int FUN_08800010")[0]
        rng = random.Random(3)
        changed = 0
        for _ in range(200):
            out = P.mutate(TWO, rng, "FUN_08800010")
            if out is None:
                continue
            changed += 1
            self.assertTrue(out.startswith(first))
            self.assertTrue(out.rstrip().endswith("}"))
        self.assertGreater(changed, 50)

    def test_cxx_member_range(self) -> None:
        text = HDR + "\nstruct Foo { int a; int get() const; };\n\nint Foo::get() const\n{\n    return a;\n}\n"
        lo, hi = P.body_range(text, "_ZNK3Foo3getEv")
        self.assertEqual(text.split("\n")[lo:hi], ["    return a;"])


class ProfileTests(unittest.TestCase):
    LOCKED = ["gcc171-O2", "gcc404xs-O2", "gcc404xs-Os", "snc127-O2"]

    def test_every_other_locked_profile_by_cc_only(self) -> None:
        text = HDR.replace(" */", " cc=gcc171-O2 */") + "\nint f(void) { return 1; }\n"
        got = P.profile_variants(text, self.LOCKED, "gcc404xs-Os")
        self.assertEqual([p for p, _t in got], ["gcc404xs-O2", "gcc404xs-Os", "snc127-O2"])
        for p, t in got:
            self.assertEqual(t.split("\n", 1)[1], text.split("\n", 1)[1])
            self.assertIn(f"cc={p} ", t.split("\n", 1)[0])

    def test_no_cc_means_the_primary_and_extra_flags_are_never_swept(self) -> None:
        text = HDR + "\nint f(void) { return 1; }\n"
        self.assertNotIn("gcc404xs-Os", [p for p, _t in P.profile_variants(text, self.LOCKED, "gcc404xs-Os")])
        extra = HDR.replace(" */", ' extra="-O1" */') + "\nint f(void) { return 1; }\n"
        self.assertEqual(P.profile_variants(extra, self.LOCKED, "gcc404xs-Os"), [])


class ProposalTests(unittest.TestCase):
    def test_proposal_is_written_under_the_run_not_src(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            src = Path(d) / "m.c"
            src.write_text(HDR + "\nint f(void) { return 1; }\n", encoding="utf-8")
            out = P.write_proposal("t1", Path(d) / "exact", src, "wip/FUN_08800000.c", "gcc171-O2")
            self.assertEqual(out.parent, Path(d) / "exact")
            text = out.read_text(encoding="utf-8")
            self.assertTrue(text.startswith(HDR))
            self.assertIn("// proposal: lbpperm.py tag t1", text)
            self.assertIn("cc=gcc171-O2", text)


if __name__ == "__main__":
    unittest.main()
