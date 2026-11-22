/* LBPMATCH module=EBOOT.BIN start=0x088b795c end=0x088b7a24 symbol=FUN_088b795c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Task {
    void (**vt)(Task *);
    int rc;
};

struct TaskPtr {
    Task *p;
    inline TaskPtr &operator=(const TaskPtr &o)
    {
        Task *n = o.p;
        if (n != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            ++n->rc;
            FUN_08ad2810(&DAT_08bef8f4);
        }
        Task *old = p;
        p = n;
        if (old != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --old->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                old->vt[2](old);
            }
        }
        return *this;
    }
};

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node : NodeBase {
    unsigned int key;
    void *a;
    TaskPtr val;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
};

struct Obj {
    unsigned char pad[0xa0];
    Tree tree;
};

extern "C" Node *FUN_088b6374(Tree *, const unsigned int *);

extern "C" void FUN_088b795c(Obj *self, unsigned int key, TaskPtr *out)
{
    Node *n = FUN_088b6374(&self->tree, &key);
    if (n != (Node *)&self->tree.header) {
        *out = n->val;
    }
}
