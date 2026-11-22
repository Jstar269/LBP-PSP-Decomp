/* LBPMATCH module=EBOOT.BIN start=0x088ca71c end=0x088ca80c symbol=FUN_088ca71c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void *DAT_08bedb1c;
extern "C" char DAT_08bb3db4[];
extern "C" void *FUN_08a9e838(int);
extern "C" void FUN_08949724(void *);
extern "C" void FUN_08ba0868(void *, int, const char *);
extern "C" void FUN_0894967c(void *, int);
extern "C" int FUN_08948e48(void *, void *);
extern "C" void FUN_088c8498(void);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Task {
    void (**vt)(Task *);
    int rc;
    void (*cb)(void);
    void *arg;
    int f10;
    int f14;
    char buf[0x20];
};

struct TaskPtr {
    Task *p;
    inline TaskPtr(Task *x) : p(x) {}
    inline ~TaskPtr()
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

struct Obj {
    unsigned char pad[0x60];
    int state;
};

extern "C" void FUN_088ca71c(Obj *self)
{
    Task *t = (Task *)FUN_08a9e838(0x38);
    FUN_08949724(t);
    TaskPtr tp(t);
    FUN_08ba0868(tp.p->buf, 0x14, DAT_08bb3db4);
    FUN_0894967c(tp.p, 2);
    tp.p->f14 = 1;
    tp.p->cb = FUN_088c8498;
    tp.p->arg = self;
    if (FUN_08948e48(DAT_08bedb1c, &tp) != 0) {
        self->state = 2;
    }
}
