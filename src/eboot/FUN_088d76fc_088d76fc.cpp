/* LBPMATCH module=EBOOT.BIN start=0x088d76fc end=0x088d77a4 symbol=FUN_088d76fc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Desc {
    int kind;
    int pad[5];
};

struct Mgr {
    unsigned char pad[0x148];
    Desc *descs;

    inline Desc *get_desc(int idx)
    {
        return &descs[idx & 0xffff];
    }
};

extern "C" void *FUN_08a9e838(unsigned int);
extern "C" void FUN_088e198c(void *, unsigned short, Desc *);
extern "C" void FUN_088dfd1c(void *, unsigned short, Desc *);

struct ObjA {
    int pad[10];
    inline void *operator new(unsigned int sz) { return FUN_08a9e838(sz); }
    inline ObjA(unsigned short id, Desc *d) { FUN_088e198c(this, id, d); }
};

struct ObjB {
    int pad[11];
    inline void *operator new(unsigned int sz) { return FUN_08a9e838(sz); }
    inline ObjB(unsigned short id, Desc *d) { FUN_088dfd1c(this, id, d); }
};

extern "C" void *FUN_088d76fc(Mgr *param_1, unsigned short param_2)
{
    Desc *d = param_1->get_desc(param_2);
    if (d->kind != 0) {
        if (d->kind == 1) {
            return new ObjB(param_2, d);
        }
        return 0;
    }
    return new ObjA(param_2, d);
}
