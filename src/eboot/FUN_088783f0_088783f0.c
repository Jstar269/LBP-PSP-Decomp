/* LBPMATCH module=EBOOT.BIN start=0x088783f0 end=0x088784ac symbol=FUN_088783f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0758;

extern void FUN_0886e018(void *, int);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_088050b8(void *);

struct Ref2 {
    int pad1[2];
    int count;
    int pad2[2];
    int lock;
};

struct Obj {
    void *vt;
    unsigned char pad[44];
    struct Ref2 *r0;
    int f34;
    int f38;
    int f3c;
    int f40;
    int f44;
    int f48;
    int f4c;
};

void FUN_088783f0(struct Obj *self, struct Ref2 **param_2)
{
    struct Ref2 *r;
    struct Ref2 *old;

    FUN_0886e018(self, 4);
    self->r0 = 0;
    self->vt = &DAT_08bb0758;
    self->f34 = 0;
    self->f38 = 0;
    self->f3c = 0;
    self->f40 = 0;
    self->f44 = 0;
    self->f48 = 0;
    self->f4c = 0;
    r = *param_2;
    if (r != 0) {
        FUN_08ad3088(&r->lock);
        r->count++;
        FUN_08ad2810(&r->lock);
    }
    old = self->r0;
    self->r0 = r;
    if (old != 0) {
        FUN_088050b8(old);
    }
}
