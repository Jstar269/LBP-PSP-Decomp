/* LBPMATCH module=EBOOT.BIN start=0x088b2010 end=0x088b20f8 symbol=FUN_088b2010 lang=c++ */
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

struct Obj1 {
    int (**vt)(Obj1 *);
};

struct Obj2 {
    void (**vt)(Obj2 *, int);
};

struct SharedPtr {
    Obj2 *p;
    int *rc;
    inline SharedPtr(Obj2 *ptr)
    {
        p = ptr;
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        int *r = (int *)FUN_08adbb84(h, 4);
        FUN_08ad2810(&h->lock);
        rc = r;
        FUN_08ad3088(&DAT_08bef8f4);
        *r = 1;
        FUN_08ad2810(&DAT_08bef8f4);
    }
};

extern "C" void *FUN_08a9e838(int);
extern "C" void FUN_088b58b8(Obj1 *);
extern "C" void FUN_088c2380(Obj2 *, Obj1 *, int);
extern "C" void FUN_08973240(Obj2 *);
extern "C" void FUN_088b2aa4(void *);
extern "C" void FUN_08804f3c(void *);

extern "C" void FUN_088b2010(void)
{
    Obj1 *a = (Obj1 *)FUN_08a9e838(92);
    FUN_088b58b8(a);
    Obj2 *b = (Obj2 *)FUN_08a9e838(148);
    FUN_088c2380(b, a, 9);
    b->vt[19](b, a->vt[16](a));
    FUN_08973240(b);
    SharedPtr sp(b);
    FUN_088b2aa4(&sp);
    FUN_08804f3c(&sp);
}

// Remaining diffs (58/58 words; 2 differ at +0xb4): retail puts `sw v0,0(s0)` in the delay slot of `jal 0x8ad2810` instead of `addiu a0,s2,-1804` (tried RAII Lock guard, same 2-word swap).
// near miss from lane oc-lbp-20261121 (unreviewed)
