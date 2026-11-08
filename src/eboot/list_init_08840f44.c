/* LBPMATCH module=EBOOT.BIN start=0x08840f44 end=0x08840f64 symbol=FUN_08840f44 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); exact match verified; names and types await human review. */
typedef struct Node { struct Node *next; struct Node *prev; } Node;
typedef struct List { int f0; Node head; Node *cur; int f16; int f20; } List;

void FUN_08840f44(List *l)
{
  Node *h = &l->head;

  l->f0 = 0;
  h->next = h;
  h->prev = h;
  l->cur = h;
  l->f16 = 0;
  l->f20 = 0;
}
