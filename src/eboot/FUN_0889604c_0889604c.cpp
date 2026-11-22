/* LBPMATCH module=EBOOT.BIN start=0x0889604c end=0x08896124 symbol=FUN_0889604c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

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
extern "C" void FUN_08adbc24(Heap *, void *, int);

struct Ref {
    void (**vt)(Ref *);
    int rc;
};

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
    int key[8];
    Ref *ref;
};

extern "C" void FUN_0889604c(void *self, Node *x)
{
    while (x != 0) {
        FUN_0889604c(self, x->right);
        Node *y = x->left;
        Ref *r = x->ref;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[2](r);
            }
        }
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        FUN_08adbc24(h, x, 52);
        FUN_08ad2810(&h->lock);
        x = y;
    }
}
