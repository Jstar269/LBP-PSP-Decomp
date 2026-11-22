/* LBPMATCH module=EBOOT.BIN start=0x089334d0 end=0x089335cc symbol=FUN_089334d0 lang=c++ */
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
struct Owner { char pad[264]; Node head; };
extern "C" Lock DAT_08bef8f4;
extern "C" Heap *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *heap, void *p, unsigned size);
extern "C" void FUN_08930d74(Item *);
extern "C" void FUN_089334d0(Owner *o)
{
  Node *h = &o->head;
  Node *n = h->next;
  if (n != h) {
  while (n != h) {
    FUN_08930d74(n->item);
    n = n->next;
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
// NEAR 60 (shifted; 61 of 63 words): same structural difference as wip/list_clear_refs_08932ec4.cpp: retail has 'bne v0,s2,TEST; move s0,v0; j END' (loop var s0 a separate copy of the wrapper test value, entry jumps to the loop's bottom test); ours threads the entry (beq s0,s2,END, no move).
// near miss from lane api-lbp-a-20261121 (unreviewed)
