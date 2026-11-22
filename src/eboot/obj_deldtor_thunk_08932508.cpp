/* LBPMATCH module=EBOOT.BIN start=0x08932508 end=0x089325ac symbol=_ZThn8_N12Obj_08932514D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08932514; exact match verified; names and types await human review. */
/* Obj_08932514 derives from PA_08bb6d40 (vtable 0x08bb6d38) and PB_08bb6d30 (vtable 0x08bb6d28), both with inline destructors over the root class 0x0894a260; members: Mem_08a9bff8 at +16 (destructor 0x08a9bff8) and a pointer at +40 released with FUN_088050b8; its destructor body releases the pointer and clears it; vtable 0x08bb6cb0 with 3 primary slots. */
extern "C" void FUN_088050b8(void *);
struct Ptr {
    void *p;
    ~Ptr()
    {
        if (p)
            FUN_088050b8(p);
    }
};
struct RootA_0894a260 {
    virtual ~RootA_0894a260();
    virtual void a1();
    char data[4];
};
struct PA_08bb6d40 : RootA_0894a260 {
    virtual ~PA_08bb6d40() {}
    virtual void a1();
};
struct RootB_0894a260 {
    virtual ~RootB_0894a260();
};
struct PB_08bb6d30 : RootB_0894a260 {
    virtual ~PB_08bb6d30() {}
    virtual void b1();
    char data[4];
};
struct Mem_08a9bff8 {
    char data[24];
    ~Mem_08a9bff8();
};
struct Obj_08932514 : PA_08bb6d40, PB_08bb6d30 {
    virtual void a1();
    Mem_08a9bff8 m;
    Ptr q;
    ~Obj_08932514()
    {
        void *x = q.p;
        q.p = 0;
        if (x)
            FUN_088050b8(x);
    }
};
void Obj_08932514::a1()
{
}
