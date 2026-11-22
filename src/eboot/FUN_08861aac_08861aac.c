/* LBPMATCH module=EBOOT.BIN start=0x08861aac end=0x08861b40 symbol=FUN_08861aac lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bafb68;
extern int DAT_08bef8f4;

extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0885c9f8(void *);
extern void FUN_08a9e0cc(void *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    void *vt;
    unsigned char pad[460];
    struct Ref *ref;
};

void FUN_08861aac(struct Obj *self)
{
    struct Ref *r = self->ref;
    self->vt = &DAT_08bafb68;
    if (r != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[1](r);
        }
    }
    FUN_0885c9f8(self);
    FUN_08a9e0cc(self);
}
