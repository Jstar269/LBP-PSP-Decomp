/* LBPMATCH module=EBOOT.BIN start=0x088b9fcc end=0x088ba0c0 symbol=FUN_088b9fcc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void *DAT_08be84a0;
extern "C" void FUN_088b9374(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_08adbc24(void *, void *, int);

struct Task {
    void (**vt)(Task *);
    int rc;
};

struct TaskPtr {
    Task *p;
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

struct RefPtr {
    void *p;
    inline ~RefPtr()
    {
        if (p != 0) {
            FUN_088050b8(p);
        }
    }
};

struct Val {
    RefPtr a;
    TaskPtr b;
    int c;
};

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node : NodeBase {
    int key;
    Val val;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;

    inline void clear()
    {
        Node *x = (Node *)header.parent;
        while (x != 0) {
            FUN_088b9374(this, x->right);
            Node *y = (Node *)x->left;
            x->val.~Val();
            FUN_08adbc24(DAT_08be84a0, x, 32);
            x = y;
        }
        header.left = &header;
        header.parent = 0;
        header.right = &header;
        count = 0;
    }
};

extern "C" void FUN_088b9fcc(Tree *self)
{
    self->clear();
}
