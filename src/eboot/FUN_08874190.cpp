/* LBPMATCH module=EBOOT.BIN start=0x08874190 end=0x088741f0 symbol=FUN_08874190 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0887394c(void *self, void *a, void *b);
extern "C" void FUN_08878c10(void *p);
extern void *DAT_08bb05c8[];
struct Node { Node *next, *prev; };
struct Obj { void **vt; int pad[16479]; Node head; /* +0x10180 */ int count; /* +0x10188 */ char sub[1]; /* +0x1018c */ };
extern "C" void FUN_08874190(Obj *o, void *a, void *b)
{
  FUN_0887394c(o, a, b);
  o->vt = DAT_08bb05c8;
  Node *h = &o->head;
  h->next = h;
  h->prev = h;
  FUN_08878c10(o->sub);
  o->count = 0;
}
