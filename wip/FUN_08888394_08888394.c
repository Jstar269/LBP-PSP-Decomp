/* LBPMATCH module=EBOOT.BIN start=0x08888394 end=0x08888428 symbol=FUN_08888394 lang=c99 rodata=0x08bb0e38 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern void FUN_08baa134(void *);

union Time {
    unsigned long long u64;
    struct {
        volatile unsigned int lo;
        unsigned int hi;
    } p;
};

int FUN_08888394(unsigned long long *out)
{
    union Time t;
    unsigned int v;

    FUN_08baa134(&t);
    v = (t.p.hi << 23) | (t.p.lo >> 9);
    t.p.hi = 0;
    t.p.lo = v;
    t.p.lo = (unsigned int)(int)((float)v * 0.512f);
    *out = t.u64;
    return 1;
}

// NEAR: 37/37 words; 6 differ at +0x18..+0x40.
// Remaining diff: volatile on t.p.lo forces loading lo before hi and storing v1 before bltz instead of in bltz's delay slot;
// without volatile GCC deletes the redundant sw v1,0(sp) (36/37 words).
// Ruled out: plain struct Time (38/37), plain unsigned long long (33/37), non-volatile union (36/37).
// near miss from lane oc-lbp-20261121 (unreviewed)
