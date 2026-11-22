/* LBPMATCH module=EBOOT.BIN start=0x089784c4 end=0x0897855c symbol=FUN_089784c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
struct Node;
struct VT { int pad[6]; int (*handle)(struct Node *n, struct Obj **ref); };
struct Node { const struct VT *vt; };
struct Stack { struct Node **base; short idx; };
struct Self { char pad[20]; struct Stack *stk; };
extern void FUN_08970b6c(struct Obj *o);

static inline short count(struct Stack *s)
{
  return s->idx + 1;
}

int FUN_089784c4(struct Self *self, struct Obj **pref)
{
  struct Stack *s = self->stk;
  int r = 1;
  if (count(s) != 0) {
    struct Node *n = s->base[s->idx];
    if (n != 0) {
      struct Obj *tmp = *pref;
      int (*fn)(struct Node *, struct Obj **) = n->vt->handle;
      if (tmp != 0)
        tmp->rc = tmp->rc + 1;
      r = fn(n, &tmp);
      if (tmp != 0)
        FUN_08970b6c(tmp);
    }
  }
  return r;
}
