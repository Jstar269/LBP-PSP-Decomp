/* LBPMATCH module=EBOOT.BIN start=0x08893228 end=0x08893320 symbol=FUN_08893228 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void *DAT_08bb1028[];
extern "C" void *DAT_08bb1038[];

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_08a9e0cc(void *);
extern "C" void FUN_0894a260(void *);

struct Ref {
    void (**vt)(Ref *);
    int rc;
};

struct Slot {
    Ref *p;
    inline ~Slot()
    {
        Ref *r = p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
    }
    inline void operator delete[](void *ptr) { FUN_08a9e0c4(ptr); }
};

struct Sub {
    int f0;
    int count;
    Slot *arr;
    inline ~Sub()
    {
        delete[] arr;
    }
};

struct Obj {
    void **vt;
    int f4;
    Sub sub;
};

extern "C" void FUN_08893228(Obj *self)
{
    self->vt = DAT_08bb1028;
    self->sub.~Sub();
    self->vt = DAT_08bb1038;
    FUN_0894a260(self);
    FUN_08a9e0cc(self);
}
