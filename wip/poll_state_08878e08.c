/* LBPMATCH module=EBOOT.BIN start=0x08878e08 end=0x08878ea8 symbol=FUN_08878e08 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern unsigned char FUN_088825ec(int);
extern unsigned char FUN_08882600(int);
struct O;
struct VT { void *s[32]; };
struct O {
    struct VT *vt;
    int pad[2];
    struct Sub { int a; int b; unsigned char flag; char p[3]; } sub;
    char pad2[168];
    unsigned char (*fn)(struct O *);
    int state;
};
static inline int Poll(struct O *o)
{
    if (o->sub.flag) {
        if (((int (*)(struct O *))o->vt->s[5])(o)) {
            ((void (*)(struct O *))o->vt->s[6])(o);
            return 1;
        }
    }
    return 0;
}
unsigned char FUN_08878e08(struct O *o)
{
    if (Poll(o))
        return 1;
    int st = o->state;
    if (st != 3) {
        if (!FUN_088825ec(st))
            return FUN_08882600(o->state) == 0;
    }
    return o->fn(o);
}
// NEAR: 37 words ours vs 40 retail. Retail: no tail call at the final fn(o) (a char-returning fn pointer type reproduces that),
// state load hoisted into beqzl delay slot, `addiu v0,a0,12; lbu 8(v0)` sub-object flag test (not folded), and the
// v6-call/return-1 block laid out AFTER the state==3 test (ours puts it first, fallthrough). Tried: inline helper, ok-flag,
// if/else with r, negated condition: all put the v6 block first.
// near miss from lane api-lbp-a-20261121 (unreviewed)
