/* LBPMATCH module=EBOOT.BIN start=0x08894cbc end=0x08894d6c symbol=FUN_08894cbc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08b898a0(void *);
extern void FUN_08893414(void *);
extern void FUN_08894c1c(void *);

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
    int pad[12];
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
};

struct Obj {
    int f0;
    void (*cb)(void *, void *);
    void *ctx;
    struct Ring *ring;
    struct Tree tree;
};

void FUN_08894cbc(struct Obj *self)
{
    struct Ring *r;
    if (self->cb != 0) {
        struct Tree *t = &self->tree;
        struct Node *n;
        for (n = t->header.left; n != &t->header; n = (struct Node *)FUN_08b898a0(n)) {
            self->cb(&n->val, self->ctx);
        }
    }
    FUN_08893414(&self->tree);
    r = self->ring;
    if (r != 0) {
        unsigned int i = r->head;
        unsigned int tail = r->tail;
        while (i != tail) {
            i = (i + 1) & (r->cap - 1);
        }
        r->head = 0;
        r->tail = 0;
        FUN_08894c1c(r->sub);
        r->flag = 0;
    }
}
