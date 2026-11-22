/* LBPMATCH module=EBOOT.BIN start=0x088637b4 end=0x08863818 symbol=_ZThn12_N12Obj_088637c0D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_088637c0; exact match verified; names and types await human review. */
/* Obj_088637c0 derives from Base_088794c4 (base-object destructor 0x088794c4, two vptrs at +0 and +12) and frees an array at +208 with delete[] in its inline destructor; vtable 0x08bafd10 with 12 primary slots. */
struct PA_088794c4 {
    virtual ~PA_088794c4();
    virtual void a1();
    virtual void a2();
    virtual void a3();
    virtual void a4();
    virtual void a5();
    virtual void a6();
    virtual void a7();
    virtual void a8();
    virtual void a9();
    virtual void a10();
    char data[8];
};
struct PB_088794c4 {
    virtual ~PB_088794c4();
    virtual void b1();
    char data[184];
};
struct Base_088794c4 : PA_088794c4, PB_088794c4 {
    virtual ~Base_088794c4();
};
struct Obj_088637c0 : Base_088794c4 {
    virtual void a1();
    char pad[8];
    char *buf;
    ~Obj_088637c0() { delete[] buf; }
};
void Obj_088637c0::a1()
{
}
