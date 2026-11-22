/* LBPMATCH module=EBOOT.BIN start=0x08942b00 end=0x08942b64 symbol=_ZThn8_N12Obj_08942b0cD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08942b0c; exact match verified; names and types await human review. */
/* Obj_08942b0c derives from PA (0x08bb7528, inline destructor over the root class whose destructor is 0x0894a260) and PB (destructor 0x0894a260, the second base at +8); vtable 0x08bb7410 with 64 primary slots. The destructor stores both vptrs, destroys PB, then PA, and returns. */
struct Root_0894a260 {
    virtual ~Root_0894a260();
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
    virtual void a18();
    virtual void a19();
    virtual void a20();
    virtual void a21();
    virtual void a22();
    virtual void a23();
    virtual void a24();
    virtual void a25();
    virtual void a26();
    virtual void a27();
    virtual void a28();
    virtual void a29();
    virtual void a30();
    virtual void a31();
    virtual void a32();
    virtual void a33();
    virtual void a34();
    virtual void a35();
    virtual void a36();
    virtual void a37();
    virtual void a38();
    virtual void a39();
    virtual void a40();
    virtual void a41();
    virtual void a42();
    virtual void a43();
    virtual void a44();
    virtual void a45();
    virtual void a46();
    virtual void a47();
    virtual void a48();
    virtual void a49();
    virtual void a50();
    virtual void a51();
    virtual void a52();
    virtual void a53();
    virtual void a54();
    virtual void a55();
    virtual void a56();
    virtual void a57();
    virtual void a58();
    virtual void a59();
    virtual void a60();
    virtual void a61();
    virtual void a62();
    char data[4];
};
struct PA_08bb7530 : Root_0894a260 {
    virtual ~PA_08bb7530() {}
    virtual void a1();
};
struct PB_0894a260 {
    virtual ~PB_0894a260();
    char data[4];
};
struct Obj_08942b0c : PA_08bb7530, PB_0894a260 {
    virtual void a1();
};
void Obj_08942b0c::a1()
{
}
