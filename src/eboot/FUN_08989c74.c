/* LBPMATCH module=EBOOT.BIN start=0x08989c74 end=0x08989d3c symbol=FUN_08989c74 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
struct Node;
struct VT { int pad[2]; int (*handle)(struct Node *n, struct Obj **ref); };
struct Node { const struct VT *vt; int pad; struct Node *next; };
struct Self { char pad0[8]; struct Node *head; };
extern void *DAT_08be8510;
extern int FUN_0880b248(void *o);
extern void FUN_08974408(struct Obj **out, int a, int *b);
extern void FUN_08970b6c(struct Obj *o);

int FUN_08989c74(struct Self *self)
{
  int x;
  struct Obj *tmp;
  struct Obj *r;
  int ret = 1;
  if (FUN_0880b248(DAT_08be8510) != 0) {
    struct Node *n;
    FUN_08974408(&r, 1000, &x);
    for (n = self->head; n != 0; n = n->next) {
      int (*fn)(struct Node *, struct Obj **) = n->vt->handle;
      int ok;
      tmp = r;
      if (r != 0)
        r->rc = r->rc + 1;
      ok = fn(n, &tmp);
      if (tmp != 0)
        FUN_08970b6c(tmp);
      if (ok == 0)
        break;
    }
    ret = 0;
    if (r != 0) {
      FUN_08970b6c(r);
      ret = 0;
    }
  }
  return ret;
}
