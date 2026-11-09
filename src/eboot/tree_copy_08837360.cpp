/* LBPMATCH module=EBOOT.BIN start=0x08837360 end=0x088374f0 symbol=FUN_08837360 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5): libstdc++'s std::_Rb_tree::_M_copy (GCC 4.0) with _M_clone_node/_M_create_node inlined and a 32-byte value; exact match verified; names and types await human review. */
inline void *operator new(unsigned int, void *p) throw() { return p; }
extern "C" void *FUN_08adbb84(void *heap, unsigned int size);
extern void *DAT_08be84a0;
struct Data_08837360 { int w[7]; };
struct Value_08837360 { int key; Data_08837360 data; };
struct Node_08837360 {
  int color;
  Node_08837360 *parent;
  Node_08837360 *left;
  Node_08837360 *right;
  Value_08837360 value;
};
struct Tree_08837360 {
  Node_08837360 *create_node(const Value_08837360 &v)
  {
    Node_08837360 *tmp = (Node_08837360 *)FUN_08adbb84(DAT_08be84a0, sizeof(Node_08837360));
    ::new (static_cast<void *>(&tmp->value)) Value_08837360(v);
    return tmp;
  }
  Node_08837360 *clone_node(const Node_08837360 *x)
  {
    Node_08837360 *tmp = create_node(x->value);
    tmp->color = x->color;
    tmp->left = 0;
    tmp->right = 0;
    return tmp;
  }
};
extern "C" Node_08837360 *FUN_08837360(Tree_08837360 *t, const Node_08837360 *x, Node_08837360 *p)
{
  Node_08837360 *top = t->clone_node(x);
  top->parent = p;
  if (x->right)
    top->right = FUN_08837360(t, x->right, top);
  p = top;
  x = x->left;
  while (x != 0) {
    Node_08837360 *y = t->clone_node(x);
    p->left = y;
    y->parent = p;
    if (x->right)
      y->right = FUN_08837360(t, x->right, y);
    p = y;
    x = x->left;
  }
  return top;
}
