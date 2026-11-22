/* LBPMATCH module=EBOOT.BIN start=0x0884daf4 end=0x0884db64 symbol=_ZThn8_N12Obj_0884db00D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_0884db00; exact match verified; names and types await human review. */
/* Obj_0884db00 derives from PA_08baee60 (vtable 0x08baee58, over the root class) and PB_08baee78 (vtable 0x08baee70, over the root class whose destructor is 0x0894a260) and holds a member at +12 destroyed by 0x0884d404; vtable 0x08baee30 with 3 primary slots. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
    virtual void a1();
};
struct PA_08baee60 : Root_0894a260 {
    virtual ~PA_08baee60() {}
    virtual void a1();
    char data[4];
};
struct PB_08baee78 : Root_0894a260 {
    virtual ~PB_08baee78() {}
    virtual void b1();
    char data[0];
};
struct Mem_0884d404 {
    virtual ~Mem_0884d404();
};
struct Obj_0884db00 : PA_08baee60, PB_08baee78 {
    virtual void a1();
    Mem_0884d404 member;
};
void Obj_0884db00::a1()
{
}
