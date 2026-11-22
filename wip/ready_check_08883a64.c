/* LBPMATCH module=EBOOT.BIN start=0x08883a64 end=0x08883b08 symbol=FUN_08883a64 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_088829e8(void) __attribute__((weak));
extern int FUN_08849ff0(void *);
struct VT { void *s[16]; };
struct Sub { int a; int x; };
struct G { struct VT *vt; int pad[2]; struct Sub sub; };
extern struct G *DAT_08bedfb4;
unsigned char FUN_08883a64(void)
{
    if (DAT_08bedfb4 && FUN_088829e8 && FUN_08849ff0(DAT_08bedfb4))
        return ((unsigned char (*)(struct G *))DAT_08bedfb4->vt->s[11])(DAT_08bedfb4);
    struct G *g = DAT_08bedfb4;
    return !g || (g->sub.x < 4 ? g->sub.x == 0 : 1);
}
// NEAR 24 words (37 of 41 words): retail keeps the null check of the reloaded global in a separate block (lui/lw v0; bnezl; addiu v1,v0,12; j return-1) and reads sub->x twice via v1 (no CSE; volatile makes it 38 words but not the same), and merges the result through li v1/andi v0,v1,0xff.
// Weak function address test used: extern void FUN_088829e8(void) __attribute__((weak)); if (g && FUN_088829e8 && FUN_08849ff0(g)).
// near miss from lane api-lbp-a-20261121 (unreviewed)
