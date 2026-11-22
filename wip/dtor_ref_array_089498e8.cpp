/* LBPMATCH module=EBOOT.BIN start=0x089498e8 end=0x089499a8 symbol=FUN_089498e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_0894a260(void *);
extern "C" char DAT_08bb76f0, DAT_08bb7730;
struct Ref {
    void *p;
    ~Ref() { if (p) FUN_088050b8(p); }
};
struct Base {
    void *vt;
    char pad[44];
    ~Base() { vt = &DAT_08bb7730; FUN_0894a260(this); }
};
struct O : Base {
    Ref arr[3];
    int f60, f64;
    void *p68, *p72;
    ~O()
    {
        vt = &DAT_08bb76f0;
        if (p72) {
            FUN_08a9e0c4(p72);
            p72 = 0;
            f64 = 0;
        }
        if (p68) {
            FUN_08a9e0c4(p68);
            p68 = 0;
            f60 = 0;
        }
    }
};
extern "C" void FUN_089498e8(O *o) { o->~O(); }
// NEAR 16 (48 of 48 words, all of them the same instructions): the member-array destructor loop is reproduced exactly by a REAL C++ member array of a class with an inline destructor (struct Ref { void *p; ~Ref() { if (p) FUN_088050b8(p); } }; Ref arr[3]; the derived dtor body, then the member loop, then the base dtor inline: vt = &DAT_08bb7730; FUN_0894a260(this)). Only the guard layout differs: retail 'bnez v0,LOOP; li s2,4; j EXIT' (then-block after the jump), ours 'beqz v0,EXIT; addiu s1; j loop-test; li s2,4'.
// near miss from lane api-lbp-a-20261121 (unreviewed)
