/* LBPMATCH module=EBOOT.BIN start=0x0894b790 end=0x0894b888 symbol=FUN_0894b790 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Stream {
    int pad0;
    unsigned char *buf;
    unsigned char *cur;
    unsigned high;
    unsigned pos;
    void put(const void *pv)
    {
        if (buf) {
            *cur = *(const unsigned char *)pv;
            unsigned p = pos + 1;
            cur = buf + p;
            pos = p;
        } else {
            unsigned p = pos + 1;
            pos = p;
            if (high < p)
                high = p;
        }
    }
};
struct Obj {
    void **vt;
};
struct VecO { Obj **start; Obj **finish; Obj **eos; };
struct A {
    int pad0, pad1, pad2;
    VecO vec;
    unsigned align;
    unsigned f28;
    unsigned f32;
    unsigned f36;
};
extern "C" void FUN_0894b790(A *a, Stream *s, int arg)
{
    unsigned char z = 0;
    unsigned pos;
    unsigned al = a->align;
    while ((pos = s->pos) % al != 0)
        s->put(&z);
    a->f28 = pos;
    VecO *vp = &a->vec;
    for (Obj **p = vp->start; p != vp->finish; p++)
        ((void (*)(Obj *, Stream *, int))(*p)->vt[2])(*p, s, arg);
    a->f32 = s->pos - a->f28;
}
