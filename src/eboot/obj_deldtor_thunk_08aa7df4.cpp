/* LBPMATCH module=EBOOT.BIN start=0x08aa7df4 end=0x08aa7e58 symbol=_ZThn12_N12Obj_08aa7e00D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08aa7e00; exact match verified; names and types await human review. */
/* Obj_08aa7e00 derives from Base_08aa7dd8 (base-object destructor 0x08aa7dd8, two vptrs at +0 and +12) and frees an array at +32 with delete[] in its inline destructor; vtable 0x08bcaf20 with 17 primary slots. */
struct PA_08aa7dd8 {
    virtual ~PA_08aa7dd8();
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
    virtual void a11();
    virtual void a12();
    virtual void a13();
    virtual void a14();
    virtual void a15();
    char data[8];
};
struct PB_08aa7dd8 {
    virtual ~PB_08aa7dd8();
    virtual void b1();
    char data[16];
};
struct Base_08aa7dd8 : PA_08aa7dd8, PB_08aa7dd8 {
    virtual ~Base_08aa7dd8();
};
struct Obj_08aa7e00 : Base_08aa7dd8 {
    virtual void a1();
    char *buf;
    ~Obj_08aa7e00() { delete[] buf; }
};
void Obj_08aa7e00::a1()
{
}
