/* LBPMATCH module=EBOOT.BIN start=0x088bf6cc end=0x088bf7a4 symbol=FUN_088bf6cc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb37a8[];

struct Heap {
    unsigned char pad[32];
    int lock;
};

inline Heap *&sys_heap()
{
    static Heap *heap;
    return heap;
}

extern "C" void FUN_0883c080(void *, void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *, void *, int);
extern "C" void FUN_088b2df4(void *);
extern "C" void FUN_08a9e0cc(void *);

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
    int key;
    void *val;
};

struct Tree {
    int pad;
    int color;
    Node *root;
    Node *left;
    Node *right;
    int count;
    inline ~Tree()
    {
        Node *x = root;
        while (x != 0) {
            FUN_0883c080(this, x->right);
            Node *y = x->left;
            if (x->val != 0) {
                FUN_088050b8(x->val);
            }
            Heap *h = sys_heap();
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, x, 24);
            FUN_08ad2810(&h->lock);
            x = y;
        }
    }
};

struct Obj {
    void **vt;
    int pad[4];
    void *f14;
    Tree tree;
};

extern "C" void FUN_088bf6cc(Obj *self)
{
    self->vt = DAT_08bb37a8;
    self->tree.~Tree();
    if (self->f14 != 0) {
        FUN_088050b8(self->f14);
    }
    FUN_088b2df4(self);
    FUN_08a9e0cc(self);
}
