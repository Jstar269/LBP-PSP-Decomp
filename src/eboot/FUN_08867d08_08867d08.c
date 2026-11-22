/* LBPMATCH module=EBOOT.BIN start=0x08867d08 end=0x08867de0 symbol=FUN_08867d08 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

struct Heap {
    unsigned char pad[32];
    int lock;
};
extern struct Heap *DAT_08be84ac;

extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_08adbc24(struct Heap *, void *, int);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
    int key;
    struct Ref *ref;
};

void FUN_08867d08(void *self, struct Node *node)
{
    while (node != 0) {
        struct Ref *r;
        struct Node *left;
        FUN_08867d08(self, node->right);
        r = node->ref;
        left = node->left;
        if (r != 0) {
            int c;
            FUN_08ad3088(&DAT_08bef8f4);
            c = --r->count;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
        {
            struct Heap *h = DAT_08be84ac;
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, node, 24);
            FUN_08ad2810(&h->lock);
        }
        node = left;
    }
}
