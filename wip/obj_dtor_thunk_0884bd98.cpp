/* LBPMATCH module=EBOOT.BIN start=0x0884bd98 end=0x0884be7c symbol=_ZThn12_N12Obj_0884bda4D1Ev lang=c++ cc=gcc404xsg-O2 */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the complete-object destructor (D1) of Obj_0884bda4; exact match verified; names and types await human review. */
/* Obj_0884bda4 derives from Base_088794c4 (base-object destructor 0x088794c4, vptrs at +0 and +12, 196 bytes) and holds an array of 64 ref-counted pointers at +196 released by the member-array destructor loop; vtable 0x08baec88 with 12 primary slots. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
struct Shared {
    virtual void v0();
    virtual void destroy();
    int refs;
};
inline void release(Shared *o)
{
    if (o) {
        FUN_08ad3088(&DAT_08bef8f4);
        int r = o->refs - 1;
        o->refs = r;
        FUN_08ad2810(&DAT_08bef8f4);
        if (r == 0)
            o->destroy();
    }
}
struct Ref {
    Shared *p;
    ~Ref() { release(p); }
};
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
    virtual void a10();
    char data[8];
};
struct PB_088794c4 {
    virtual ~PB_088794c4();
    virtual void b1();
    char data[180];
};
struct Base_088794c4 : PA_088794c4, PB_088794c4 {
    virtual ~Base_088794c4();
};
struct Obj_0884bda4 : Base_088794c4 {
    virtual void a1();
    Ref arr[64];
};
void Obj_0884bda4::a1()
{
}
// NEAR (gcc404xsg-O2: 41 words differ of 57; at -Os the size differs so the link fails). Member-array destructor loop over 64 ref-counted pointers: retail guard is `addiu v0,a0,200` and the
// loop starts at a0+452 (pre-decrement), while Ref arr[64] at +196 gives guard a0+196 and a j-entry into the loop with s3 = a0+448. The array/guard relation (guard = &arr[1]?) is not reproduced.
// near miss from lane api-thunk-20261121 (unreviewed)
