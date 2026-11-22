/* LBPMATCH module=EBOOT.BIN start=0x088c2444 end=0x088c2524 symbol=FUN_088c2444 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0883c3b4(void *);

struct Task {
    void (**vt)(Task *);
    int rc;
};

struct TaskPtr {
    Task *p;
    inline TaskPtr() : p(0) {}
    inline ~TaskPtr()
    {
        Task *r = p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[2](r);
            }
        }
    }
};

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node : NodeBase {
    int key;
    void *val;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
};

struct Obj {
    void (**vt)(Obj *, void *, TaskPtr *, int);
    unsigned char pad[0x54 - 4];
    Tree tree;
};

extern "C" Node *FUN_08b898a0(Node *);

extern "C" void FUN_088c2444(Obj *self)
{
    Tree *t = &self->tree;
    if (t->count != 0) {
        for (Node *n = (Node *)t->header.left; n != (Node *)&t->header; n = FUN_08b898a0(n)) {
            TaskPtr tp;
            self->vt[13](self, &n->val, &tp, 0);
        }
        FUN_0883c3b4(t);
    }
}
