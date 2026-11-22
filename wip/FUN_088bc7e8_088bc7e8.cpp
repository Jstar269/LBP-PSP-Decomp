/* LBPMATCH module=EBOOT.BIN start=0x088bc7e8 end=0x088bc8d4 symbol=FUN_088bc7e8 lang=c++ */
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

extern "C" void *DAT_08bedb1c;
extern "C" int DAT_08bef8f4;
extern "C" void FUN_089463d4(void *);
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
extern "C" void FUN_088bf55c(Obj1 *);
extern "C" void FUN_088c2380(Obj2 *, Obj1 *, int);
extern "C" void FUN_088b2aa4(void *);
extern "C" void FUN_08804f3c(void *);

extern "C" void FUN_088bc7e8(void)
{
    FUN_089463d4(DAT_08bedb1c);
    Obj1 *a = (Obj1 *)FUN_08a9e838(52);
    FUN_088bf55c(a);
    Obj2 *b = (Obj2 *)FUN_08a9e838(148);
    FUN_088c2380(b, a, 9);
    b->vt[19](b, a->vt[16](a));
    SharedPtr sp(b);
    FUN_088b2aa4(&sp);
    FUN_08804f3c(&sp);
}

// Remaining diffs (59/59 words; 2 differ at +0xb8): same 2-word delay-slot swap as FUN_088b2010 (retail puts `sw v0,0(s0)` in `jal 0x8ad2810` delay slot instead of `addiu a0,s2,-1804`).
// near miss from lane oc-lbp-20261121 (unreviewed)
