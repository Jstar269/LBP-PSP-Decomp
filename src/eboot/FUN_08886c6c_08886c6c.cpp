/* LBPMATCH module=EBOOT.BIN start=0x08886c6c end=0x08886d50 symbol=FUN_08886c6c lang=c++ */
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

extern "C" void FUN_08886ad4(void *);
extern "C" void *FUN_08b89838(void *);
extern "C" void *FUN_08b89b70(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *, void *, int);

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
};

extern "C" void FUN_08886c6c(Tree *self, NodeBase *first, NodeBase *last)
{
    if (first == self->header.left && last == &self->header) {
        FUN_08886ad4(self);
        return;
    }
    while (first != last) {
        NodeBase *next = (NodeBase *)FUN_08b89838(first);
        void *y = FUN_08b89b70(first, &self->header);
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        FUN_08adbc24(h, y, 28);
        FUN_08ad2810(&h->lock);
        self->count--;
        first = next;
    }
}
