/* LBPMATCH module=EBOOT.BIN start=0x088741f0 end=0x088742a4 symbol=FUN_088741f0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Pool {
  char pad[32];
  char lock[1];
};
extern "C" Pool *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08adbc24(void *heap, void *p, unsigned size);
struct Node { Node *next; Node *prev; void *item; };
struct List { Node *next; Node *prev; };
struct Self {
  char pad[0x10180];
  List list; /* +0x10180 */
};
extern "C" void FUN_088741f0(Self *self)
{
  Node *head = (Node *)&self->list;
  for (Node *n = head->next; n != head; n = n->next) {
    if (n->item)
      FUN_08a9e0c4(n->item);
  }
  Node *n = head->next;
  while (n != head) {
    Node *next = n->next;
    Pool *p = DAT_08be84ac;
    FUN_08ad3088(p->lock);
    FUN_08adbc24(p, n, 16);
    FUN_08ad2810(p->lock);
    n = next;
  }
  head->next = head;
  head->prev = head;
}
