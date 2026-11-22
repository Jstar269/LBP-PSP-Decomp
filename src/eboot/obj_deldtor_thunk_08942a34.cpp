/* LBPMATCH module=EBOOT.BIN start=0x08942a34 end=0x08942b00 symbol=_ZThn8_N12Obj_08942a40D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08942a40; exact match verified; names and types await human review. */
/* Obj_08942a40 derives from a first base (destructor 0x0894a260, inline with vtable 0x08bb7500) and PB_08bb74f8 (vtable 0x08bb74f0, inline destructor over the root class 0x0894a260, second base at +8) and holds 1 ref-counted pointer member(s) at +16 released with the global lock; vtable 0x08bb7438 with 3 primary slots. */
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
    ~Ref() { release(p); }
};
struct RootA_0894a260 {
    virtual ~RootA_0894a260();
    virtual void a1();
    char data[4];
};
struct PA_08bb7508 : RootA_0894a260 {
    virtual ~PA_08bb7508() {}
    virtual void a1();
};
struct Root_0894a260 {
    virtual ~Root_0894a260();
};
struct PB_08bb74f8 : Root_0894a260 {
    virtual ~PB_08bb74f8() {}
    virtual void b1();
    char data[4];
};
struct Obj_08942a40 : PA_08bb7508, PB_08bb74f8 {
    virtual void a1();
    Ref r0;
};
void Obj_08942a40::a1()
{
}
