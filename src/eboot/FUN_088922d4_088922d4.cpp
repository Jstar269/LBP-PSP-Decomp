/* LBPMATCH module=EBOOT.BIN start=0x088922d4 end=0x088923b8 symbol=FUN_088922d4 lang=c++ */
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
extern "C" void FUN_0889207c(void *, const void *);
extern "C" void FUN_08b899e4(bool, void *, void *, void *);

struct Sub {
    int a[4];
    Sub(const Sub &o) { FUN_0889207c(this, &o); }
};

struct Value {
    int key;
    Sub val;
};

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node : NodeBase {
    Value val;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
};

extern "C" Node *FUN_088922d4(Tree *self, Node *x, Node *p, const Value *v)
{
    bool insert_left = (x != 0 || p == &self->header || v->key < p->val.key);
    Heap *h = sys_heap();
    FUN_08ad3088(&h->lock);
    Node *z = (Node *)FUN_08adbb84(h, 36);
    FUN_08ad2810(&h->lock);
    new (&z->val) Value(*v);
    FUN_08b899e4(insert_left, z, p, &self->header);
    self->count++;
    return z;
}
