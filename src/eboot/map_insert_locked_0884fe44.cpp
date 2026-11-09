/* LBPMATCH module=EBOOT.BIN start=0x0884fe44 end=0x0884ff20 symbol=FUN_0884fe44 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17g, GPT-6); exact match verified; names and types await human review. */
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Node : NodeBase { unsigned key; };
struct Tree { int compare; NodeBase header; unsigned count; };
struct Heap { char pad[32]; char lock[1]; };
inline Heap *&sys_heap() { static Heap *heap; return heap; }
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adbb84(Heap *, unsigned);
extern "C" void FUN_08b899e4(bool, NodeBase *, NodeBase *, NodeBase *);
template<class T> struct Alloc {
  T *allocate(unsigned n)
  {
    Heap *h = sys_heap();
    FUN_08ad3088(h->lock);
    void *r = FUN_08adbb84(h, n * sizeof(T));
    FUN_08ad2810(h->lock);
    return (T *)r;
  }
};
extern "C" Node *FUN_0884fe44(Tree *tree, int mode, NodeBase *parent, const unsigned &key)
{
  bool insert_left = (mode != 0 || parent == &tree->header || key < static_cast<Node *>(parent)->key);
  Alloc<Node> a;
  Node *node = a.allocate(1);
  if (&node->key != 0)
    node->key = key;
  FUN_08b899e4(insert_left, node, parent, &tree->header);
  ++tree->count;
  return node;
}
