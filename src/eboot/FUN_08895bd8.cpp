/* LBPMATCH module=EBOOT.BIN start=0x08895bd8 end=0x08895c3c symbol=FUN_08895bd8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_08ba0d98(const char *a, const char *b);
struct Node { Node *next; int pad; char name[1]; };
struct Obj { int pad[7]; Node head; };
extern "C" bool FUN_08895bd8(Obj *o, const char *name)
{
  Node *head = &o->head;
  Node *n = head->next;
  while (n != head && FUN_08ba0d98(n->name, name) != 0) n = n->next;
  return n != head;
}
