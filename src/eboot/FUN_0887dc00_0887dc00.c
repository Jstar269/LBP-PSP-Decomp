/* LBPMATCH module=EBOOT.BIN start=0x0887dc00 end=0x0887dcf4 symbol=FUN_0887dc00 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_08a9e0c4(void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    int f0;
    void *f4;
    unsigned char pad[324 - 8];
    struct Ref *r1;
    struct Ref *r2;
};

void FUN_0887dc00(struct Obj *self)
{
    struct Ref *r;

    if (self->f4 != 0) {
        FUN_08a9e0c4(self->f4);
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
}
