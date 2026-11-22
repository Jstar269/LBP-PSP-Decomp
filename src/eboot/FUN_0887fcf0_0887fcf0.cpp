/* LBPMATCH module=EBOOT.BIN start=0x0887fcf0 end=0x0887fdd0 symbol=FUN_0887fcf0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Ref {
    void (**vt)(Ref *);
    int rc;
};

struct SharedPtr {
    Ref *p;
    inline SharedPtr &operator=(const SharedPtr &o)
    {
        Ref *r = o.p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            r->rc++;
            FUN_08ad2810(&DAT_08bef8f4);
        }
        Ref *old = p;
        p = r;
        if (old != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --old->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                old->vt[2](old);
            }
        }
        return *this;
    }
};

struct Data {
    int f0;
    SharedPtr ref;
    int f8;
};

struct Src {
    unsigned char pad[668];
    Data d;
};

struct Dst {
    unsigned char pad[28];
    Data d;
};

struct Obj {
    unsigned char pad1[32];
    Src *src;
    int f24;
    Dst *dst;
};

extern "C" int FUN_0887fcf0(Obj *self)
{
    self->dst->d = self->src->d;
    return 1;
}
