/* LBPMATCH module=EBOOT.BIN start=0x088431c4 end=0x08843228 symbol=FUN_088431c4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Node {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual int test();
  int pad[3];
  Node *next; /* +0x10 */
};
extern "C" int FUN_088431c4(Node *p)
{
  for (; p; p = p->next)
    if (p->test()) return p->test();
  return 0;
}
