/* LBPMATCH module=EBOOT.BIN start=0x08a836b4 end=0x08a836c0 symbol=_ZThn152_N12Obj_08a836c0D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk (addiu a0,a0,-152; j next; nop) that precedes the function at Obj_08a836c0 (modelled as the D1 thunk of a class with two polymorphic bases); exact match verified; names and types await human review. */
/* The range is the thunk alone: its target 0x08a836c0 is the next word (a separate inventory range, modelled here as the D1 body). Obj_08a836c0 derives from two polymorphic classes with inline destructors, so the second vptr sits at +152 and the vtable entries reached through it are thunks; GCC emits the thunk in .gnu.linkonce.t.D1 just before the D1 body, laid out at the thunk retail address by tools/lbpmatch.py. The key function is defined so that the vtable is emitted here. */
struct PA_08a836b4 {
    virtual ~PA_08a836b4() {}
    virtual void a1();
    char data[148];
};
struct PB_08a836b4 {
    virtual ~PB_08a836b4() {}
    virtual void b1();
};
struct Obj_08a836c0 : PA_08a836b4, PB_08a836b4 {
    virtual void a1();
};
void Obj_08a836c0::a1()
{
}
