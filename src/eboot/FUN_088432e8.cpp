/* LBPMATCH module=EBOOT.BIN start=0x088432e8 end=0x08843338 symbol=FUN_088432e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct N {
  virtual void v0(); virtual void release();
  int pad[9];
  N *next; /* +0x28 */
};
struct Obj { int pad[6]; N *head; N *tail; };
extern "C" void FUN_088432e8(Obj *o)
{
  N *n = o->head;
  while (n) {
    N *next = n->next;
    n->release();
    n = next;
  }
  o->head = 0;
  o->tail = 0;
}
