/* LBPMATCH module=EBOOT.BIN start=0x088b6260 end=0x088b6320 symbol=FUN_088b6260 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; matching idiom: __attribute__((noinline)) to prevent self-recursion inlining; names and types await human review. */

struct Node;
struct Tree;

struct Tree {
    int pad;
    int color;
    Node *root;
    Node *left;
    Node *right;
    int count;
};

struct Val {
    int f0;
    int f4;
    int f8;
    int f0c;
    Tree sub;
};

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
    int key;
    Val *val;
};

struct Target {
    void *(**vt)(Target *, void *, void *);
};

extern "C" Node *FUN_08b898a0(Node *);

extern "C" void FUN_088b6260(void *self, Target *target, void *arg3, Tree *tree) __attribute__((noinline));

extern "C" void FUN_088b6260(void *self, Target *target, void *arg3, Tree *tree)
{
    if (tree->count != 0) {
        for (Node *n = tree->left; n != (Node *)&tree->color; n = FUN_08b898a0(n)) {
            if (n->val->f0c != 0) {
                void *res = target->vt[11](target, arg3, &n->val->f0c);
                Tree *sub = &n->val->sub;
                if (sub->count != 0) {
                    FUN_088b6260(self, target, res, sub);
                }
            }
        }
    }
}
