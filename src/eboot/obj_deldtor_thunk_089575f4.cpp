/* LBPMATCH module=EBOOT.BIN start=0x089575f4 end=0x08957650 symbol=_ZThn4_N12Obj_08957600D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08957600; exact match verified; names and types await human review. */
/* Obj_08957600 derives from PA (0x08bb7a40, inline destructor) and PB (destructor 0x08950404, the second base at +4); vtable 0x08bb7d98 with 11 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
struct PA_08bb7a48 {
    virtual ~PA_08bb7a48() {}
    virtual void a1();
    virtual void a2();
    virtual void a3();
    virtual void a4();
    virtual void a5();
    virtual void a6();
    virtual void a7();
    virtual void a8();
    virtual void a9();
    char data[0];
};
struct PB_08950404 {
    virtual ~PB_08950404();
    char data[0];
};
struct Obj_08957600 : PA_08bb7a48, PB_08950404 {
    virtual void a1();
};
void Obj_08957600::a1()
{
}
