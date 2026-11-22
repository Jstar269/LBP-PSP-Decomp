/* LBPMATCH module=EBOOT.BIN start=0x08944bf0 end=0x08944cac symbol=FUN_08944bf0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern "C" void FUN_0894a258(void *);
extern "C" void *FUN_08b9eb88(void *, int, int);
extern "C" char DAT_08bb7258;
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Tree {
    int compare;
    NodeBase header;
    unsigned count;
    void init()
    {
        FUN_08b9eb88(&header, 0, 16);
        header.color = 0;
        header.parent = 0;
        header.left = &header;
        header.right = &header;
        count = 0;
    }
};
struct O {
    void *vt;
    int one;
    Tree t1;
    Tree t2;
    int g56, g60, g64;
};
extern "C" void FUN_08944bf0(O *o)
{
    FUN_0894a258(o);
    o->one = 1;
    o->vt = &DAT_08bb7258;
    o->t1.init();
    o->t2.init();
    o->g56 = 0;
    o->g60 = 0;
    o->g64 = 0;
}
// NEAR 3 words: only the order of the last three stores differs (retail: sw s4,16(s1); sw zero,64(s0); sw zero,4(s1); ours 4(s1) before 16(s1)). Brute force: all 120 store orders of the _Rb_tree-style empty init x 6 orders of the three trailing stores (720 compiles) never gave 0; per-tree explicit orders (720) neither.
// Source: member init() of struct Tree{int compare; NodeBase header; unsigned count;}: memset(&header,0,16); right/left=&header; count=0; color=0; parent=0.
// near miss from lane api-lbp-a-20261121 (unreviewed)
