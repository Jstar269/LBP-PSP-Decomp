/* LBPMATCH module=EBOOT.BIN start=0x088436b0 end=0x08843714 symbol=FUN_088436b0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Node {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
  virtual int test();
  int pad[9];
  Node *next; /* +0x28 */
};
struct Owner { int pad[10]; Node *head; };
extern "C" int FUN_088436b0(Owner *o)
{
  for (Node *p = o->head; p; p = p->next)
    if (p->test()) return p->test();
  return 0;
}
