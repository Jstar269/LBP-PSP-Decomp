/* LBPMATCH module=EBOOT.BIN start=0x0886c1b8 end=0x0886c25c symbol=_ZThn12_N12Obj_0886c09cD0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane thunks-20261121, Claude Opus 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_0886c09c; exact match verified; names and types await human review. */
/* Reconstructed class layout: Obj_0886c09c (constructor 0x0886c09c) derives from Base_088794c4 (constructor
   0x08879430, base-object destructor 0x088794c4), itself derived from two polymorphic classes, so the
   second vptr sits at +12 and the vtable entries reached through it are thunks (addiu a0,a0,-12; j body;
   nop). The destructor is implicit: GCC emits D1 and D0 with the vtable, each preceded by its thunk in the
   section .gnu.linkonce.t.<D1|D0>, laid out at the thunk's retail address by tools/lbpmatch.py. Retail
   vtables: Obj 0x08bb0228 (12 primary slots, so the second vptr is _ZTV+64), Mid 0x08baec58; member at
   +200 of class Member_0886bf48 (constructor 0x0886bf48, vtable 0x08bb01f8, whose vptr store in the
   inlined destructor is dead), 16 Elem_0886b790 at +408. cc=gcc404xsg-Os: the assembler with -g, as for
   every LBP thunk (docs/TOOLCHAIN.md). */
struct Elem_0886b790 {
    int w[4];
    Elem_0886b790();
    ~Elem_0886b790();
};
struct Root_0885c9f8 {
    virtual ~Root_0885c9f8();
};
struct Mid_08849d04 : Root_0885c9f8 {
    virtual void m2();
};
struct Member_0886bf48 : Mid_08849d04 {
    Member_0886bf48();
    int state;
    char rest[200];
};
struct Primary_088794c4 {
    virtual ~Primary_088794c4();
    virtual bool s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    int a, b;
};
struct Second_088794c4 {
    virtual ~Second_088794c4();
    virtual void t2();
    virtual void t3();
    virtual bool t4();
    virtual void t5();
    int x;
    char c;
};
struct Base_088794c4 : Primary_088794c4, Second_088794c4 {
    Base_088794c4(void *, void (*)(), int);
    virtual ~Base_088794c4();
    virtual bool t4();
    virtual void s10();
    virtual void t3();
    char data[176];
};
struct Obj_0886c09c : Base_088794c4 {
    Obj_0886c09c(void *owner);
    virtual bool s2();
    virtual bool t4();
    virtual void t3();
    Member_0886bf48 member;
    Elem_0886b790 elems[16];
    int last;
};

/* The key function (the first virtual function Obj_0886c09c declares that is not inline): defining it
   emits the class's vtable in this file, and with it the implicit destructor's D1 and D0 bodies, each after
   its this-adjusting thunk, as in retail. Its body (retail 0x0886b3c4) is not part of the compared range. */
bool Obj_0886c09c::s2()
{
    return member.state != 6;
}
