/* LBPMATCH module=EBOOT.BIN start=0x08872ad8 end=0x08872bb8 symbol=FUN_08872ad8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned int, void *p) throw() { return p; }
extern "C" int DAT_08bef8f4;
struct Pool {
  char pad[32];
  char lock[1];
};
extern "C" Pool *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adbb84(void *heap, unsigned size);
extern "C" void FUN_08b88fac(void *node, void *pos);
struct Rc { int pad; int count; };
struct Item {
  int a;
  Rc *r;
  Item(const Item &o) {
    Rc *rr = o.r;
    a = o.a;
    r = rr;
    if (rr) { FUN_08ad3088(&DAT_08bef8f4); rr->count++; FUN_08ad2810(&DAT_08bef8f4); }
  }
};
struct Node { Node *next; void *prev; Item item; };
extern "C" void FUN_08872ad8(void *unused, void *pos, Node *first, Node *last)
{
  for (Node *n = first; n != last; n = n->next) {
    Pool *p = DAT_08be84ac;
    const Item &src = n->item;
    FUN_08ad3088(p->lock);
    Node *z = (Node *)FUN_08adbb84(p, sizeof(Node));
    FUN_08ad2810(p->lock);
    new ((char *)z + 8) Item(src);
    FUN_08b88fac(z, pos);
  }
}
