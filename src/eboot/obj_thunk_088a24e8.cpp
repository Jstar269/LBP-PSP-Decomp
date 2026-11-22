/* LBPMATCH module=EBOOT.BIN start=0x088a24e8 end=0x088a24f4 symbol=_ZThn12_N12Obj_088a24f4D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk (addiu a0,a0,-12; j next; nop) that precedes the function at Obj_088a24f4 (modelled as the D1 thunk of a class with two polymorphic bases); exact match verified; names and types await human review. */
/* The range is the thunk alone: its target 0x088a24f4 is the next word (a separate inventory range, modelled here as the D1 body). Obj_088a24f4 derives from two polymorphic classes with inline destructors, so the second vptr sits at +12 and the vtable entries reached through it are thunks; GCC emits the thunk in .gnu.linkonce.t.D1 just before the D1 body, laid out at the thunk retail address by tools/lbpmatch.py. The key function is defined so that the vtable is emitted here. */
struct PA_088a24e8 {
    virtual ~PA_088a24e8() {}
    virtual void a1();
    char data[8];
};
struct PB_088a24e8 {
    virtual ~PB_088a24e8() {}
    virtual void b1();
};
struct Obj_088a24f4 : PA_088a24e8, PB_088a24e8 {
    virtual void a1();
};
void Obj_088a24f4::a1()
{
}
