/* LBPMATCH module=EBOOT.BIN start=0x08923050 end=0x089230b4 symbol=_ZThn8_N12Obj_0892305cD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_0892305c; exact match verified; names and types await human review. */
/* Obj_0892305c derives from PA (0x08bb6ae8, inline destructor over the root class whose destructor is 0x0894a260) and PB (destructor 0x0894a260, the second base at +8); vtable 0x08bb6ab0 with 8 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
    virtual void a1();
    virtual void a2();
    virtual void a3();
    virtual void a4();
    virtual void a5();
    virtual void a6();
    char data[4];
};
struct PA_08bb6af0 : Root_0894a260 {
    virtual ~PA_08bb6af0() {}
    virtual void a1();
};
struct PB_0894a260 {
    virtual ~PB_0894a260();
    char data[4];
};
struct Obj_0892305c : PA_08bb6af0, PB_0894a260 {
    virtual void a1();
};
void Obj_0892305c::a1()
{
}
