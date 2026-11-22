/* LBPMATCH module=EBOOT.BIN start=0x088c2524 end=0x088c25c8 symbol=FUN_088c2524 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_0893862c(void *);
extern "C" void FUN_089377f0(void *);
extern "C" void FUN_088c22ac(void *);
extern "C" void FUN_088c2444(void *);

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node : NodeBase {
    int key;
    int val;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
};

extern "C" Node *FUN_08b898a0(Node *);

struct Obj {
    int pad1[6];
    unsigned char f18;
    unsigned char f19;
    unsigned char pad2[0x3c - 0x1a];
    Tree t2;
    int pad3[6];
    int arr[8];
    int count;
};

extern "C" void FUN_088c2524(Obj *self)
{
    for (int i = 0; i < self->count; i++) {
        FUN_0893862c(&self->arr[i]);
    }
    self->f19 = (self->count == 0);
    if (self->t2.count != 0) {
        for (Node *n = (Node *)self->t2.header.left; n != (Node *)&self->t2.header; n = FUN_08b898a0(n)) {
            FUN_089377f0(&n->val);
        }
        FUN_088c22ac(&self->t2);
    }
    FUN_088c2444(self);
}

// Remaining diffs (40/41 words; 28 differ): s1/s2 register swap and loop pointer increment scheduling on arr[i].
// near miss from lane oc-lbp-20261121 (unreviewed)
