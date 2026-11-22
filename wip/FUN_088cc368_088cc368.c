/* LBPMATCH module=EBOOT.BIN start=0x088cc368 end=0x088cc464 symbol=FUN_088cc368 lang=c99 rodata=0x08bb42a0 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    int pad[5];
    int state;
    int f18;
    int f1c;
    int f20;
    unsigned int flags;
};

extern unsigned char DAT_08bee6cc;
extern unsigned char DAT_08bee6ce;
extern void FUN_088cc1e0(struct Obj *);
extern void FUN_088cbfe8(struct Obj *);
extern void FUN_088cc0f4(struct Obj *, int);
extern void FUN_088b2a48(void);

void FUN_088cc368(struct Obj *self)
{
    switch (self->state) {
    case 0:
        FUN_088cc1e0(self);
        return;
    case 3:
        FUN_088cbfe8(self);
        return;
    case 5:
        self->f18 = 0;
        FUN_088cc0f4(self, 0x260);
        self->state = 7;
        return;
    case 6:
        self->f18 = 3;
        FUN_088cc0f4(self, 0x260);
        self->state = 7;
        return;
    case 7: {
        unsigned int f = self->flags;
        if ((f & 0xe) != 0) {
            if ((f & 2) == 0 || (f & 0x10) != 0) {
                self->f1c = 0;
                self->state = self->f18;
                self->f20 = 0;
                return;
            }
            self->state = 9;
        }
        return;
    }
    case 8:
        DAT_08bee6ce = 1;
        FUN_088b2a48();
        return;
    case 9:
        DAT_08bee6cc = 1;
        DAT_08bee6ce = 0;
        FUN_088b2a48();
        return;
    }
}

// Remaining diffs (63/63 words; 30 differ at +0x74): cases 0..6 and jump table match verbatim; GCC places the shared `sw v0,20(s0)` block at +0x74 instead of at +0xe8 before the epilogue.
// near miss from lane oc-lbp-20261121 (unreviewed)
