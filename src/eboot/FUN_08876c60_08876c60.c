/* LBPMATCH module=EBOOT.BIN start=0x08876c60 end=0x08876cf8 symbol=FUN_08876c60 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0718;

extern void FUN_0894a258(void *);
extern void FUN_088768e4(void *);

struct Obj {
    void *vt;
    int f4;
    int f8;
    unsigned char b0c;
    unsigned char pad1[3];
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    unsigned char b24;
    unsigned char b25;
    unsigned char pad2[2];
    int f28;
    int f2c;
    int f30;
};

void FUN_08876c60(struct Obj *self, int p2, unsigned char p3, int p4, int p5)
{
    FUN_0894a258(self);
    self->f4 = 1;
    self->vt = &DAT_08bb0718;
    self->f8 = 0;
    self->b0c = 0;
    FUN_088768e4(&self->f10);
    self->f18 = p2;
    self->b25 = p3;
    self->f28 = p4;
    self->f2c = p5;
    self->f30 = 0;
    self->f1c = 0;
    self->b24 = 0;
    self->f20 = 0;
}
