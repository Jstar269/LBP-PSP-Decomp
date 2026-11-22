/* LBPMATCH module=EBOOT.BIN start=0x0885a4ac end=0x0885a518 symbol=_ZThn12_N12Obj_0885a4b8D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_0885a4b8; exact match verified; names and types await human review. */
/* Obj_0885a4b8 derives from Base_088794c4 (base-object destructor 0x088794c4, two vptrs at +0 and +12, 200 bytes) and holds a pointer to a polymorphic object deleted by its destructor; vtable 0x08baf488 with 11 primary slots. */
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
struct Elem_000000c8 {
    virtual ~Elem_000000c8();
};
struct Obj_0885a4b8 : Base_088794c4 {
    virtual void a1();
    Elem_000000c8 *p;
    ~Obj_0885a4b8() { delete p; }
};
void Obj_0885a4b8::a1()
{
}
