/* LBPMATCH module=EBOOT.BIN start=0x089e9110 end=0x089e91b0 symbol=FUN_089e9110 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item { virtual void v0(); virtual void v1(); };
struct Node { Node *next; Node *prev; Item *item; };
void operator delete(void *) throw();
extern "C" void *DAT_08be84a0;
extern "C" void FUN_08adbc24(void *heap, void *p, int size);
extern "C" char DAT_08bc2028[];
struct List {
  Node head;
  ~List()
  {
    Node *n = head.next;
    while (n != &head) {
      Node *nx = n->next;
      FUN_08adbc24(DAT_08be84a0, n, 12);
      n = nx;
    }
  }
};
struct Obj {
  const void *vt;
  char pad4[100];
  List list;
};
extern "C" void FUN_0896cd08(Obj *o);
extern "C" void FUN_089e9110(Obj *o)
{
  Node *n = o->list.head.next;
  Node *end = &o->list.head;
  o->vt = DAT_08bc2028;
  for (; n != end; n = n->next) {
    if (n->item != 0)
      n->item->v1();
  }
  o->list.~List();
  FUN_0896cd08(o);
  operator delete(o);
}
