/* LBPMATCH module=EBOOT.BIN start=0x0898c03c end=0x0898c0e8 symbol=FUN_0898c03c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct M { struct Rc *a; struct Rc *b; int c; int d; };
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);

static inline void addref(struct Rc *r)
{
  if (r != 0) {
    FUN_08ad3088(r->lock);
    r->count = r->count + 1;
    FUN_08ad2810(r->lock);
  }
}

void FUN_0898c03c(struct M *d, struct M *s)
{
  struct Rc *r = s->a;
  d->a = r;
  addref(r);
  r = s->b;
  d->b = r;
  addref(r);
  d->c = s->c;
  d->d = s->d;
}
