/* LBPMATCH module=EBOOT.BIN start=0x088374f0 end=0x08837568 symbol=FUN_088374f0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08adbc24(void *g, void *n, int sz);
extern void *DAT_08be84a0;
struct Node { int pad[2]; Node *left; Node *right; };
struct Tree { int pad[2]; Node *root; Node *leftmost; Node *rightmost; int count; };
extern "C" void FUN_08837090(Tree *t, Node *n);
extern "C" void FUN_088374f0(Tree *t)
{
  Node *n = t->root;
  while (n) {
    FUN_08837090(t, n->right);
    Node *l = n->left;
    FUN_08adbc24(DAT_08be84a0, n, 48);
    n = l;
  }
  Node *h = (Node *)&t->pad[1];
  t->leftmost = h;
  t->root = 0;
  t->rightmost = h;
  t->count = 0;
}
