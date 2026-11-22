/* LBPMATCH module=EBOOT.BIN start=0x08865d64 end=0x08865e7c symbol=_ZThn12_N12Obj_08865d70D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08865d70; exact match verified; names and types await human review. */
/* Obj_08865d70 derives from Base_088794c4 (base-object destructor 0x088794c4, 200 bytes, vptrs at +0 and +12) and holds a member at +200 (vptr 0x08bafdc0, over the root class 0x0885c9f8) with a ref-counted pointer at +208; vtable 0x08bafde8 with 11 primary slots. */
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
struct Mem_08bafdc0 : Root_0885c9f8 {
    virtual ~Mem_08bafdc0() {}
    virtual void x2();
    char rest[204];
    Ref r;
};
struct Obj_08865d70 : Base_088794c4 {
    virtual void a1();
    Mem_08bafdc0 member;
    char pad2[20];
    Ref cr;
};
void Obj_08865d70::a1()
{
}
