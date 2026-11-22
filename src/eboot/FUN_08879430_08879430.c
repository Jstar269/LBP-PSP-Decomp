/* LBPMATCH module=EBOOT.BIN start=0x08879430 end=0x088794c4 symbol=FUN_08879430 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on sub->b8 (keeps sb out of jal delay slot); exact match verified; names and types await human review. */

extern int DAT_08bb07d0;
extern int DAT_08bb0804;

extern void FUN_0894a258(void *);
extern void FUN_0888b860(void *);

struct Sub {
    void *vt;
    int f4;
    volatile unsigned char b8;
    unsigned char pad[3];
};

struct Obj {
    void *vt0;
    int f4;
    int f8;
    struct Sub sub;
    int f18;
    unsigned char pad[188 - 28];
    int fbc;
    int fc0;
    int fc4;
};

void FUN_08879430(struct Obj *self, int p2, int p3, int p4)
{
    struct Sub *sub;

    FUN_0894a258(self);
    self->f4 = 1;
    self->vt0 = &DAT_08bb07d0;
    sub = &self->sub;
    sub->vt = &DAT_08bb0804;
    sub->f4 = 0;
    self->f8 = 0;
    sub->b8 = 0;
    FUN_0888b860(&self->f18);
    self->fc0 = p3;
    self->fbc = p2;
    *((unsigned char *)self + 21) = 0;
    self->fc4 = p4;
}
