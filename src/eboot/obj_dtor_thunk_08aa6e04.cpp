/* LBPMATCH module=EBOOT.BIN start=0x08aa6e04 end=0x08aa6e60 symbol=_ZThn12_N12Obj_08aa6e10D1Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane thunks-20261121, Claude Opus 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_08aa6e10; exact match verified; names and types await human review. */
/* Obj_08aa6e10 derives from Base_08aa6d84 (base-object destructor 0x08aa6d84, which resets both vptrs), itself
   derived from two polymorphic classes: the second vptr sits at +12, and the vtable (0x08bcb2c0, null typeinfo,
   17 primary slots, so the second vptr is _ZTV+84) reaches the destructors through thunks. The inline
   destructor frees the array at +32 with delete[]. GCC emits D1 and D0 each after its thunk in
   .gnu.linkonce.t.<D1|D0>; tools/lbpmatch.py lays the section out at the thunk's retail address.
   cc=gcc404xsg-Os: the assembler with -g, as for every LBP thunk (docs/TOOLCHAIN.md). */
struct Primary_08aa6d84 {
    virtual ~Primary_08aa6d84();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    int a, b;
};
struct Second_08aa6d84 {
    virtual ~Second_08aa6d84();
    virtual void w2();
    int x, y, z;
};
struct Base_08aa6d84 : Primary_08aa6d84, Second_08aa6d84 {
    virtual ~Base_08aa6d84();
    int own;
};
struct Obj_08aa6e10 : Base_08aa6d84 {
    virtual void v2();
    ~Obj_08aa6e10() { delete[] buf; }
    char *buf;
};

/* The key function (the first virtual function Obj_08aa6e10 declares that is not inline, retail 0x08aa4418):
   defining it emits the vtable here, and with it the inline destructor's D1 and D0 after their thunks. Its body
   is not reconstructed (left empty) and is not part of the compared range. */
void Obj_08aa6e10::v2()
{
}
