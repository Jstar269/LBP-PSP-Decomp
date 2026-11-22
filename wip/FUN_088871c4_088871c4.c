/* LBPMATCH module=EBOOT.BIN start=0x088871c4 end=0x088872a4 symbol=FUN_088871c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_0888b8c4(void *);
extern void FUN_08886ddc(void *, void *, void *, int, int, unsigned int);
extern void FUN_0888b8cc(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);

struct Ref {
    void *vt;
    int rc;
};

struct Obj {
    unsigned char pad[44];
    int id;
    int lock;
};

struct Ref **FUN_088871c4(struct Ref **out, struct Obj *self, void *p3, struct Ref **p4, unsigned char p5, unsigned char p6)
{
    void *lock = &self->lock;
    int id;
    unsigned int flags;
    struct Ref *r;

    FUN_0888b8c4(lock);
    id = self->id;
    flags = p5 ? 2 : 1;
    if (p6) {
        flags |= 4;
    }
    self->id = id + 1;
    FUN_08886ddc(self, p3, p4, 1, id, flags);
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

// NEAR: 56/56 words; 2 differ at +0x58..+0x5c.
// Remaining diff: instruction swap ori v0,t1,0x4; addiu v1,t0,1 vs retail addiu v1,t0,1; ori v0,t1,0x4.
// Ruled out: self->id++ before flags (6 differ), self->id = id + 1 between p5 and p6 (6 differ), lang=c++ bool params (identical 2 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
