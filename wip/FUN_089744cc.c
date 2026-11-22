/* LBPMATCH module=EBOOT.BIN start=0x089744cc end=0x089745b8 symbol=FUN_089744cc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
struct Node;
struct VT { int pad[2]; int (*handle)(struct Node *n, struct Obj **ref); };
struct Node { const struct VT *vt; int pad; struct Node *next; };
struct Self { char pad0[8]; struct Node *head; char pad1[16]; int key; struct Obj *ref; };
struct Ev { char pad[8]; int key; };
extern void FUN_08970b6c(struct Obj *o);

int FUN_089744cc(struct Self *self, struct Ev **pe)
{
  struct Ev *e = *pe;
  int r;
  if (e->key == self->key) {
    struct Obj *ref = self->ref;
    struct Node *n;
    if (ref != 0)
      ref->rc = ref->rc + 1;
    for (n = self->head; n != 0; n = n->next) {
      int (*fn)(struct Node *, struct Obj **) = n->vt->handle;
      struct Obj *tmp = ref;
      int ok;
      if (ref != 0)
        ref->rc = ref->rc + 1;
      ok = fn(n, &tmp);
      if (tmp != 0)
        FUN_08970b6c(tmp);
      if (ok == 0)
        break;
    }
    r = 0;
    if (ref != 0) {
      FUN_08970b6c(ref);
      r = 0;
    }
  } else {
    struct Obj *o = (struct Obj *)e;
    r = 1;
    if (o != 0) {
      o->rc = o->rc + 1;
      FUN_08970b6c(o);
      r = 1;
    }
  }
  return r;
}
// NEAR: 18 words differ (56/59). Structure matches (event key compare, ref-counted copy per handler call, release after). Retail keeps the `if (e != 0)` test on the event pointer in the key-mismatch arm even though e->key was read first
// (`bne ..; nop` / `beqz a1`, shared epilogue); ours deletes it as dominated by the dereference (`bnel` with the lhu hoisted). Tried: cast to Obj* (same value, folded).
// near miss from lane api-lbp-b-20261121 (unreviewed)
