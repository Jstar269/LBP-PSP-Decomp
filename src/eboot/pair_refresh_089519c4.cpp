/* LBPMATCH module=EBOOT.BIN start=0x089519c4 end=0x08951a88 symbol=FUN_089519c4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct It { unsigned *p; int x; };
extern "C" int FUN_08a9b8a4(It *, unsigned *);
extern "C" unsigned *FUN_08a9d790(It *);
extern "C" unsigned char DAT_08bdb804;
extern "C" void FUN_089519c4(unsigned **t)
{
    It d, c, b, a;
    unsigned *p2 = t[0];
    unsigned *p3 = t[1];
    if (DAT_08bdb804) {
        a.x = FUN_08a9b8a4(&a, p2);
        a.p = p2;
        unsigned *r1 = FUN_08a9d790(&a);
        int bx = FUN_08a9b8a4(&b, p2);
        t[0] = r1;
        b.p = p2;
        b.x = bx;
        c.x = FUN_08a9b8a4(&c, p3);
        c.p = p3;
        unsigned *r2 = FUN_08a9d790(&c);
        FUN_08a9b8a4(&d, p3);
        t[1] = r2;
    }
}
// matching idioms: the locals d,c,b,a are declared in reverse order (slot order); the constructor-like call FUN_08a9b8a4 returns the length in v0 (Ghidra shows a 64-bit result) and is stored to the second word;
// the temporary b is written after the t[0] store (`bx` local) and d is only passed to the call, as retail has no stores for it.
