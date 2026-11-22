/* LBPMATCH module=EBOOT.BIN start=0x08866c48 end=0x08866d04 symbol=FUN_08866c48 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_088917a0(void);
extern void FUN_0885cacc(void *, void *);
extern char *FUN_08875f58(void *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_08847554(void *, char *, void *, int);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    unsigned char pad[0xd0];
    int list;
};

void FUN_08866c48(struct Obj *self)
{
    struct Ref *r;
    struct Ref *p_ref;
    int buf[3];
    char *p;

    FUN_088917a0();
    buf[0] = 0;
    FUN_0885cacc(&r, self);
    p = FUN_08875f58(r, buf);
    p_ref = r;
    if (p_ref != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --p_ref->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            p_ref->vt[1](p_ref);
        }
    }
    FUN_08847554(&self->list, p, 0, 1);
}
