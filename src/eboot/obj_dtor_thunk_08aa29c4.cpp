/* LBPMATCH module=EBOOT.BIN start=0x08aa29c4 end=0x08aa2a74 symbol=_ZThn12_N12Obj_08aa29d0D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_08aa29d0; exact match verified; names and types await human review. */
/* Obj_08aa29d0 derives from PA_08bab530 (vtable 0x08bab528) and PB_08bc3820 (vtable 0x08bc3818), both with inline destructors; the destructor releases two buffers at +32 and +36 when flag bit 0 of the byte at +52 is set: with FUN_08a9e11c when bit 1 is set, otherwise with delete[]; vtable 0x08bcadf8 with 19 primary slots. */
extern "C" void FUN_08a9e11c(void *);
struct PA_08bab530 {
    virtual ~PA_08bab530() {}
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
    virtual void a16();
    virtual void a17();
    char data[8];
};
struct PB_08bc3820 {
    virtual ~PB_08bc3820() {}
    virtual void b1();
    char data[16];
};
struct Obj_08aa29d0 : PA_08bab530, PB_08bc3820 {
    virtual void a1();
    char *a;
    char *b;
    char pad[12];
    unsigned char flags;
    ~Obj_08aa29d0()
    {
        if (flags & 1) {
            if (flags & 2) {
                FUN_08a9e11c(a);
                FUN_08a9e11c(b);
            } else {
                delete[] a;
                delete[] b;
            }
        }
    }
};
void Obj_08aa29d0::a1()
{
}
