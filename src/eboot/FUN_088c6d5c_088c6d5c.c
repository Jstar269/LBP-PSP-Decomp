/* LBPMATCH module=EBOOT.BIN start=0x088c6d5c end=0x088c6dfc symbol=FUN_088c6d5c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Pad {
    int f0;
    unsigned int f4;
    unsigned int f8;
    unsigned char pad[0xb8 - 0x0c];
    unsigned int bit;
};

struct Obj {
    unsigned char pad[0x8c];
    unsigned int flags;
};

extern unsigned int DAT_08bee6b4;
extern unsigned int DAT_08bee6b8;
extern void FUN_089ecaa8(void);
extern struct Pad *FUN_089ea97c(void);
extern void FUN_08a2d874(unsigned int, unsigned int);
extern void FUN_088c2c70(void);

static inline unsigned int pad_pressed(struct Pad *p, unsigned int mask)
{
    return (~p->f8 & p->f4) & mask;
}

void FUN_088c6d5c(struct Obj *self)
{
    if ((~self->flags & 1) != 0) {
        struct Pad *p;
        FUN_089ecaa8();
        p = FUN_089ea97c();
        if (p != 0 && pad_pressed(p, 1 << p->bit) != 0) {
            self->flags |= 1;
            FUN_08a2d874(DAT_08bee6b4, DAT_08bee6b8);
            FUN_088c2c70();
        }
    }
}
