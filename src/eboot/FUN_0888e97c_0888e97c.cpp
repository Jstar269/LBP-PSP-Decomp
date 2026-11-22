/* LBPMATCH module=EBOOT.BIN start=0x0888e97c end=0x0888ea34 symbol=FUN_0888e97c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" int DAT_08bef8f4;

struct Heap {
    unsigned char pad[32];
    int lock;
};

inline Heap *&sys_heap()
{
    static Heap *heap;
    return heap;
}

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adbb84(Heap *, int);
extern "C" void FUN_08b88fac(void *, void *);

struct Ref {
    void *vt;
    int rc;
};

struct SharedPtr {
    Ref *p;
    inline SharedPtr(const SharedPtr &o)
    {
        Ref *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            r->rc++;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct Item {
    int f0;
    SharedPtr ref;
};

struct Node {
    Node *next;
    Node *prev;
    Item item;
};

extern "C" void FUN_0888e97c(void *self, Node *pos, const Item *x)
{
    Heap *h = sys_heap();
    FUN_08ad3088(&h->lock);
    Node *n = (Node *)FUN_08adbb84(h, 16);
    FUN_08ad2810(&h->lock);
    new (&n->item) Item(*x);
    FUN_08b88fac(n, pos);
}
