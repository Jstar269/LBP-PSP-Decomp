/* LBPMATCH module=EBOOT.BIN start=0x0889d240 end=0x0889d2f4 symbol=FUN_0889d240 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bedb1c;
extern "C" int DAT_08bef8f4;

extern "C" void *FUN_08a9e838(int);
extern "C" void FUN_08949d20(void *, int);
extern "C" void FUN_0889fad8(void);
extern "C" void FUN_089487a8(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Task {
    void (**vt)(Task *);
    int rc;
    void (*fn)(void);
    int f0c;
    int f10;
    int f14;
    unsigned char pad[96 - 24];
    int f60;
};

struct SharedPtr {
    Task *p;
    inline SharedPtr(Task *x) : p(x) {}
    inline ~SharedPtr()
    {
        Task *r = p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
    }
};

extern "C" void FUN_0889d240(void)
{
    Task *t = (Task *)FUN_08a9e838(112);
    FUN_08949d20(t, 1);
    t->fn = FUN_0889fad8;
    t->f0c = t->f60;
    t->f14 = 0;
    SharedPtr sp(t);
    FUN_089487a8(DAT_08bedb1c, &sp);
}
