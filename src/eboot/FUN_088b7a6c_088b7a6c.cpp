/* LBPMATCH module=EBOOT.BIN start=0x088b7a6c end=0x088b7b18 symbol=FUN_088b7a6c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Item {
    int f0;
    int f4;
    int rc;
    int f0c;
    int f10;
    int lock;
};

struct RefPtr {
    Item *p;
    inline void init(const RefPtr &o)
    {
        Item *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&r->lock);
            ++r->rc;
            FUN_08ad2810(&r->lock);
        }
    }
};

struct Task {
    int f0;
    int rc;
};

struct TaskPtr {
    Task *p;
    inline void init(const TaskPtr &o)
    {
        Task *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            ++r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct Val {
    RefPtr a;
    TaskPtr b;
    int c;
};

extern "C" void FUN_088b7a6c(Val *dst, const Val *src)
{
    dst->a.init(src->a);
    dst->b.init(src->b);
    dst->c = src->c;
}
