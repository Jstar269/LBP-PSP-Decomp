/* LBPMATCH module=EBOOT.BIN start=0x089e3cb0 end=0x089e3d90 symbol=FUN_089e3cb0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(void *, void *, int);
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
struct Heap { char pad[32]; Mutex lock; };
extern Heap *DAT_08be84ac;
struct Obj;
struct Key;
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Node : NodeBase { Obj *val; };
struct Tree { int compare; NodeBase header; unsigned count; };
extern "C" Node *FUN_089e2cd4(Tree *t, Key *const *k);
extern "C" void *FUN_08b89b70(Node *n, NodeBase *header);
struct Pool { int pad0; Obj **stack; int pad8[2]; int top; };
struct Mgr {
  char pad0[84];
  Pool pool;
  char pad104[12];
  Tree tree;
};
extern "C" void FUN_089e3cb0(Mgr *m, Key *k)
{
  Key *key = k;
  Tree *t = &m->tree;
  Pool *p = &m->pool;
  Node *n = FUN_089e2cd4(t, &key);
  Obj *o = n->val;
  FUN_08ad3088(&DAT_08bef8f4);
  p->stack[--p->top] = o;
  FUN_08ad2810(&DAT_08bef8f4);
  void *z = FUN_08b89b70(n, &m->tree.header);
  Heap *h = DAT_08be84ac;
  FUN_08ad3088(&h->lock);
  FUN_08adbc24(h, z, 24);
  FUN_08ad2810(&h->lock);
  t->count--;
}
