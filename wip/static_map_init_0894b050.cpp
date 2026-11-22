/* LBPMATCH module=EBOOT.BIN start=0x0894b050 end=0x0894b110 symbol=FUN_0894b050 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
struct Node { int pad[2]; Node *left; Node *right; int pad2[2]; };
extern "C" void *FUN_08b9eb88(void *, int, int);
extern "C" void FUN_08adbc24(void *, void *, int);
extern "C" void *DAT_08be84a0;
struct Tree {
    int compare; int color; Node *parent; Node *left; Node *right; unsigned count;
    void init()
    {
        Node *h = (Node *)&color;
        FUN_08b9eb88(h, 0, 16);
        right = h;
        count = 0;
        color = 0;
        parent = 0;
        left = h;
    }
};
extern "C" Tree DAT_08bef988;
extern "C" void FUN_0883be04(Tree *, Node *);
extern "C" void FUN_0894b050(int init, int prio)
{
    if (init == 1) {
        if (prio == 0xffff)
            DAT_08bef988.init();
    } else if (init == 0 && prio == 0xffff) {
        Node *n = DAT_08bef988.parent;
        while (n) {
            FUN_0883be04(&DAT_08bef988, n->right);
            Node *next = n->left;
            FUN_08adbc24(DAT_08be84a0, n, 24);
            n = next;
        }
    }
}
// NEAR 32 (48 of 48 words, same code shape): global std::map-like init/teardown guarded by (init==1/0, prio==0xffff). Logic and loop match; retail keeps the global's address in one callee-saved register (lui s0; addiu s0,s0,-1656) with the header at s1 = s0+4 and stores 'sw s1,16(s0)', 'sw zero,20(s0)', ..., ours rebuilds lui/addiu per access and folds the +4 into a constant. Member-function init(), direct field stores and a Tree* local did not change that.
// near miss from lane api-lbp-a-20261121 (unreviewed)
