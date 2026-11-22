/* LBPMATCH module=EBOOT.BIN start=0x0895759c end=0x089575f4 symbol=_ZThn4_N12Obj_089575a8D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_089575a8; exact match verified; names and types await human review. */
/* Obj_089575a8 derives from PA (0x08bb7a40, inline destructor) and PB (destructor 0x08950404, the second base at +4); vtable 0x08bb7d98 with 11 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
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
struct Obj_089575a8 : PA_08bb7a48, PB_08950404 {
    virtual void a1();
};
void Obj_089575a8::a1()
{
}
