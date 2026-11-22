/* LBPMATCH module=EBOOT.BIN start=0x08944a6c end=0x08944a78 symbol=_ZThn8_N12Obj_08944a78D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk (addiu a0,a0,-8; j next; nop) that precedes the function at Obj_08944a78 (modelled as the D1 thunk of a class with two polymorphic bases); exact match verified; names and types await human review. */
/* The range is the thunk alone: its target 0x08944a78 is the next word (a separate inventory range, modelled here as the D1 body). Obj_08944a78 derives from two polymorphic classes with inline destructors, so the second vptr sits at +8 and the vtable entries reached through it are thunks; GCC emits the thunk in .gnu.linkonce.t.D1 just before the D1 body, laid out at the thunk retail address by tools/lbpmatch.py. The key function is defined so that the vtable is emitted here. */
struct PA_08944a6c {
    virtual ~PA_08944a6c() {}
    virtual void a1();
    char data[4];
};
struct PB_08944a6c {
    virtual ~PB_08944a6c() {}
    virtual void b1();
};
struct Obj_08944a78 : PA_08944a6c, PB_08944a6c {
    virtual void a1();
};
void Obj_08944a78::a1()
{
}
