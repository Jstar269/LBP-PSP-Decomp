/* LBPMATCH module=EBOOT.BIN start=0x08a39a9c end=0x08a39b2c symbol=_ZThn152_N12Obj_08a39aa8D0Ev lang=c++ cc=gcc404xsg-O2 */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08a39aa8; exact match verified; names and types await human review. */
/* Obj_08a39aa8 derives from PA_08a6fbac (destructor 0x08a6fbac) and PB_08bc493c (vtable 0x08bc4934, inline destructor, second base at +152); its destructor fetches the handle at +228 (looked up from the sub-object at +216 when null) and unregisters this with the manager (FUN_08a50fbc() + 2940). */
extern "C" char *FUN_08a50fbc(void);
extern "C" void *FUN_08a52bb8(void *, void *);
extern "C" void FUN_08a8dc80(void *, void *);
struct PA_08a6fbac {
    virtual ~PA_08a6fbac();
    virtual void a1();
    char data[148];
};
struct PB_08bc493c {
    virtual ~PB_08bc493c() {}
    virtual void b1();
    char data[60];
};
struct Obj_08a39aa8 : PA_08a6fbac, PB_08bc493c {
    virtual void a1();
    char sub[12];
    void *handle;
    ~Obj_08a39aa8()
    {
        void *h = handle;
        if (!h) {
            h = FUN_08a52bb8(FUN_08a50fbc(), sub);
            handle = h;
        }
        if (h)
            FUN_08a8dc80(FUN_08a50fbc() + 2940, (PA_08a6fbac *)this);
    }
};
void Obj_08a39aa8::a1()
{
}
// NEAR (gcc404xsg-O2: 24 words differ of 36). Retail stores only the two vptrs of C at entry (S0=0x08bc4818, S152=0x08bc493c) and calls the out-of-line base D2 0x08a6fbac;
// ours also emits the second base inline destructor store (PB inline dtor) later, so the vptr stores/order differ. Retail has NO inline vptr store for the +152 base: the second base is polymorphic
// but its destructor leaves no store (not reproduced: tried PB with inline virtual dtor). The cold block (lookup 0x08a52bb8 when handle at +228 is null) is laid out as in retail at O2.
// near miss from lane api-thunk-20261121 (unreviewed)
