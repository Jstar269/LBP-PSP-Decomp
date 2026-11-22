/* LBPMATCH module=EBOOT.BIN start=0x088c68f8 end=0x088c69bc symbol=FUN_088c68f8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0883a3c8(void *);
extern "C" void FUN_08921bec(void *);
extern "C" void FUN_08922a00(void *, int);
extern "C" void FUN_088c6000(void);
extern "C" void FUN_088c601c(void);

struct Task {
    void (**vt)(Task *);
    int rc;
    int pad0[14];
    int f40;
    int f44;
    int pad1[0x92 - 0x12];
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

extern "C" void FUN_088c68f8(void *param_1, int param_2)
{
    TaskPtr tp;
    FUN_0883a3c8(&tp);
    FUN_08921bec(&tp);
    tp.p->cb1 = FUN_088c6000;
    tp.p->arg2 = param_1;
    tp.p->arg1 = param_1;
    tp.p->f44 = param_2;
    tp.p->cb2 = FUN_088c601c;
    tp.p->f40 = 0;
    tp.p->f248 = 0;
    FUN_08922a00(&tp, 0);
}

// Remaining diffs (49/49 words; 5 differ at +0x48): store scheduling order among arg1/arg2 and f40/f248 before jal 0x8922a00.
// near miss from lane oc-lbp-20261121 (unreviewed)
