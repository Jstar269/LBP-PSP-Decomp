/* LBPMATCH module=EBOOT.BIN start=0x088c7ff4 end=0x088c80d0 symbol=FUN_088c7ff4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Task {
    void (**vt)(Task *);
    int rc;
    unsigned char pad1[0x40 - 8];
    int f40;
    unsigned char pad2[0x70 - 0x44];
    unsigned char *f70;
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
                r->vt[2](r);
            }
        }
    }
};

struct Obj {
    unsigned char pad1[0x60];
    int state;
    unsigned char pad2[0xd4 - 0x64];
    unsigned char flag;
};

extern "C" void FUN_088c7ff4(const TaskPtr *param_1, Obj *param_2)
{
    TaskPtr tp(*param_1);
    if (tp.p->f40 == 0) {
        if (*tp.p->f70 == 0) {
            param_2->flag = 0;
        } else {
            param_2->flag = 1;
        }
    } else {
        param_2->flag = 0;
    }
    param_2->state++;
}

// Remaining diffs (53/55 words): GCC deletes the redundant `beqz s1` null check in ~TaskPtr() after dereferencing tp.p, whereas retail retains it.
// near miss from lane oc-lbp-20261121 (unreviewed)
