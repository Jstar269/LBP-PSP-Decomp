/* LBPMATCH module=EBOOT.BIN start=0x088c6bd0 end=0x088c6c74 symbol=FUN_088c6bd0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    unsigned char pad1[0x60];
    int state;
    unsigned char pad2[0x8c - 0x64];
    unsigned int flags;
};

extern void FUN_088c60ec(void);
extern void FUN_088c6b30(struct Obj *);
extern void FUN_088c6630(struct Obj *);

void FUN_088c6bd0(struct Obj *self)
{
    unsigned int f = self->flags;
    if ((f & 2) != 0) {
        if ((f & 4) != 0 && (f & 0x20) != 0) {
            FUN_088c60ec();
            self->state = 0x2f;
        } else if ((f & 4) != 0 && (f & 0x40) != 0) {
            self->state = 0x32;
            FUN_088c60ec();
            FUN_088c6b30(self);
        } else if ((f & 4) != 0 || (f & 8) != 0 || (f & 0x10) != 0) {
            FUN_088c6630(self);
        }
    }
}

// Remaining diffs (40/41 words; 19 differ): GCC eliminates redundant `move a0,s0` before `j 0x88c6630` and swaps a2/a3 for the flag masks (same in C++).
// near miss from lane oc-lbp-20261121 (unreviewed)
