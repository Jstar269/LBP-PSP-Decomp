/* LBPMATCH module=EBOOT.BIN start=0x0888b9e4 end=0x0888bab0 symbol=FUN_0888b9e4 lang=c++ */
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
    Node *next;
    Node *prev;
    int f8;
    Ref *ref;
};

extern "C" void FUN_0888b9e4(Node *self)
{
    Node *cur = self->next;
    while (cur != self) {
        Node *next = cur->next;
        Ref *r = cur->ref;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        FUN_08adbc24(h, cur, 16);
        FUN_08ad2810(&h->lock);
        cur = next;
    }
}
