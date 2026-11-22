/* LBPMATCH module=EBOOT.BIN start=0x08877bbc end=0x08877cbc symbol=FUN_08877bbc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0758;
extern int DAT_08bef8f4;

extern void FUN_08a9e0c4(void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_088050b8(void *);
extern void FUN_0886cd04(void *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    void *vt;
    unsigned char pad[44];
    void *f30;
    struct Ref *r1;
    struct Ref *r2;
    void *f3c;
    int f40;
    void *f44;
};

void FUN_08877bbc(struct Obj *self)
{
    struct Ref *r;

    self->vt = &DAT_08bb0758;
    if (self->f44 != 0) {
        FUN_08a9e0c4(self->f44);
    }
    if (self->f3c != 0) {
        FUN_08a9e0c4(self->f3c);
    }
    r = self->r2;
    if (r != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[1](r);
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
    if (self->f30 != 0) {
        FUN_088050b8(self->f30);
    }
    FUN_0886cd04(self);
}
