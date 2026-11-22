/* LBPMATCH module=EBOOT.BIN start=0x08946350 end=0x089463d4 symbol=_ZThn104_N12Obj_0894635cD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_0894635c; exact match verified; names and types await human review. */
/* Obj_0894635c derives from PA_08bb7270 (vtable 0x08bb7268, over the root class 0x08944cac, with a member at +72 whose inline destructor calls FUN_08a9c6b0(this, 1) then FUN_08a9bff8(this)) and PB_0894a260 (destructor 0x0894a260, second base at +104); vtable 0x08bb7370 with 126 primary slots. */
extern "C" void FUN_08a9c6b0(void *, int);
extern "C" void FUN_08a9bff8(void *);
struct Q_08a9c6b0 {
    char data[32];
    ~Q_08a9c6b0()
    {
        FUN_08a9c6b0(this, 1);
        FUN_08a9bff8(this);
    }
};
struct Root_08944cac {
    virtual ~Root_08944cac();
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
    virtual void a63();
    virtual void a64();
    virtual void a65();
    virtual void a66();
    virtual void a67();
    virtual void a68();
    virtual void a69();
    virtual void a70();
    virtual void a71();
    virtual void a72();
    virtual void a73();
    virtual void a74();
    virtual void a75();
    virtual void a76();
    virtual void a77();
    virtual void a78();
    virtual void a79();
    virtual void a80();
    virtual void a81();
    virtual void a82();
    virtual void a83();
    virtual void a84();
    virtual void a85();
    virtual void a86();
    virtual void a87();
    virtual void a88();
    virtual void a89();
    virtual void a90();
    virtual void a91();
    virtual void a92();
    virtual void a93();
    virtual void a94();
    virtual void a95();
    virtual void a96();
    virtual void a97();
    virtual void a98();
    virtual void a99();
    virtual void a100();
    virtual void a101();
    virtual void a102();
    virtual void a103();
    virtual void a104();
    virtual void a105();
    virtual void a106();
    virtual void a107();
    virtual void a108();
    virtual void a109();
    virtual void a110();
    virtual void a111();
    virtual void a112();
    virtual void a113();
    virtual void a114();
    virtual void a115();
    virtual void a116();
    virtual void a117();
    virtual void a118();
    virtual void a119();
    virtual void a120();
    virtual void a121();
    virtual void a122();
    virtual void a123();
    virtual void a124();
    char data[68];
};
struct PA_08bb7270 : Root_08944cac {
    virtual ~PA_08bb7270() {}
    virtual void a1();
    Q_08a9c6b0 member;
};
struct PB_0894a260 {
    virtual ~PB_0894a260();
    char data[4];
};
struct Obj_0894635c : PA_08bb7270, PB_0894a260 {
    virtual void a1();
};
void Obj_0894635c::a1()
{
}
