/* LBPMATCH module=EBOOT.BIN start=0x0894da14 end=0x0894dab8 symbol=FUN_0894da14 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj { void *vt; int refs; };
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
struct Fields { int a, b, c, d, e, f, g; };
struct Handle {
    Obj *p;
    Fields f;
    void copy(const Handle &o)
    {
        Obj *q = o.p;
        p = q;
        if (q) {
            FUN_08ad3088(&DAT_08bef8f4);
            ++q->refs;
            FUN_08ad2810(&DAT_08bef8f4);
        }
        f = o.f;
    }
};
extern "C" void FUN_0894da14(Handle *dst, const Handle *src)
{
    dst->copy(*src);
}
