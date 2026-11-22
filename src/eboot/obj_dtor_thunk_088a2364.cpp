/* LBPMATCH module=EBOOT.BIN start=0x088a2364 end=0x088a23e4 symbol=_ZThn12_N12Obj_088a2370D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_088a2370; exact match verified; names and types await human review. */
/* Obj_088a2370 derives from PA_08bb23b8 (vtable 0x08bb23b0, whose inline destructor clears the global DAT_08bee3e8 when the byte at +4 is set) and PB_08bb22e8 (vtable 0x08bb22e0); its destructor frees an array at +20 with delete[]; vtable 0x08bb2370 with 6 primary slots. */
extern int DAT_08bee3e8;
struct PA_08bb23b8 {
    virtual ~PA_08bb23b8() { if (flag) DAT_08bee3e8 = 0; }
    virtual void a1();
    virtual void a2();
    virtual void a3();
    virtual void a4();
    unsigned char flag;
    char data[7];
};
struct PB_08bb22e8 {
    virtual ~PB_08bb22e8() {}
    virtual void b1();
    char data[4];
};
struct Obj_088a2370 : PA_08bb23b8, PB_08bb22e8 {
    virtual void a1();
    char *buf;
    ~Obj_088a2370() { delete[] buf; }
};
void Obj_088a2370::a1()
{
}
