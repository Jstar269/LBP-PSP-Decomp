/* LBPMATCH module=EBOOT.BIN start=0x089a1a40 end=0x089a1b08 symbol=FUN_089a1a40 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Obj;
struct Link { struct Link *next; struct Link *prev; struct Obj *obj; };
struct Obj {
  char pad0[152];
  struct Obj *parent;
  struct Link list;
  char pad1[4];
  unsigned state;
};
struct Self { struct Obj *obj; unsigned char flag; };

static inline void set_group_state(struct Obj *o, unsigned s)
{
  struct Obj *root;
  struct Link *n;

  root = o->parent;
  if (root == 0)
    root = o;
  n = root->list.next;
  o->state = s;
  root->state = s;
  while (n != &root->list) {
    struct Obj *c = n->obj;
    n = n->next;
    c->state = s;
  }
}

void FUN_089a1a40(struct Self *self, int *p)
{
  struct Obj *o;

  if (self->flag != 0) {
    o = self->obj;
    if (o->state - 2 < 2) {
      if (*p == 11)
        set_group_state(o, 1);
    } else if (*p == 10) {
      set_group_state(o, 3);
    }
    return;
  }
  o = self->obj;
  *p = (o->state == 1) ? 11 : 10;
}
