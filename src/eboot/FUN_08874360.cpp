/* LBPMATCH module=EBOOT.BIN start=0x08874360 end=0x08874414 symbol=FUN_08874360 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Pool {
  char pad[32];
  char lock[1];
};
extern "C" Pool *DAT_08be84ac;
extern "C" char DAT_08bb05c8[];
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088741f0(void *self);
extern "C" void FUN_08878b78(void *p);
extern "C" void FUN_08873a2c(void *self);
extern "C" void FUN_08a9e0cc(void *self);
extern "C" void FUN_08adbc24(void *heap, void *p, unsigned size);
struct Node { Node *next; Node *prev; void *item; };
struct List { Node *next; Node *prev; };
struct Self {
  void *vtbl;
  char pad[0x1017c];
  List list;       /* +0x10180 */
  int pad188;
  char sub[1];     /* +0x1018c */
};
extern "C" void FUN_08874360(Self *self)
{
  self->vtbl = DAT_08bb05c8;
  FUN_088741f0(self);
  FUN_08878b78(self->sub);
  Node *head = (Node *)&self->list;
  Node *n = head->next;
  while (n != head) {
    Node *next = n->next;
    Pool *p = DAT_08be84ac;
    FUN_08ad3088(p->lock);
    FUN_08adbc24(p, n, 16);
    FUN_08ad2810(p->lock);
    n = next;
  }
  FUN_08873a2c(self);
}
