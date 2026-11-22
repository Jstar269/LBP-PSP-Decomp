/* LBPMATCH module=EBOOT.BIN start=0x0886c86c end=0x0886c8fc symbol=FUN_0886c86c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" int FUN_08b9e714(const void *, const void *, unsigned);

struct Key {
    int f0;
    char *data;
    int num;
};

extern "C" bool FUN_0886c86c(const Key *a, const Key *b)
{
    if (a->num != 0 || b->num != 0) {
        return a->f0 == b->f0 && a->num == b->num;
    }
    if (a->f0 != 0 && b->f0 != 0 && a->f0 != b->f0) {
        return false;
    }
    return FUN_08b9e714(a->data + 12, b->data + 12, 20) == 0;
}
// 19 differing words (34/36): first half (0x0886c86c..0x0886c8ac) matches;
// retail places the `a->f0 != 0` branch block out of line at the end (0x0886c8e0) with `bnezl` back to 0x0886c8b0.
// Ruled out: C99 (25/32), C++ reversed if (30), O2 (31).
// near miss from lane oc-lbp-20261121 (unreviewed)
