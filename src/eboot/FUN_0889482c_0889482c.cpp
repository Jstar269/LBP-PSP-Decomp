/* LBPMATCH module=EBOOT.BIN start=0x0889482c end=0x08894908 symbol=FUN_0889482c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb1060[];

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
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_08a9e0cc(void *);
extern "C" void FUN_08894614(void *, void *);
extern "C" void FUN_0889333c(void *, void *);

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
};

struct Tree1 {
    int pad;
    int color;
    Node *root;
    Node *left;
    Node *right;
    int count;
    inline ~Tree1()
    {
        Node *x = root;
        while (x != 0) {
            FUN_08894614(this, x->right);
            Node *y = x->left;
            Heap *h = sys_heap();
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, x, 56);
            FUN_08ad2810(&h->lock);
            x = y;
        }
    }
};

struct Tree2 {
    int pad;
    int color;
    Node *root;
    Node *left;
    Node *right;
    int count;
    inline ~Tree2()
    {
        FUN_0889333c(this, root);
    }
};

struct Sub {
    int pad[4];
    Tree1 tree;
    void *f28;
    inline ~Sub()
    {
        if (f28 != 0) {
            FUN_08a9e0c4(f28);
        }
    }
};

struct Obj {
    void **vt;
    int f4;
    int f8;
    Sub *sub;
    Tree2 tree;
};

extern "C" void FUN_0889482c(Obj *self)
{
    Sub *s = self->sub;
    self->vt = DAT_08bb1060;
    if (s != 0) {
        s->~Sub();
        FUN_08a9e0cc(s);
    }
    self->tree.~Tree2();
    FUN_08a9e0cc(self);
}
