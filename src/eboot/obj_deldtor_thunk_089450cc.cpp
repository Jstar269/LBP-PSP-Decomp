/* LBPMATCH module=EBOOT.BIN start=0x089450cc end=0x089451d8 symbol=_ZThn68_N12Obj_089450d8D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_089450d8; exact match verified; names and types await human review. */
/* Obj_089450d8 derives from a first base (destructor 0x08944cac) and PB_08bb7568 (vtable 0x08bb7560, inline destructor over the root class 0x0894a260, second base at +68) and holds 2 ref-counted pointer member(s) at +84,+88 released with the global lock; vtable 0x08bb7398 with 3 primary slots. */
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
struct PB_08bb7568 : Root_0894a260 {
    virtual ~PB_08bb7568() {}
    virtual void b1();
    char data[12];
};
struct Obj_089450d8 : PA_08944cac, PB_08bb7568 {
    virtual void a1();
    Ref r0;
    Ref r1;
};
void Obj_089450d8::a1()
{
}
