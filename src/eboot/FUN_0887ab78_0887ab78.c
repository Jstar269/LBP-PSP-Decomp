/* LBPMATCH module=EBOOT.BIN start=0x0887ab78 end=0x0887ac18 symbol=FUN_0887ab78 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Heap {
    unsigned char pad[32];
    int lock;
};
extern struct Heap *DAT_08be84ac;

extern void FUN_0887a8c8(void *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_08adbc24(struct Heap *, void *, int);

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};

struct Tree {
    int pad;
    struct Node header;
    int count;
};

void FUN_0887ab78(struct Tree *self)
{
    struct Node *x = self->header.parent;
    while (x != 0) {
        struct Node *left;
        FUN_0887a8c8(self, x->right);
        left = x->left;
        {
            struct Heap *h = DAT_08be84ac;
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, x, 40);
            FUN_08ad2810(&h->lock);
        }
        x = left;
    }
    self->count = 0;
    self->header.left = &self->header;
    self->header.parent = 0;
    self->header.right = &self->header;
}
