/* LBPMATCH module=EBOOT.BIN start=0x08988f6c end=0x0898904c symbol=FUN_08988f6c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct M { struct Rc *a; struct Rc *b; int c; int d; };
struct Node {
  const void *vt;
  short size;
  short rc;
  int x;
  struct M m;
};
extern char DAT_08bbf5e0[];
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);

static inline void copy_m(struct M *d, struct M *s)
{
  struct Rc *r = s->a;
  d->a = r;
  if (r != 0) {
    FUN_08ad3088(r->lock);
    r->count = r->count + 1;
    FUN_08ad2810(r->lock);
  }
  r = s->b;
  d->b = r;
  if (r != 0) {
    FUN_08ad3088(r->lock);
    r->count = r->count + 1;
    FUN_08ad2810(r->lock);
  }
}

void FUN_08988f6c(struct Node *n, int x, struct M *src)
{
  n->size = -1;
  n->rc = 1;
  n->x = x;
  n->vt = DAT_08bbf5e0;
  int dd, cc;
  struct M *mp = &n->m;
  copy_m(mp, src);
  cc = src->c;
  dd = src->d;
  n->size = 28;
  mp->c = cc;
  mp->d = dd;
}
