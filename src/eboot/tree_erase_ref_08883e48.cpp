/* LBPMATCH module=EBOOT.BIN start=0x08883e48 end=0x08883f20 symbol=FUN_08883e48 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void destroy();
  int refs;
};
struct Node {
  char pad[8];
  Node *next;
  Node *child;
  char pad2[16];
  Obj *obj;
};
struct Pool {
  char pad[32];
  char lock[1];
};
struct Tree { int pad; };
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" Pool *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_08adbc24(Pool *pool, void *p, int size);

extern "C" void FUN_08883e48(Tree *t, Node *n)
{
  while (n) {
    FUN_08883e48(t, n->child);
    Obj *o = n->obj;
    Node *next = n->next;
    if (o) {
      FUN_08ad3088(&DAT_08bef8f4);
      int r = o->refs - 1;
      o->refs = r;
      FUN_08ad2810(&DAT_08bef8f4);
      if (r == 0) o->destroy();
    }
    Pool *pool = DAT_08be84ac;
    FUN_08ad3088(pool->lock);
    FUN_08adbc24(pool, n, 0x24);
    FUN_08ad2810(pool->lock);
    n = next;
  }
}
