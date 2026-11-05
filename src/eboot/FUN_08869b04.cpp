/* LBPMATCH module=EBOOT.BIN start=0x08869b04 end=0x08869b5c symbol=FUN_08869b04 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08869a34(void *a, void *b);
struct Node { Node *next; int pad; char data[1]; };
struct Obj { int pad[50]; char sub[1]; };
extern "C" int FUN_08869b04(Obj *o, Node *head)
{
  for (Node *n = head->next; n != head; n = n->next) FUN_08869a34(o->sub, n->data);
  return 1;
}
