/* LBPMATCH module=EBOOT.BIN start=0x088c26b0 end=0x088c279c symbol=FUN_088c26b0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Heap {
    unsigned char pad[32];
    int lock;
};

inline Heap *&sys_heap()
{
    static Heap *heap;
    return heap;
}

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void *FUN_08adbb84(Heap *, int);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088c2444(void *);
extern "C" void FUN_088b2aa4(void *);
extern "C" void FUN_08804f3c(void *);

struct SharedPtr {
    void *p;
    int *rc;
    inline SharedPtr(void *ptr)
    {
        p = ptr;
        if (ptr != 0) {
            Heap *h = sys_heap();
            FUN_08ad3088(&h->lock);
            int *r = (int *)FUN_08adbb84(h, 4);
            FUN_08ad2810(&h->lock);
            rc = r;
            FUN_08ad3088(&DAT_08bef8f4);
            *r = 1;
            FUN_08ad2810(&DAT_08bef8f4);
        } else {
            rc = 0;
        }
    }
};

struct Obj {
    int pad[5];
    void *f14;
    unsigned char f18;
    unsigned char f19;
    unsigned char f1a;
    unsigned char f1b;
    unsigned int f1c;
};

extern "C" void FUN_088c26b0(Obj *self)
{
    FUN_088c2444(self);
    if (self->f18 != 0 && self->f19 != 0 && self->f1a != 0) {
        if (++self->f1c > 2) {
            SharedPtr sp(self->f14);
            FUN_088b2aa4(&sp);
            FUN_08804f3c(&sp);
            self->f14 = 0;
        }
    }
}
