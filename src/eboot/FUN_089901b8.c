/* LBPMATCH module=EBOOT.BIN start=0x089901b8 end=0x08990268 symbol=FUN_089901b8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct M { struct Rc *a; struct Rc *b; int c; int d; };
struct P { struct Rc *r; };
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

void FUN_089901b8(struct M *d, struct P *p1, struct P *p2, int c)
{
  struct Rc *r = p1->r;
  d->a = r;
  addref(r);
  r = p2->r;
  d->b = r;
  addref(r);
  d->c = c;
  d->d = 0;
}
