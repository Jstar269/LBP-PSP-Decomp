/* LBPMATCH module=EBOOT.BIN start=0x088b519c end=0x088b5264 symbol=FUN_088b519c lang=c++ */
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
extern "C" void *FUN_08a9e838(int);
extern "C" void FUN_088c42c4(void *);
extern "C" void FUN_088cc5e0(void *, void *);
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

extern "C" void FUN_088b519c(void)
{
    void *a = FUN_08a9e838(20);
    FUN_088c42c4(a);
    void *b = FUN_08a9e838(36);
    FUN_088cc5e0(b, a);
    SharedPtr sp(b);
    FUN_088b2aa4(&sp);
    FUN_08804f3c(&sp);
}
