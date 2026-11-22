/* LBPMATCH module=EBOOT.BIN start=0x0887e580 end=0x0887e610 symbol=FUN_0887e580 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern void *DAT_08bedba4;

extern void FUN_0888b8c4(void *);
extern void FUN_08baa4b4(int);
extern void FUN_0888b8cc(void *);
extern void FUN_0887c784(void *, void *);

struct Obj {
    int f0;
    int f4;
    unsigned char flag;
    unsigned char pad1[3];
    void *f0c;
    int pad2[3];
    int fd;
    int lock;
};

void FUN_0887e580(struct Obj *self)
{
    void *lock = &self->lock;
    int neg;

    FUN_0888b8c4(lock);
    neg = self->fd < 0;
    if (!neg) {
        FUN_08baa4b4(self->fd);
    }
    self->flag = 1;
    FUN_0888b8cc(lock);
    if (self->f0c != 0 && !neg) {
        FUN_0887c784(DAT_08bedba4, self->f0c);
    }
}

// NEAR: 36/36 words; 6 differ; first at +0x28.
// Remaining diff: GCC emits srl s2,v0,0x1f for fd < 0 whereas retail emits slti s2,v0,0,
// plus delay-slot ordering around bnez s2 at +0x58.
// Ruled out: C++ bool neg (6 differ), fd <= -1 (6 differ), CSE without bool (31 differ), gcc404xs-O2 (27 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
