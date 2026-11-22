/* LBPMATCH module=EBOOT.BIN start=0x089e2cd4 end=0x089e2d84 symbol=FUN_089e2cd4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. libstdc++'s std::_Rb_tree::find for a pointer key with a three-level comparison. */
struct Obj { int pad0; int pad4; unsigned f8; };
struct Key { int a; Obj *b; unsigned c; };
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; Key *key; };
struct Tree { int compare; NodeBase header; };
inline bool less(const Key *x, const Key *y)
{
  bool r = x->a < y->a;
  if (x->a == y->a) {
    unsigned p, q;
    if (x->b == y->b) {
      p = x->c;
      q = y->c;
    } else {
      p = x->b->f8;
      q = y->b->f8;
    }
    r = p < q;
  }
  return r;
}
extern "C" NodeBase *FUN_089e2cd4(Tree *t, Key *const *k)
{
  NodeBase *y = &t->header;
  NodeBase *x = t->header.parent;
  while (x != 0) {
    if (!less(x->key, *k)) {
      y = x;
      x = x->left;
    } else
      x = x->right;
  }
  return (y == &t->header || less(*k, y->key)) ? &t->header : y;
}
// NEAR 40 words (45/44 words): the algorithm is right (lower_bound loop + (j==end||less(k,key(j)))?end:j); retail keeps y in a0 / end in t2 / x in a3 and fills the beq delay slot with `move t0,t2`; ours y=t2 end=a0 x=t1 and a nop slot. Tried local end, inline lower_bound, && end() expressions.
// near miss from lane api-lbp-d-20261121 (unreviewed)
