/* LBPMATCH module=EBOOT.BIN start=0x089e04a8 end=0x089e0558 symbol=FUN_089e04a8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item { int pad0; unsigned flags; };
struct Node { Node *next; Node *prev; Item *item; };
struct Iface {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void v5();
  virtual void v6();
};
struct Obj {
  int pad0;
  unsigned flags;
  int pad8[4];
  Node *first;
  Node *lastn;
  Node *cur;
  Node *prev;
  int pad40[2];
  int f48;
  Iface *owner;
};
extern "C" void FUN_089e0608(Iface *);
extern "C" void FUN_089e04a8(Obj *o)
{
  o->owner->v6();
  Node *head = (Node *)&o->first;
  Node *n = o->first;
  o->cur = head;
  o->prev = head;
  o->f48 = 0;
  for (; n != head; n = n->next) {
    unsigned f = n->item->flags;
    if (f & 0x400) continue;
    if (f & 4) {
      if (o->cur == n) return;
      o->prev = o->cur;
      o->cur = n;
      if (o->flags & 0x200) FUN_089e0608(o->owner);
      o->flags |= 0x1000;
      return;
    }
  }
}
// NEAR 12 words: item-pointer register a1 vs ours v0 (retail keeps item and flags in distinct regs), and retail has a separate `j epilogue; lw ra` for the loop-exit return (ours falls through; 2 words shorter).
// Tried: explicit return in loop, inline find() helper, flags read twice: none gives the extra exit jump.
// near miss from lane api-lbp-d-20261121 (unreviewed)
