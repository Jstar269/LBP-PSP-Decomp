/* LBPMATCH module=EBOOT.BIN start=0x088caa74 end=0x088cab54 symbol=FUN_088caa74 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088c8398(void *, const void *);

struct Task {
    void (**vt)(Task *);
    int rc;
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
    unsigned char pad2[0x84 - 0x64];
    int list2[2];
};

extern "C" void FUN_088caa74(const TaskPtr *param_1, Obj *param_2)
{
    TaskPtr tp(*param_1);
    FUN_088c8398(param_2->list2, param_1);
    param_2->state += 1;
}
