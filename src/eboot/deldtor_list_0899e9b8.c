/* LBPMATCH module=EBOOT.BIN start=0x0899e9b8 end=0x0899ea64 symbol=FUN_0899e9b8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Obj;
struct ObjVT { void *pad; void (*destroy)(struct Obj *); };
struct Obj { struct ObjVT *vt; };
struct Node { struct Node *next; struct Node *prev; struct Obj *obj; };
struct Self { void *vt; char pad[108]; struct Node head; };

extern void *DAT_08bc04d8[];
extern void *DAT_08bc0538[];
extern int DAT_08be84a0;

extern void FUN_08adbc24(int heap, void *p, int size);
extern void FUN_0896cd08(struct Self *self);
extern void FUN_08a9e0cc(void *p);

void FUN_0899e9b8(struct Self *self)
{
  struct Node *n;
  struct Node *next;
  struct Node *h = &self->head;

  self->vt = DAT_08bc04d8;
  for (n = h->next; n != h; n = n->next) {
    if (n->obj != 0)
      n->obj->vt->destroy(n->obj);
  }
  h = (struct Node *)((char *)self + 112);
  n = h->next;
  while (n != h) {
    next = n->next;
    FUN_08adbc24(DAT_08be84a0, n, 24);
    n = next;
  }
  self->vt = DAT_08bc0538;
  FUN_0896cd08(self);
  FUN_08a9e0cc(self);
}
// matching idiom: the second loop re-derives the list sentinel as (char *)self + 112 (retail recomputes addiu s2,s1,112).
