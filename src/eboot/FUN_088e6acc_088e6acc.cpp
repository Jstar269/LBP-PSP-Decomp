/* LBPMATCH module=EBOOT.BIN start=0x088e6acc end=0x088e6b68 symbol=FUN_088e6acc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(int *);
    int f4;
    int ref;
    int pad[2];
    int lock;
};

struct Outer {
    unsigned char pad[0x90];
    Obj *obj;
};

extern "C" void FUN_08ad3088(int *);
extern "C" void FUN_08ad2810(int *);
extern "C" void FUN_088050b8(Obj *);

extern "C" void FUN_088e6acc(Outer **param_1, int *param_2)
{
    if (*param_2 != 0) {
        Obj *o = (*param_1)->obj;
        if (o != 0) {
            FUN_08ad3088(&o->lock);
            o->ref++;
            FUN_08ad2810(&o->lock);
        }
        o->v5(param_2);
        *param_2 = 0;
        FUN_088050b8(o);
    }
}
