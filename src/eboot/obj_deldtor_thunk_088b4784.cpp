/* LBPMATCH module=EBOOT.BIN start=0x088b4784 end=0x088b47fc symbol=_ZThn8_N12Obj_088b4790D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_088b4790; exact match verified; names and types await human review. */
/* Obj_088b4790 derives from PA_08bb3320 (vtable 0x08bb3318, over the root class) and PB_08bb3338 (vtable 0x08bb3330, over the root class whose destructor is 0x0894a260) and holds a member at +12 destroyed by 0x08804f3c; vtable 0x08bb32f0 with 3 primary slots. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
    virtual void a1();
};
struct PA_08bb3320 : Root_0894a260 {
    virtual ~PA_08bb3320() {}
    virtual void a1();
    char data[4];
};
struct PB_08bb3338 : Root_0894a260 {
    virtual ~PB_08bb3338() {}
    virtual void b1();
    char data[0];
};
struct Mem_08804f3c {
    virtual ~Mem_08804f3c();
};
struct Obj_088b4790 : PA_08bb3320, PB_08bb3338 {
    virtual void a1();
    Mem_08804f3c member;
};
void Obj_088b4790::a1()
{
}
