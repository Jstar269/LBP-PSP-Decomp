/* LBPMATCH module=EBOOT.BIN start=0x08932618 end=0x0893275c symbol=_ZThn8_N12Obj_08932624D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_08932624; exact match verified; names and types await human review. */
/* Obj_08932624 derives from PA_08bb6d18 (vtable 0x08bb6d10) and PB_08bb6d08 (vtable 0x08bb6d00), both with inline destructors over the root class 0x0894a260; members: a pointer at +12 released with FUN_088050b8 and a ref-counted pointer at +16; its destructor body releases and clears both; vtable 0x08bb6cd8 with 3 primary slots. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
struct Shared {
    virtual void v0();
    virtual void v1();
    virtual void destroy();
    int refs;
};
inline void release(Shared *o)
{
    if (o) {
        FUN_08ad3088(&DAT_08bef8f4);
        int r = o->refs - 1;
        o->refs = r;
        FUN_08ad2810(&DAT_08bef8f4);
        if (r == 0)
            o->destroy();
    }
}
struct Ref {
    Shared *p;
    void reset()
    {
        Shared *y = p;
        p = 0;
        release(y);
    }
    ~Ref() { release(p); }
};
extern "C" void FUN_088050b8(void *);
struct Ptr {
    void *p;
    void reset()
    {
        void *x = p;
        p = 0;
        if (x)
            FUN_088050b8(x);
    }
    ~Ptr()
    {
        if (p)
            FUN_088050b8(p);
    }
};
struct RootA_0894a260 {
    virtual ~RootA_0894a260();
    virtual void a1();
    char data[4];
};
struct PA_08bb6d18 : RootA_0894a260 {
    virtual ~PA_08bb6d18() {}
    virtual void a1();
};
struct RootB_0894a260 {
    virtual ~RootB_0894a260();
};
struct PB_08bb6d08 : RootB_0894a260 {
    virtual ~PB_08bb6d08() {}
    virtual void b1();
    char data[0];
};
struct Obj_08932624 : PA_08bb6d18, PB_08bb6d08 {
    virtual void a1();
    Ptr q;
    Ref r;
    ~Obj_08932624()
    {
        q.reset();
        r.reset();
    }
};
void Obj_08932624::a1()
{
}
