/* LBPMATCH module=EBOOT.BIN start=0x088bbf6c end=0x088bc040 symbol=FUN_088bbf6c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0883a3c8(void *);
extern "C" void FUN_08921bec(void *);
extern "C" void FUN_08922ce0(void *, int, int, int);
extern "C" void FUN_08922a00(void *, int);
extern "C" void FUN_088bb960(void);
extern "C" void FUN_088bb9d0(void);

struct Task {
    void (**vt)(Task *);
    int rc;
    int pad1[0x90];
    int f248;
    int pad2[0x21];
    void (*cb1)(void);
    void (*cb2)(void);
    void *arg1;
    void *arg2;
};

struct TaskPtr {
    Task *p;
    inline ~TaskPtr()
    {
        Task *r = p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[2](r);
            }
        }
    }
};

struct Obj {
    unsigned char pad[0x58];
    unsigned char f58;
};

extern "C" void FUN_088bbf6c(Obj *self)
{
    self->f58 = 0;
    TaskPtr tp;
    FUN_0883a3c8(&tp);
    FUN_08921bec(&tp);
    FUN_08922ce0(&tp, 0x23c, 0, 0);
    tp.p->f248 = 400;
    tp.p->cb1 = FUN_088bb960;
    tp.p->cb2 = FUN_088bb9d0;
    tp.p->arg2 = self;
    tp.p->arg1 = self;
    FUN_08922a00(&tp, 0);
}

// Remaining diffs (53/53 words; 2 differ at +0x68): GCC schedules sw s0,728(v1) before sw s0,732(v1) regardless of source assignment order.
// near miss from lane oc-lbp-20261121 (unreviewed)
