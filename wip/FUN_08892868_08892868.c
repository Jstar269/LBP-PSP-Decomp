/* LBPMATCH module=EBOOT.BIN start=0x08892868 end=0x08892928 symbol=FUN_08892868 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08b898a0(void *);
extern void FUN_08891aa8(void *);
extern void FUN_088927c8(void *);

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
    int pad[4];
    int val;
};

struct Tree {
    int pad;
    struct Node header;
};

struct Ring {
    unsigned int tail;
    unsigned int head;
    unsigned int cap;
    unsigned char flag;
    unsigned char pad1[3];
    int sub[6];
    unsigned int *arr;
};

struct Obj {
    int f0;
    void (*cb)(void *, void *);
    void *ctx;
    struct Ring *ring;
    struct Tree tree;
};

void FUN_08892868(struct Obj *self)
{
    struct Ring *r;
    if (self->cb != 0) {
        struct Tree *t = &self->tree;
        struct Node *n;
        for (n = t->header.left; n != &t->header; n = (struct Node *)FUN_08b898a0(n)) {
            self->cb(&n->val, self->ctx);
        }
    }
    FUN_08891aa8(&self->tree);
    r = self->ring;
    if (r != 0) {
        unsigned int i = r->head;
        unsigned int tail = r->tail;
        while (i != tail) {
            unsigned int *arr = r->arr;
            unsigned int cap = r->cap;
            arr[i] = 7;
            i = (i + 1) & (cap - 1);
        }
        r->head = 0;
        r->tail = 0;
        FUN_088927c8(r->sub);
        r->flag = 0;
    }
}
// near miss from lane oc-lbp-20261121 (unreviewed)
