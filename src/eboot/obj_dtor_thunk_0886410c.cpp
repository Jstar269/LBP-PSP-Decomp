/* LBPMATCH module=EBOOT.BIN start=0x0886410c end=0x08864168 symbol=_ZThn8_N12Obj_08864118D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_08864118; exact match verified; names and types await human review. */
/* Obj_08864118 derives from PA (0x08bafd90, inline destructor over the root class whose destructor is 0x0894a260) and PB (destructor 0x0894a260, the second base at +8); vtable 0x08bafd68 with 14 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
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
    virtual void a11();
    virtual void a12();
    char data[4];
};
struct PA_08bafd98 : Root_0894a260 {
    virtual ~PA_08bafd98() {}
    virtual void a1();
};
struct PB_0894a260 {
    virtual ~PB_0894a260();
    char data[4];
};
struct Obj_08864118 : PA_08bafd98, PB_0894a260 {
    virtual void a1();
};
void Obj_08864118::a1()
{
}
