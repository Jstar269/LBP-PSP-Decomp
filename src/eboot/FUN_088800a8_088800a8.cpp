/* LBPMATCH module=EBOOT.BIN start=0x088800a8 end=0x0888018c symbol=FUN_088800a8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

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

struct Item {
    int a[7];
};

struct Node {
    Node *next;
    Node *prev;
    Item item;
};

static inline void push_back(Node *dst, const Item &x)
{
    Heap *h = sys_heap();
    FUN_08ad3088(&h->lock);
    Node *n = (Node *)FUN_08adbb84(h, 36);
    FUN_08ad2810(&h->lock);
    new (&n->item) Item(x);
    FUN_08b88fac(n, dst);
}

extern "C" void FUN_088800a8(Node *dst, Node *src)
{
    dst->next = dst;
    dst->prev = dst;
    for (Node *cur = src->next; cur != src; cur = cur->next) {
        push_back(dst, cur->item);
    }
}
