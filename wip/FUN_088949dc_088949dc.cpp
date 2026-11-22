/* LBPMATCH module=EBOOT.BIN start=0x088949dc end=0x08894ad0 symbol=FUN_088949dc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb1060[];

extern "C" void *FUN_08b9eb88(void *, int, unsigned int);
extern "C" void *FUN_08a9e838(unsigned int);
extern "C" void *FUN_08a9e824(unsigned int);

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
    inline void init()
    {
        FUN_08b9eb88(&header, 0, 16);
        count = 0;
        header.color = 0;
        header.parent = 0;
        header.left = &header;
        header.right = &header;
    }
};

struct Sub {
    int tail;
    int head;
    int cap;
    unsigned char flag;
    unsigned char pad[3];
    Tree tree;
    void *arr;
    inline void *operator new(unsigned int sz) { return FUN_08a9e838(sz); }
    inline Sub(int n)
    {
        cap = n;
        tree.init();
        arr = FUN_08a9e824(n * 36);
        flag = 0;
        head = 0;
        tail = 0;
    }
};

struct Obj {
    void **vt;
    int f4;
    int f8;
    Sub *sub;
    Tree tree;
    int cap;
};

extern "C" void FUN_088949dc(Obj *self, int param_2)
{
    self->vt = DAT_08bb1060;
    self->f4 = 0;
    self->f8 = 0;
    self->tree.init();
    self->cap = param_2;
    if (param_2 != 0) {
        self->sub = new Sub(self->cap);
    } else {
        self->sub = 0;
    }
}
// near miss from lane oc-lbp-20261121 (unreviewed)
