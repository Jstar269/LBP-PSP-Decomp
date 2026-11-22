/* LBPMATCH module=EBOOT.BIN start=0x08908ef0 end=0x08908fe0 symbol=FUN_08908ef0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Callback {
    virtual void v0();
    virtual void v1();
    virtual void on_done(bool);
};

struct Req {
    unsigned char pad[0x34];
    int f34;
};

struct Obj {
    int f00;
    Callback *f04;
    int pad0[2];
    int f10[6];
    int f28[6];
    int f40[7];
    int f5c[7];
    int f78;
};

extern "C" void FUN_08921858(void *);
extern "C" void FUN_08a9c6b0(void *, int);

extern "C" void FUN_08908ef0(Req **param_1, Obj *param_2)
{
    bool failed = (*param_1)->f34 != 0;
    if (!failed) {
        FUN_08921858(param_2->f40);
        FUN_08921858(param_2->f5c);
        FUN_08a9c6b0(param_2->f10, 1);
        FUN_08a9c6b0(param_2->f28, 1);
        param_2->f78 = 0;
        param_2->f00 = 0;
        if (param_2->f04 != 0) {
            param_2->f04->on_done(false);
        }
    } else {
        FUN_08921858(param_2->f40);
        FUN_08921858(param_2->f5c);
        FUN_08a9c6b0(param_2->f10, 1);
        FUN_08a9c6b0(param_2->f28, 1);
        param_2->f78 = 0;
        param_2->f00 = 0;
        if (param_2->f04 != 0) {
            param_2->f04->on_done(true);
        }
    }
}
