/* LBPMATCH module=EBOOT.BIN start=0x088b3974 end=0x088b3a4c symbol=FUN_088b3974 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088b38a0(void *);
extern "C" void FUN_088b3728(void *, void *);
extern "C" void FUN_08804f34(void) __attribute__((weak));

struct Item {
    unsigned char pad[17];
    unsigned char f11;
};

struct SharedPtr {
    Item *p;
    int *rc;
};

struct Task {
    void (**vt)(Task *);
    int rc;
    int pad[3];
    int f14;
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
    int f0;
    int f4;
    int pad[3];
    int lock;
};

extern "C" bool FUN_088b3974(Obj *self, SharedPtr *arg)
{
    bool res = false;
    Item *it = arg->p;
    if (it != 0 && FUN_08804f34 != 0 && it->f11 == 0) {
        it->f11 = 1;
        TaskPtr tp;
        FUN_088b38a0(&tp);
        tp.p->f14 = 2;
        FUN_08ad3088(&self->lock);
        FUN_088b3728(&self->f4, &tp);
        FUN_08ad2810(&self->lock);
        res = true;
    }
    return res;
}
