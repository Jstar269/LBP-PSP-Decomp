/* LBPMATCH module=EBOOT.BIN start=0x0887ffec end=0x088800a8 symbol=FUN_0887ffec lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *DAT_08bb09d8[];
extern void *DAT_08bb09b8[];
extern void *DAT_08bb09c8[];

extern void FUN_0894a258(void *);
extern void FUN_0884d3bc(void *);
extern void FUN_0888851c(void *, void *, float, float, float);

struct Pair {
    int a;
    int b;
};

struct Obj {
    void **vt0;
    int f04;
    void **vt8;
    unsigned char sub0c[64];
    int f4c;
    int f50;
    int f54;
    unsigned char f58;
    unsigned char pad1[3];
    int f5c;
    int f60;
    int f64;
    int f68;
    int f6c;
    unsigned char f70;
    unsigned char f71;
    unsigned char f72;
    unsigned char pad2;
    struct Pair f74;
};

void FUN_0887ffec(struct Obj *self, struct Pair *param_2)
{
    FUN_0894a258(self);
    self->vt0 = DAT_08bb09d8;
    self->f04 = 1;
    FUN_0894a258(&self->vt8);
    self->vt0 = DAT_08bb09b8;
    self->vt8 = DAT_08bb09c8;
    FUN_0884d3bc(self->sub0c);
    self->f54 = 0;
    self->f58 = 0;
    self->f5c = 0;
    self->f60 = 0;
    self->f64 = 0;
    self->f68 = 0;
    self->f6c = 0;
    self->f74 = *param_2;
    self->f70 = 0;
    self->f71 = 0;
    self->f72 = 0;
    FUN_0888851c(&self->f4c, &self->f50, 1.0f, 0.0f, 0.0f);
}
