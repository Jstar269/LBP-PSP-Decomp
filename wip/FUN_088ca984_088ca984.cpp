/* LBPMATCH module=EBOOT.BIN start=0x088ca984 end=0x088caa74 symbol=FUN_088ca984 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088c8398(void *, const void *);

struct Task {
    void (**vt)(Task *);
    int rc;
    int f8;
    int f0c;
    int f10;
};

struct TaskPtr {
    Task *p;
    inline TaskPtr(const TaskPtr &o)
    {
        Task *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            ++r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
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
    unsigned char pad1[0x60];
    int state;
    unsigned char pad2[0x7c - 0x64];
    int list1[2];
    int list2[2];
};

extern "C" void FUN_088ca984(const TaskPtr *param_1, Obj *param_2)
{
    TaskPtr tp(*param_1);
    if (tp.p->f10 == 0) {
        FUN_088c8398(param_2->list2, param_1);
        param_2->state += 1;
    } else {
        FUN_088c8398(param_2->list1, param_1);
        param_2->state += 2;
    }
}

// Remaining diffs (59/60 words; 39 differ): GCC deletes `beqz s1` in ~TaskPtr() after dereferencing tp.p->f10, whereas retail keeps the null check.
// near miss from lane oc-lbp-20261121 (unreviewed)
