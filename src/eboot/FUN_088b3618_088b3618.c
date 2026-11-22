/* LBPMATCH module=EBOOT.BIN start=0x088b3618 end=0x088b36c0 symbol=FUN_088b3618 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    void (**vt)(struct Item *);
    unsigned char pad[13];
    unsigned char f11;
};

struct SharedPtr {
    struct Item *p;
    int *rc;
};

struct Obj {
    void (**vt)(struct SharedPtr *, struct Obj *);
    int pad1[4];
    int lock;
    unsigned char pad2[0xa9 - 0x18];
    unsigned char flag;
};

extern void FUN_08ad3088(void *);
extern int FUN_08ad2cf0(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08804f3c(struct SharedPtr *);
extern void FUN_08804f34(void) __attribute__((weak));

void FUN_088b3618(struct Obj *self)
{
    struct SharedPtr sp;

    if (self->flag != 0) {
        if (FUN_08ad2cf0(&self->lock) != 0) {
            return;
        }
    } else {
        FUN_08ad3088(&self->lock);
    }
    self->vt[14](&sp, self);
    if (sp.p != 0 && FUN_08804f34 != 0 && sp.p->f11 == 0) {
        sp.p->vt[9](sp.p);
    }
    FUN_08ad2810(&self->lock);
    FUN_08804f3c(&sp);
}
