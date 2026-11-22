/* LBPMATCH module=EBOOT.BIN start=0x0886b948 end=0x0886ba38 symbol=FUN_0886b948 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

struct Mutex { int a; };
extern Mutex DAT_08bef8f4;

struct Heap {
    unsigned char pad[32];
    int lock;
};
extern Heap *DAT_08be84ac;

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adbb84(Heap *, int);
extern "C" void FUN_08b88fac(void *, void *);

struct Ref {
    void *vt;
    int count;
};

struct Item {
    int f0;
    int f4;
    Ref *r;
    Item(const Item &o) {
        f0 = o.f0;
        f4 = o.f4;
        Ref *rr = o.r;
        r = rr;
        if (rr != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            rr->count++;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct Node {
    Node *next;
    Node *prev;
    Item item;
};

inline void push_back(Node *dst, const Item &x)
{
    Heap *h = DAT_08be84ac;
    FUN_08ad3088(&h->lock);
    Node *n = (Node *)FUN_08adbb84(h, 20);
    FUN_08ad2810(&h->lock);
    new (&n->item) Item(x);
    FUN_08b88fac(n, dst);
}

extern "C" void FUN_0886b948(Node *dst, Node *src)
{
    Node *cur;
    dst->next = dst;
    dst->prev = dst;
    for (cur = src->next; cur != src; cur = cur->next) {
        push_back(dst, cur->item);
    }
}
