/* LBPMATCH module=EBOOT.BIN start=0x088fa9e4 end=0x088faab4 symbol=FUN_088fa9e4 lang=c++ rodata=0x08bab5e8 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" void *DAT_08bb60e8;
extern "C" void *DAT_08bb5950;
extern "C" void FUN_0891048c(void *);

struct Sub {
    void *vtable;
    int f04;
    unsigned char f08;
    unsigned char pad09[0x53];
    unsigned char b[2];
    unsigned char pad5e[2];
    int c[3];
    int f6c;
    int f70;

    inline void init() {
        vtable = &DAT_08bb60e8;
        f04 = 0;
        f08 = 0;
        for (int i = 0; i < 2; i++) {
            b[i] = 0;
        }
        for (int i = 0; i < 3; i++) {
            c[i] = 0;
        }
        f6c = 0;
        f70 = 0;
        FUN_0891048c(this);
        vtable = &DAT_08bb5950;
    }
};

struct Obj {
    int f00;
    int arr[74];
    Sub sub;
    int f1a0;
    float f1a4;
    int f1a8;
    int f1ac;
    int f1b0;
};

extern "C" void FUN_088fa9e4(Obj *param_1)
{
    param_1->f00 = 0;
    param_1->sub.init();
    param_1->f1a0 = 0;
    param_1->f1a4 = 0.2f;
    param_1->f1a8 = 0;
    param_1->f1ac = 0;
    param_1->f1b0 = 0;
    for (int i = 0; i < 74; i++) {
        param_1->arr[i] = 0;
    }
}

// Near-miss note: 52/52 words, 6 differ: subobject array loops use addiu base+0 vs s0+92/96.
// near miss from lane oc-lbp-20261121 (unreviewed)
