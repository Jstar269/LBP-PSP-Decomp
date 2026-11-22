/* LBPMATCH module=EBOOT.BIN start=0x089e304c end=0x089e3140 symbol=FUN_089e304c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adbb84(void *, int);
extern "C" void FUN_08b88fac(void *, void *);
extern "C" void FUN_089e1b98(void *);
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
struct Heap { char pad[32]; Mutex lock; };
extern Heap *DAT_08be84ac;
struct Node;
struct Obj {
  char b0;
  char pad1[4779];
  char sub[20];
  int f4800;
  Node *f4804;
};
struct Node { int a; int b; Obj *item; };
struct Pool { int pad0; Obj **stack; int pad8[2]; int top; };
struct Mgr {
  char pad0[56];
  Pool pool;
  char pad76[72];
  int list;
};
extern "C" Obj *FUN_089e304c(Mgr *m, int v)
{
  Pool *pl = &m->pool;
  int *lst = &m->list;
  FUN_08ad3088(&DAT_08bef8f4);
  Obj *o = pl->stack[pl->top++];
  if (o) {
    FUN_089e1b98(o->sub);
    o->b0 = 0;
    o->f4800 = 0;
    o->f4804 = 0;
  }
  FUN_08ad2810(&DAT_08bef8f4);
  o->f4800 = v;
  Heap *h = DAT_08be84ac;
  FUN_08ad3088(&h->lock);
  Node *n = (Node *)FUN_08adbb84(h, 12);
  FUN_08ad2810(&h->lock);
  new (&n->item) Obj *(o);
  FUN_08b88fac(n, lst);
  o->f4804 = n;
  return o;
}
// NEAR 60 words (59/61): structure and offsets right (pool pop under the global lock, ctor of the sub-object, heap node under heap->lock, placement-new null check, list hook); only register roles differ: retail keeps the manager pointer in s1 (hi in s4, list in s5, v in s3), ours uses v0 for it and is 2 words shorter.
// near miss from lane api-lbp-d-20261121 (unreviewed)
