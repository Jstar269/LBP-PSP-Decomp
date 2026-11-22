/* LBPMATCH module=EBOOT.BIN start=0x08866170 end=0x088662b4 symbol=_ZThn12_N12Obj_0886617cD1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_0886617c; exact match verified; names and types await human review. */
/* Obj_0886617c derives from Base_088794c4 (base-object destructor 0x088794c4, 200 bytes, vptrs at +0 and +12) and holds, in order: a member at +204 (vptr 0x08baec60 over the root class 0x0885c9f8), a member at +412 (destructor 0x08865f30), ref-counted pointers at +628 and +632, and a buffer holder at +636 (vptr 0x08bae6e0, base destructor 0x08843f48) that frees a heap array at +52 unless it is the static buffer DAT_08bedb94; vtable 0x08bafea0 with 12 primary slots. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
struct Shared {
    virtual void v0();
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
extern "C" char DAT_08bedb94[];
struct PA_088794c4 {
    virtual ~PA_088794c4();
    virtual void a1();
    virtual void a2();
    virtual void a3();
    virtual void a4();
    virtual void a5();
    virtual void a6();
    virtual void a7();
    virtual void a8();
    virtual void a9();
    virtual void a10();
    char data[8];
};
struct PB_088794c4 {
    virtual ~PB_088794c4();
    virtual void b1();
    char data[184];
};
struct Base_088794c4 : PA_088794c4, PB_088794c4 {
    virtual ~Base_088794c4();
};
struct Root_0885c9f8 {
    virtual ~Root_0885c9f8();
};
struct Mid_08baec60 : Root_0885c9f8 {
    virtual ~Mid_08baec60() {}
    virtual void m2();
};
struct Mem_08865f30 {
    char data[216];
    ~Mem_08865f30();
};
struct WBase_08843f48 {
    virtual ~WBase_08843f48();
    char data[48];
};
struct W_08bae6e0 : WBase_08843f48 {
    char *buf;
    virtual ~W_08bae6e0() {
        if (buf != DAT_08bedb94 && buf)
            delete[] buf;
    }
};
struct Obj_0886617c : Base_088794c4 {
    virtual void a1();
    char pad[4];
    Mid_08baec60 member;
    char pad2[204];
    Mem_08865f30 m2;
    Ref r0;
    Ref r1;
    W_08bae6e0 w;
};
void Obj_0886617c::a1()
{
}
