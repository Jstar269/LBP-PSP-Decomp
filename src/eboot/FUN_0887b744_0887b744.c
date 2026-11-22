/* LBPMATCH module=EBOOT.BIN start=0x0887b744 end=0x0887b830 symbol=FUN_0887b744 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0858;
extern int DAT_08bb0868;
extern int DAT_08bef8f4;

extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0894a260(void *);
extern void FUN_08a9e0cc(void *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    void *vt;
    unsigned char pad1[24];
    struct Ref *r1;
    int pad2;
    struct Ref *r2;
};

void FUN_0887b744(struct Obj *self)
{
    struct Ref *r;

    r = self->r2;
    self->vt = &DAT_08bb0858;
    if (r != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[2](r);
        }
    }
    r = self->r1;
    if (r != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[1](r);
        }
    }
    self->vt = &DAT_08bb0868;
    FUN_0894a260(self);
    FUN_08a9e0cc(self);
}
