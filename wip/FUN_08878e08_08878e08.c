/* LBPMATCH module=EBOOT.BIN start=0x08878e08 end=0x08878ea8 symbol=FUN_08878e08 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern unsigned char FUN_088825ec(int);
extern unsigned char FUN_08882600(int);

struct Sub {
    int pad[2];
    volatile unsigned char flag;
};

struct Obj {
    int (**vt)(struct Obj *);
    int pad1[2];
    struct Sub sub;
    unsigned char pad2[192 - 24];
    long long (*cb)(struct Obj *);
    int state;
};

int FUN_08878e08(struct Obj *self)
{
    struct Sub *sub = &self->sub;
    if (sub->flag == 0 || self->vt[5](self) == 0) {
        if (self->state != 3) {
            if (FUN_088825ec(self->state) == 0) {
                return FUN_08882600(self->state) == 0;
            }
        }
        return (int)self->cb(self);
    }
    self->vt[6](self);
    return 1;
}

// NEAR: 40/40 words; 19 differ; first at +0x30.
// Remaining diff: block ordering between vt[6]()/return 1, state == 3 check, and cb() tail block.
// Retail places vt[6]()/return 1 at +0x50 between state==3 jump and FUN_088825ec, and puts cb() at +0x88 falling into epilogue.
// Ruled out: putting flag&&vt[5] first in C99 (23 differ), C++ bool + Base/Sub inheritance (32 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
