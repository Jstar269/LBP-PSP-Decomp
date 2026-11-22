/* LBPMATCH module=EBOOT.BIN start=0x08863640 end=0x088636c8 symbol=_ZThn12_N12Obj_0886364cD1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_0886364c; exact match verified; names and types await human review. */
/* Obj_0886364c derives from Base_088794c4 (base-object destructor 0x088794c4, 200 bytes, vptrs at +0 and +12) and holds a member at +204 (vptr 0x08bafc28) whose inline destructor frees an array at +208 and then runs the middle (vptr 0x08baec60) and root (0x0885c9f8) destructors; vtable 0x08bafc50 with 12 primary slots. */
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
struct Mem_08bafc28 : Mid_08baec60 {
    virtual ~Mem_08bafc28() { delete[] buf; }
    virtual void x2();
    char rest[204];
    char *buf;
};
struct Obj_0886364c : Base_088794c4 {
    virtual void a1();
    char pad[4];
    Mem_08bafc28 member;
};
void Obj_0886364c::a1()
{
}
