/* LBPMATCH module=EBOOT.BIN start=0x08868d78 end=0x08868e58 symbol=FUN_08868d78 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    int pad0;
    struct Ref *r2;
    int pad8;
    struct Ref *r1;
};

void FUN_08868d78(struct Obj *self)
{
    struct Ref *r;

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
    r = self->r2;
    if (r != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[2](r);
        }
    }
}
