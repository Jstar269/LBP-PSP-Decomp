/* LBPMATCH module=EBOOT.BIN start=0x0894a7bc end=0x0894a7dc symbol=FUN_0894a7bc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp20f, Claude Opus 5.5); exact match verified; names and types await human review. */
struct S_0894a7bc {
    int a;
    int b;
    unsigned n;
};

/* the distance from x up to the next multiple of s->n (0 when x is one) */
unsigned FUN_0894a7bc(struct S_0894a7bc *s, unsigned x)
{
    unsigned r = x % s->n;
    return r ? s->n - r : 0;
}
