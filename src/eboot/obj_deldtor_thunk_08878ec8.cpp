/* LBPMATCH module=EBOOT.BIN start=0x08878ec8 end=0x08878f38 symbol=_ZThn12_N12Obj_08878ed4D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08878ed4; exact match verified; names and types await human review. */
/* Obj_08878ed4 derives from PA_08bb0848 (vtable 0x08bb0840, over the root class) and PB_08bb0828 (vtable 0x08bb0820, inline destructor) and holds a member at +24 destroyed by 0x0888b898; vtable 0x08bb07c8 with 11 primary slots. */
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
};
struct PA_08bb0848 : Root_0894a260 {
    virtual ~PA_08bb0848() {}
    virtual void a1();
    char data[8];
};
struct PB_08bb0828 {
    virtual ~PB_08bb0828() {}
    virtual void b1();
    char data[8];
};
struct Mem_0888b898 {
    virtual ~Mem_0888b898();
};
struct Obj_08878ed4 : PA_08bb0848, PB_08bb0828 {
    virtual void a1();
    Mem_0888b898 member;
};
void Obj_08878ed4::a1()
{
}
