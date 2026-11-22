/* LBPMATCH module=EBOOT.BIN start=0x089429d0 end=0x08942a34 symbol=_ZThn8_N12Obj_089429dcD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_089429dc; exact match verified; names and types await human review. */
/* Obj_089429dc derives from PA (0x08bb74d8, inline destructor over the root class whose destructor is 0x0894a260) and PB (destructor 0x0894a260, the second base at +8); vtable 0x08bb7460 with 24 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
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
    virtual void a13();
    virtual void a14();
    virtual void a15();
    virtual void a16();
    virtual void a17();
    virtual void a18();
    virtual void a19();
    virtual void a20();
    virtual void a21();
    virtual void a22();
    char data[4];
};
struct PA_08bb74e0 : Root_0894a260 {
    virtual ~PA_08bb74e0() {}
    virtual void a1();
};
struct PB_0894a260 {
    virtual ~PB_0894a260();
    char data[4];
};
struct Obj_089429dc : PA_08bb74e0, PB_0894a260 {
    virtual void a1();
};
void Obj_089429dc::a1()
{
}
