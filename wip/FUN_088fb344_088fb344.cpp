/* LBPMATCH module=EBOOT.BIN start=0x088fb344 end=0x088fb3e8 symbol=FUN_088fb344 lang=c++ rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Widget {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void slot6(float);
    virtual float slot7();
    int f04;
    int f08;
    void *f0c;
};

struct Global {
    unsigned char pad0[0x2ac];
    float f2ac;
    unsigned char pad1[0x7c8 - 0x2b0];
    void *arr[1];
};

struct Obj {
    unsigned char pad0[0x3c];
    Widget *f3c;
    unsigned char pad1[0x71 - 0x40];
    unsigned char f71;
    unsigned char pad2[2];
    float f74;
    unsigned char pad3[0x84 - 0x78];
    Widget *f84;
};

extern "C" Global *DAT_08bf7304;
extern "C" void FUN_08a50fbc(Obj *);

extern "C" void FUN_088fb344(Obj *param_1)
{
    FUN_08a50fbc(param_1);
    Global *g = DAT_08bf7304;
    Widget *w = param_1->f3c;
    float val = g->f2ac;
    param_1->f84 = w;
    int idx = (int)val;
    w->f0c = g->arr[idx > 0 ? idx : 0];
    w->slot6(1.0f - w->slot7());
    param_1->f71 = 1;
    param_1->f74 = val;
}

// Near-miss note: 41/41 words: virtual slot7/slot6 load scheduling and f71/f74 store order differ.
// near miss from lane oc-lbp-20261121 (unreviewed)
