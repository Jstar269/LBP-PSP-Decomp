/* LBPMATCH module=EBOOT.BIN start=0x08946610 end=0x089466cc symbol=FUN_08946610 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern int FUN_08baa93c(void);
extern int FUN_08baa954(void);
extern int FUN_08baa96c(int);
struct Inner { char pad[8]; struct Tgt *t; };
struct Tgt { char pad[16]; int v; };
struct Mid { char pad[8]; struct Tgt *t; };
struct O {
    char pad[32];
    int x20;
    char pad1[1452];
    int x5d0;
    char pad2[48];
    int state;
    char pad3[8];
    struct Mid *mid;
};
void FUN_08946610(struct O *o)
{
    int k = FUN_08baa93c();
    int a;
    if (k == 3) {
        int v = FUN_08baa954();
        if (v == 0) {
            o->state = 4;
            return;
        }
        a = (v == 2) ? o->x5d0 : o->x20;
    } else if (k == 2) {
        int v = FUN_08baa96c(2);
        if (v == 0)
            return;
        a = (v == k) ? o->x5d0 : o->x20;
    } else {
        struct Mid *m = o->mid;
        o->state = 5;
        m->t->v = o->x20;
        return;
    }
    {
        struct Mid *m = o->mid;
        o->state = 5;
        m->t->v = a;
    }
}
// NEAR 24 (48 of 47 words): logic, offsets and the retail delay-slot lw's reproduce; retail places the s1!=2&&s1!=3 block LAST, after the shared tail (lw v0,1552; li 5; sw state; ...), ours emits it before the tail. Tried: if/else-if/else, switch with default last, (k==3
// 
// k==2) wrapper, else folded into a (all differ in layout).
// near miss from lane api-lbp-a-20261121 (unreviewed)
