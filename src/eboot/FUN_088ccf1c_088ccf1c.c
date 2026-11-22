/* LBPMATCH module=EBOOT.BIN start=0x088ccf1c end=0x088ccfb0 symbol=FUN_088ccf1c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Pad {
    int f0;
    unsigned int f4;
    unsigned int f8;
    unsigned char pad[0xb8 - 0x0c];
    unsigned int bit;
};

struct Obj {
    int pad1[5];
    void *f14;
    int pad2[2];
    unsigned char flag;
};

extern void FUN_089ecaa8(void);
extern struct Pad *FUN_089ea97c(void);
extern void FUN_0883bd5c(void *, void *);
extern void FUN_088b2aa4(void *);
extern void FUN_08804f3c(void *);

static inline unsigned int pad_pressed(struct Pad *p, unsigned int mask)
{
    return (~p->f8 & p->f4) & mask;
}

void FUN_088ccf1c(struct Obj *self)
{
    if (self->flag != 0) {
        struct Pad *p;
        FUN_089ecaa8();
        p = FUN_089ea97c();
        if (p != 0 && pad_pressed(p, 1 << p->bit) != 0) {
            self->flag = 0;
            if (self->f14 != 0) {
                int sp[4];
                FUN_0883bd5c(sp, self->f14);
                FUN_088b2aa4(sp);
                FUN_08804f3c(sp);
            }
        }
    }
}
