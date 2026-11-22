/* LBPMATCH module=EBOOT.BIN start=0x08881564 end=0x088815d4 symbol=_ZThn8_N12Obj_08881570D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_08881570; exact match verified; names and types await human review. */
/* Obj_08881570 derives from PA_08bb09d8 (vtable 0x08bb09d0, over the root class) and PB_08bb09e8 (vtable 0x08bb09e0, over the root class whose destructor is 0x0894a260) and holds a member at +12 destroyed by 0x0884d404; vtable 0x08bb09b0 with 2 primary slots. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
};
struct PA_08bb09d8 : Root_0894a260 {
    virtual ~PA_08bb09d8() {}
    char data[4];
};
struct PB_08bb09e8 : Root_0894a260 {
    virtual ~PB_08bb09e8() {}
    char data[0];
};
struct Mem_0884d404 {
    virtual ~Mem_0884d404();
};
struct Obj_08881570 : PA_08bb09d8, PB_08bb09e8 {
    Mem_0884d404 member;
};
/* No virtual function to serve as key function: the class has only its destructor in each vtable group, so the vtable (and D1/D0 after their thunks) is emitted where the class is instantiated. */
Obj_08881570 *Obj_08881570_make()
{
    return new Obj_08881570;
}
