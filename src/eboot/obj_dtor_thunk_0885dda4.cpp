/* LBPMATCH module=EBOOT.BIN start=0x0885dda4 end=0x0885de00 symbol=_ZThn12_N12Obj_0885ddb0D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_0885ddb0; exact match verified; names and types await human review. */
/* Obj_0885ddb0 derives from Base_088794c4 (base-object destructor 0x088794c4, two vptrs at +0 and +12, 200 bytes) and holds a member at +236 whose inline destructor stores vptr 0x08baec60 and calls the root destructor 0x0885c9f8; vtable 0x08baf720 with 12 primary slots. */
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
    int x;
};
struct Mid_08baec60 : Root_0885c9f8 {
    virtual ~Mid_08baec60() {}
    virtual void m2();
};
struct Obj_0885ddb0 : Base_088794c4 {
    virtual void a1();
    char pad[36];
    Mid_08baec60 member;
};
void Obj_0885ddb0::a1()
{
}
