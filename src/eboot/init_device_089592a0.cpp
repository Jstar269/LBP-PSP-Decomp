/* LBPMATCH module=EBOOT.BIN start=0x089592a0 end=0x08959338 symbol=FUN_089592a0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Dev {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12();
    virtual int query(int what, int mask);
    virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17();
    virtual void enable(int on);
};
extern "C" void *FUN_08a9e824(unsigned);
extern "C" Dev *FUN_08abf8ac(int);
extern "C" void FUN_08aba7b8(Dev *, int, void *, unsigned);
struct S {
    void *buf;
    Dev *dev;
    int f8;
};
extern "C" void FUN_089592a0(S *s, unsigned n)
{
    s->f8 = 0;
    s->buf = FUN_08a9e824(n);
    Dev *d = FUN_08abf8ac(796);
    FUN_08aba7b8(d, 0, s->buf, n);
    s->dev = d;
    d->enable(1);
    s->f8 = s->dev->query(15, (int)0xffff0000);
}
