/* LBPMATCH module=EBOOT.BIN start=0x0887c464 end=0x0887c560 symbol=FUN_0887c464 lang=c++ */
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
extern "C" void FUN_08b899e4(bool, void *, void *, void *);

struct Value {
    unsigned key;
    int a;
    int b;
    int c;
    int d;
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

extern "C" Node *FUN_0887c464(Tree *self, Node *x, Node *p, const Value *v)
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
