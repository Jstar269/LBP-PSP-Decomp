/* LBPMATCH module=EBOOT.BIN start=0x08891d64 end=0x08891e3c symbol=FUN_08891d64 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;

extern "C" void *FUN_08891ae4(void *, void *);
extern "C" void FUN_08891c90(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Ref {
    void *vt;
    int rc;
};

struct SharedPtr {
    Ref *p;
    inline SharedPtr() : p(0) {}
    inline SharedPtr(const SharedPtr &o)
    {
        Ref *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            r->rc++;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct Node {
    unsigned char pad1[28];
    int state;
    SharedPtr ref;
};

struct Tree {
    int pad;
    Node header;
};

struct Obj {
    int f0;
    int f4;
    int f8;
    void *f0c;
    Tree tree;
};

extern "C" SharedPtr FUN_08891d64(Obj *self, void *p3, bool p4)
{
    Node *n = (Node *)FUN_08891ae4(&self->tree, p3);
    if (n == &self->tree.header) {
        return SharedPtr();
    }
    if (p4 && n->state != 1) {
        return SharedPtr();
    }
    if (self->f0c != 0) {
        FUN_08891c90(self->f0c, p3);
    }
    return n->ref;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
