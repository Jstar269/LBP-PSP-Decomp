/* LBPMATCH module=EBOOT.BIN start=0x08909418 end=0x089094bc symbol=FUN_08909418 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb5e28;
extern "C" void *DAT_08bb5e58;
extern "C" void *DAT_08bb5e40;
extern "C" void FUN_0899e590(void *);
extern "C" void FUN_08a9c014(void *);
extern "C" void FUN_08908684(void);
extern "C" void FUN_08908a98(void);
extern "C" void FUN_089085d8(void *, void *, void *);
extern "C" void FUN_0898e3b0(void *, void *);

struct Delegate {
    void *vtbl;
    void *obj;
    void (*fn)(void);
    int f0c;

    inline void init(void *v, void *o, void (*f)(void)) {
        vtbl = v;
        obj = o;
        fn = f;
        f0c = 0;
    }
};

struct Obj {
    void *vtbl;
    int pad0[3];
    int str10[6];
    Delegate del1;
    unsigned char sub38[0xb8 - 0x38];
    Delegate del2;
    int subc8;
};

extern "C" void FUN_08909418(Obj *param_1, void *param_2)
{
    FUN_0899e590(param_1);
    param_1->vtbl = &DAT_08bb5e28;
    FUN_08a9c014(param_1->str10);
    param_1->del1.init(&DAT_08bb5e58, param_1, FUN_08908684);
    FUN_089085d8(param_1->sub38, param_2, &param_1->del1);
    param_1->del2.init(&DAT_08bb5e40, param_1, FUN_08908a98);
    FUN_0898e3b0(&param_1->subc8, &param_1->del2);
}
