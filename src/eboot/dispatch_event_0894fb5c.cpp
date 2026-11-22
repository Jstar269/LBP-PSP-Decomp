/* LBPMATCH module=EBOOT.BIN start=0x0894fb5c end=0x0894fbf4 symbol=FUN_0894fb5c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj { char pad[8]; int f8; char pad1[60]; int f72; };
struct Self {
    virtual void v0(); virtual void v1();
    virtual int get(int x);
    virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6();
    virtual void v7(); virtual void v8();
    virtual unsigned kind();
};
extern "C" void *FUN_08ab2c20(int, int, Obj *, int, unsigned long long, long long);
extern "C" void FUN_08aad534(void *, int);
extern "C" bool FUN_0894fb5c(Self *self, Obj *o)
{
    bool r = false;
    if (o) {
        int h = self->get(o->f72);
        unsigned long long k = self->kind();
        void *r2 = FUN_08ab2c20(o->f8, 0, o, h, k, -1LL);
        FUN_08aad534(r2, 0);
        r = true;
    }
    return r;
}
