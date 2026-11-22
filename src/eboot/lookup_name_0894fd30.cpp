/* LBPMATCH module=EBOOT.BIN start=0x0894fd30 end=0x0894fdf8 symbol=FUN_0894fd30 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Slice { int a, b; };
struct Obj { char pad[8]; int f8; };
struct Self {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5();
    virtual int use(Obj *o);
};
extern "C" void FUN_08a9c304(void *, Slice *);
extern "C" void FUN_08a9bff8(void *);
extern "C" int FUN_08aaded8();
extern "C" void *FUN_08ab2a68(int, int, char *, int, Obj **);
extern "C" void *FUN_08ab3244(int, int, Obj *);
extern "C" void FUN_08aad534(void *, int);
struct Name {
    int a, b;
    char *s;
    int pad4;
    long long rest[2];
    Name(Slice sl) { FUN_08a9c304(this, &sl); }
    ~Name() { FUN_08a9bff8(this); }
};
extern "C" int FUN_0894fd30(Self *self, Slice *p)
{
    int r = 0;
    Name name(*p);
    int h = FUN_08aaded8();
    if (h != 0) {
        Obj *out = 0;
        FUN_08aad534(FUN_08ab2a68(h, 0, name.s, 1, &out), 0);
        if (out != 0) {
            r = self->use(out);
            FUN_08aad534(FUN_08ab3244(out->f8, 0, out), 0);
        }
    }
    return r;
}
