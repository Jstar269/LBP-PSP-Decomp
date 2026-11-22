/* LBPMATCH module=EBOOT.BIN start=0x089598ac end=0x08959974 symbol=FUN_089598ac lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
struct List { int pad; unsigned *cur; };
extern struct List *DAT_08bdc188;
extern char DAT_08bb8028[], DAT_08bb7fa8[];
void FUN_089598ac(int unused, unsigned char flag)
{
    struct List *l = DAT_08bdc188;
    unsigned *p = l->cur;
    unsigned a = (unsigned)DAT_08bb8028;
    unsigned b = (unsigned)DAT_08bb7fa8;
    unsigned wa = ((a >> 24) & 0xf) << 16 | 0x10000000;
    unsigned wb = ((b >> 24) & 0xf) << 16 | 0x10000000;
    unsigned ta = (a & 0xffffff) | 0x1000000;
    unsigned tb = (b & 0xffffff) | 0x1000000;
    l->cur = p + 1;
    *p = 0xcb000000;
    if (flag) {
        p[4] = 0x4040004;
        l->cur = p + 5;
        p[1] = 0x120001e3;
        p[2] = wa;
        p[3] = ta;
    } else {
        p[4] = 0x4040004;
        l->cur = p + 5;
        p[1] = 0x120001e3;
        p[2] = wb;
        p[3] = tb;
    }
}
// NEAR 49 as counted (48 of 50 words; the real gap is two words): the GE command writer is right (ext/ins of the texture symbol addresses: ((a>>24)&0xf)<<16
// 0x10000000 and (a&0xffffff)
// 0x1000000 reproduce retail's ext/sll/ins/or sequence). Retail materialises the two constants with a fresh 'lui v0,0x1000'/'lui v0,0x100' before each OR (4 luis, struct pointer in t2, cursor in t0), ours CSEs them into t0/t1 (2 luis) and shifts the struct pointer to t3.
// near miss from lane api-lbp-a-20261121 (unreviewed)
