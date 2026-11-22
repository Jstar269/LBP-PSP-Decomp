/* LBPMATCH module=EBOOT.BIN start=0x088b8080 end=0x088b8160 symbol=FUN_088b8080 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct StrView;
extern "C" int FUN_08a9b8a4(StrView *, const char *);
extern "C" void FUN_08a9c3ac(void *, const StrView *);

struct StrView {
    const char *s;
    int len;
    inline StrView(const char *p)
    {
        len = FUN_08a9b8a4(this, p);
        s = p;
    }
};

struct OutStr {
    inline void set(const StrView &sv)
    {
        FUN_08a9c3ac(this, &sv);
    }
};

struct Inner {
    unsigned char pad[0x1c];
    const char *str;
};

struct Item {
    int f0;
    int f4;
    int f8;
    Inner *inner;
};

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Node1 : NodeBase {
    unsigned int key;
    void *a;
    Item *item;
};

struct Node2 : NodeBase {
    unsigned int key;
    Item *item;
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

extern "C" Tree DAT_08bee510;
extern "C" Node1 *FUN_088b6374(Tree *, const unsigned int *);
extern "C" Node2 *FUN_088b6320(Tree *, const unsigned int *);

extern "C" bool FUN_088b8080(Obj *self, unsigned int key, OutStr *out)
{
    Node1 *n1 = FUN_088b6374(&self->tree, &key);
    if (n1 == (Node1 *)&self->tree.header) {
        Node2 *n2 = FUN_088b6320(&DAT_08bee510, &key);
        if (n2 == (Node2 *)&DAT_08bee510.header) {
            return false;
        }
        out->set(StrView(n2->item->inner->str));
        return true;
    }
    out->set(StrView(n1->item->inner->str));
    return true;
}

// Remaining diffs (56/56 words; 8 differ at +0x4c): a1/v1 register swap on the n2 lookup / return value and 2-word delay-slot swap on sw s0,8(sp) vs sw v0,12(sp).
// near miss from lane oc-lbp-20261121 (unreviewed)
