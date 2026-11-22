/* LBPMATCH module=EBOOT.BIN start=0x08891e3c end=0x08891edc symbol=FUN_08891e3c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb1028[];

extern "C" void FUN_0894a258(void *);
extern "C" void *FUN_08a9e824(unsigned int);

struct Slot {
    int p;
    inline Slot() : p(0) {}
    inline ~Slot() {}
    inline void *operator new[](unsigned int sz) { return FUN_08a9e824(sz); }
};

struct Sub {
    int f0;
    int count;
    Slot *arr;
    int f0c;
    int f10;
    unsigned char b14;
    unsigned char b15;
    inline void init(int n)
    {
        count = n;
        arr = new Slot[n];
        b14 = 1;
        f10 = 0;
        f0c = 0;
        b15 = 0;
    }
};

struct Obj {
    void **vt;
    int f4;
    Sub sub;
};

extern "C" void FUN_08891e3c(Obj *self, int param_2)
{
    FUN_0894a258(self);
    self->f4 = 1;
    self->vt = DAT_08bb1028;
    self->sub.init(param_2);
}
