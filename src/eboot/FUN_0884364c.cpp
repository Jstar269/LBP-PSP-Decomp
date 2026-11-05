/* LBPMATCH module=EBOOT.BIN start=0x0884364c end=0x088436b0 symbol=FUN_0884364c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Node {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
  virtual int test();
  int pad[9];
  Node *next; /* +0x28 */
};
struct Owner { int pad[6]; Node *head; };
extern "C" int FUN_0884364c(Owner *o)
{
  for (Node *p = o->head; p; p = p->next)
    if (p->test()) return p->test();
  return 0;
}
