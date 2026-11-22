/* LBPMATCH module=EBOOT.BIN start=0x08932ec4 end=0x08932fc4 symbol=FUN_08932ec4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void destroy();
  int refs;
};
struct Item { char pad[52]; int state; };
struct Node { Node *next; Node *prev; Item *item; };
struct Heap { char pad[32]; char lock[1]; };
struct Lock { int x; };
struct Owner { char pad[396]; Node head; };
extern "C" Lock DAT_08bef8f4;
extern "C" Heap *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *heap, void *p, unsigned size);
extern "C" void FUN_08931c30(Item *);
extern "C" void FUN_08932ec4(Owner *o)
{
  Node *h = &o->head;
  Node *n = h->next;
  if (n != h) {
    Node *p = n;
  while (p != h) {
    Item *it = p->item;
    it->state = 9;
    FUN_08931c30(it);
    p = p->next;
  }
  n = h->next;
  while (n != h) {
    Obj *r = (Obj *)n->item;
    Node *next = n->next;
    if (r) {
      FUN_08ad3088(&DAT_08bef8f4);
      int c = r->refs - 1;
      r->refs = c;
      FUN_08ad2810(&DAT_08bef8f4);
      if (c == 0) r->destroy();
    }
    Heap *heap = DAT_08be84ac;
    FUN_08ad3088(heap->lock);
    FUN_08adbc24(heap, n, 12);
    FUN_08ad2810(heap->lock);
    n = next;
  }
  h->prev = h;
  h->next = h;
  }
}
// NEAR 58 words (63 vs 64 words; the difference is structural, rest of the loops match): retail enters loop 1 by 'bne v0,s2,TEST; move s0,v0; j END' (jump into the loop's bottom test, header not copied), ours drops the entry test and falls into the body (beq v0,s2,END).
// Tried: early return, if-wrapper, first/n copies, while/for(;;)/do shapes, local copy p of the iterator: all give the rotated form.
// near miss from lane api-lbp-a-20261121 (unreviewed)
