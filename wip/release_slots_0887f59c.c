/* LBPMATCH module=EBOOT.BIN start=0x0887f59c end=0x0887f644 symbol=FUN_0887f59c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_0887eef8(void) __attribute__((weak));
struct VT { void *s[4]; };
struct Sub { struct VT *vt; };
struct Obj { int pad[3]; struct Sub sub; };
struct A { struct Obj *obj; int pad[11]; };
struct B { struct Obj *obj; int pad[12]; };
struct Host {
    char pad0[44];
    int flag;
    char pad1[96];
    struct A a[23];
    char pad2[184];
    struct B b[23];
};
void FUN_0887f59c(struct Host *h)
{
    int i;
    h->flag = 1;
    for (i = 0; i != 23; i++) {
        struct Obj *p = h->a[i].obj;
        if (p && FUN_0887eef8)
            ((void (*)(struct Sub *))p->sub.vt->s[3])(&p->sub);
        p = h->b[i].obj;
        if (p && FUN_0887eef8)
            ((void (*)(struct Sub *))p->sub.vt->s[3])(&p->sub);
    }
}
// NEAR 5 words: retail strength-reduces both arrays to pointers that START at the array (addiu s1,a0,144 / addiu s0,a0,1432; lw 0(s1)); ours keeps base a0 with the offset folded into each lw (144(s1)).
// With explicit pa/pb locals and pa[i]/pb[i] the pointers match but 7 words differ: s0/s1 are swapped (retail A=s1, B=s0); declaration order, inline helper, do-while do not change that.
// The 'beqz v0' on a lui/addiu constant is a weak function address test (extern void FUN_0887eef8(void) __attribute__((weak)); if (p && FUN_0887eef8)).
// near miss from lane api-lbp-a-20261121 (unreviewed)
