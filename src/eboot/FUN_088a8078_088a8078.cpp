/* LBPMATCH module=EBOOT.BIN start=0x088a8078 end=0x088a8114 symbol=FUN_088a8078 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" void *DAT_08be84a0;

extern "C" void *FUN_08adbb84(void *, int);
extern "C" void FUN_08b88fac(void *, void *);

struct Item {
    unsigned short a;
    unsigned short b;
    unsigned short c;
};

struct Node {
    Node *next;
    Node *prev;
    Item item;
};

static inline void push_back(Node *dst, const Item &x)
{
    Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, 16);
    new (&n->item) Item(x);
    FUN_08b88fac(n, dst);
}

extern "C" void FUN_088a8078(Node *dst, Node *src)
{
    dst->next = dst;
    dst->prev = dst;
    for (Node *cur = src->next; cur != src; cur = cur->next) {
        push_back(dst, cur->item);
    }
}
