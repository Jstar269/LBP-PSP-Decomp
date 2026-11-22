/* LBPMATCH module=EBOOT.BIN start=0x089900e0 end=0x089901b8 symbol=FUN_089900e0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct M { struct Rc *a; struct Rc *b; int c; int d; };
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);

static inline void addref(struct Rc *r)
{
  if (r != 0) {
    FUN_08ad3088(r->lock);
    r->count = r->count + 1;
    FUN_08ad2810(r->lock);
  }
}

struct M *FUN_089900e0(struct M *d, struct M *s)
{
  struct Rc *r = s->a;
  struct Rc *old;
  addref(r);
  old = d->a;
  d->a = r;
  if (old != 0)
    FUN_088050b8(old);
  r = s->b;
  addref(r);
  old = d->b;
  d->b = r;
  if (old != 0)
    FUN_088050b8(old);
  d->c = s->c;
  d->d = s->d;
  return d;
}
