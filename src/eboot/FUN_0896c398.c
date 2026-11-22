/* LBPMATCH module=EBOOT.BIN start=0x0896c398 end=0x0896c438 symbol=FUN_0896c398 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item;
struct IVT { int pad[7]; void (*f1c)(struct Item *, float); };
struct Item { const struct IVT *vt; unsigned flags; };
struct LNode { struct LNode *next; struct LNode *prev; struct Item *item; };
struct Group { int pad[2]; struct LNode head; int pad2; };
struct Owner { int pad; unsigned flags; int pad1[2]; struct Group groups[2]; };

void FUN_0896c398(float dt, struct Owner *o)
{
  int i;
  struct Group *g;
  if ((o->flags & 2) != 0) {
    g = o->groups;
    i = 0;
    do {
      struct LNode *n;
      struct LNode *end = &g->head;
      for (n = g->head.next; n != end; n = n->next) {
        struct Item *it = n->item;
        if ((it->flags & 4) != 0)
          it->vt->f1c(it, dt);
      }
      i++;
      g++;
    } while (i != 2);
  }
}
