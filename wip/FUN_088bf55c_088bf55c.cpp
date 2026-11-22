/* LBPMATCH module=EBOOT.BIN start=0x088bf55c end=0x088bf5fc symbol=FUN_088bf55c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb37a8[];
extern "C" void *DAT_08bf03b8;
extern "C" void FUN_088b2dd0(void *, int, int);
extern "C" void FUN_08b9eb88(void *, int, int);
extern "C" void *FUN_08a9e838(int);
extern "C" void FUN_0896cc58(void *);
extern "C" void FUN_0896f85c(void *, int, void *);

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
        header.color = 0;
        header.parent = 0;
        header.left = &header;
        header.right = &header;
        count = 0;
    }
};

struct Obj {
    void **vt;
    int pad[4];
    void *f14;
    Tree tree;
    void *f30;
};

extern "C" void FUN_088bf55c(Obj *self)
{
    FUN_088b2dd0(self, 13, 17);
    self->vt = DAT_08bb37a8;
    self->f14 = 0;
    self->tree.init();
    void *p = FUN_08a9e838(0x68);
    FUN_0896cc58(p);
    self->f30 = p;
    FUN_0896f85c(DAT_08bf03b8, 5, p);
}

// Remaining diffs (40/40 words; 2 differ at +0x3c): adjacent instruction swap between `sw v0,0(s2)` and `move a0,s1` before `jal 0x8b9eb88`.
// near miss from lane oc-lbp-20261121 (unreviewed)
