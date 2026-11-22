/* LBPMATCH module=EBOOT.BIN start=0x088c6c74 end=0x088c6d5c symbol=FUN_088c6c74 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Global {
    unsigned char pad1[0x614];
    int lock;
    unsigned char pad2[0x6f4 - 0x618];
    unsigned char flag;
};

struct Obj {
    unsigned char pad1[0x60];
    int state;
    unsigned char pad2[0x8c - 0x64];
    unsigned int flags;
};

extern struct Global *DAT_08bedb1c;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_088c60ec(void);
extern void FUN_088c6b30(struct Obj *);
extern void FUN_088c6818(struct Obj *);

void FUN_088c6c74(struct Obj *self)
{
    unsigned int f = self->flags;
    if ((f & 2) != 0) {
        if ((f & 4) != 0 && (f & 0x20) != 0) {
            struct Global *g = DAT_08bedb1c;
            FUN_08ad3088(&g->lock);
            g->flag = 0;
            FUN_08ad2810(&g->lock);
            self->state = 0x2f;
        } else if ((f & 4) != 0 && (f & 0x40) != 0) {
            FUN_088c60ec();
            self->state = 0x32;
            FUN_088c6b30(self);
        } else if ((f & 4) != 0 || (f & 8) != 0 || (f & 0x10) != 0) {
            self->flags = f & ~0xfeU;
            FUN_088c6818(self);
        }
    }
}

// Remaining diffs (57/58 words; 16 differ): exact match through +0xb0 (including ins v1,zero,1,7); differs only because GCC eliminates redundant `move a0,s2` before `j 0x88c6818`.
// near miss from lane oc-lbp-20261121 (unreviewed)
