/* LBPMATCH module=EBOOT.BIN start=0x08894c1c end=0x08894cbc symbol=FUN_08894c1c lang=c++ */
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

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *, void *, int);
extern "C" void FUN_08894614(void *, void *);

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
};

struct Tree {
    int pad;
    Node header;
    int count;
};

extern "C" void FUN_08894c1c(Tree *self)
{
    Node *x = self->header.parent;
    while (x != 0) {
        FUN_08894614(self, x->right);
        Node *y = x->left;
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        FUN_08adbc24(h, x, 56);
        FUN_08ad2810(&h->lock);
        x = y;
    }
    self->count = 0;
    self->header.left = &self->header;
    self->header.parent = 0;
    self->header.right = &self->header;
}
