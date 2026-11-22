/* LBPMATCH module=EBOOT.BIN start=0x08944ef0 end=0x08944fb0 symbol=_ZThn68_N12Obj_08944efcD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08944efc; exact match verified; names and types await human review. */
/* Obj_08944efc derives from a first base (destructor 0x08944cac) and PB_08bb7558 (vtable 0x08bb7550, inline destructor over the root class 0x0894a260, second base at +68) and holds 1 ref-counted pointer member(s) at +96 released with the global lock; vtable 0x08bb73c0 with 3 primary slots. */
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
struct PA_08944cac {
    virtual ~PA_08944cac();
    virtual void a1();
    char data[64];
};
struct Root_0894a260 {
    virtual ~Root_0894a260();
};
struct PB_08bb7558 : Root_0894a260 {
    virtual ~PB_08bb7558() {}
    virtual void b1();
    char data[24];
};
struct Obj_08944efc : PA_08944cac, PB_08bb7558 {
    virtual void a1();
    Ref r0;
};
void Obj_08944efc::a1()
{
}
