/* LBPMATCH module=EBOOT.BIN start=0x08aa77ec end=0x08aa7814 symbol=_ZThn12_N12Obj_08aa77f8D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08aa77f8; exact match verified; names and types await human review. */
/* Obj_08aa77f8 derives from two polymorphic classes with inline destructors (PA_08bab530, vtable 0x08bab528; PB_08bc3820, vtable 0x08bc3818), so the second vptr sits at +12 and the destructor is reached through a this-adjusting thunk. The implicit destructor resets both vptrs (the class's own vptr stores are dead). The key function is defined here so GCC emits the vtable with D1 and D0, each after its thunk in .gnu.linkonce.t.<D1|D0>. */
struct PA_08bab530 {
    virtual ~PA_08bab530() {}
    virtual void a1();
    char data[8];
};
struct PB_08bc3820 {
    virtual ~PB_08bc3820() {}
    virtual void b1();
};
struct Obj_08aa77f8 : PA_08bab530, PB_08bc3820 {
    virtual void a1();
};
void Obj_08aa77f8::a1()
{
}
