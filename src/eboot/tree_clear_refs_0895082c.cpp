/* LBPMATCH module=EBOOT.BIN start=0x0895082c end=0x089508d0 symbol=FUN_0895082c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj {
    virtual void v0();
    virtual void destroy();
    int refs;
};
struct Node { int pad[2]; Node *left; Node *right; int pad2[1]; Obj *obj; };
struct Tree { int pad; int header[1]; Node *root; Node *leftmost; Node *rightmost; unsigned count; };
struct Heap { char pad[32]; char lock[1]; };
extern "C" Heap *DAT_08be84a0;
extern "C" void FUN_08adbc24(Heap *heap, void *p, unsigned size);
extern "C" void FUN_08950504(Tree *t, Node *n);
extern "C" void FUN_0895082c(Tree *t)
{
    Node *n = t->root;
    while (n) {
        FUN_08950504(t, n->right);
        Obj *o = n->obj;
        Node *next = n->left;
        if (o && --o->refs == 0)
            o->destroy();
        FUN_08adbc24(DAT_08be84a0, n, 24);
        n = next;
    }
    t->count = 0;
    t->leftmost = (Node *)t->header;
    t->root = 0;
    t->rightmost = (Node *)t->header;
}
