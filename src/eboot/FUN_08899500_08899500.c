/* LBPMATCH module=EBOOT.BIN start=0x08899500 end=0x088995d8 symbol=FUN_08899500 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_0888b8c4(void *);
extern void FUN_08899118(void *, void *, void *, int, int, unsigned int);
extern void FUN_0888b8cc(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);

struct Ref {
    void *vt;
    int rc;
};

struct Obj {
    unsigned char pad[44];
    int lock;
};

struct Ref **FUN_08899500(struct Ref **out, struct Obj *self, void *p3, struct Ref **p4, unsigned char p5, unsigned char p6)
{
    void *lock = &self->lock;
    unsigned int flags;
    struct Ref *r;

    FUN_0888b8c4(lock);
    flags = p5 ? 2 : 1;
    if (p6) {
        flags |= 4;
    }
    FUN_08899118(self, p3, p4, 1, 0, flags);
    FUN_0888b8cc(lock);
    r = *p4;
    *out = r;
    if (r != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        r->rc++;
        FUN_08ad2810(&DAT_08bef8f4);
    }
    return out;
}
