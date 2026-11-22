/* LBPMATCH module=EBOOT.BIN start=0x0899e2f4 end=0x0899e3ac symbol=FUN_0899e2f4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Rc1 { int a; int b; int count; int c; int d; char lock[4]; };
struct Rc2 { int count; };
struct Foo { struct Rc1 *a; int x; struct Rc2 *b; };

extern void FUN_08ad3088(void *lock);
extern void FUN_08ad2810(void *lock);
extern void FUN_088050b8(void *p);
extern void FUN_0890451c(struct Rc2 **p);

struct Foo *FUN_0899e2f4(struct Foo *self, const struct Foo *o)
{
  struct Rc1 *q;
  struct Rc1 *old;
  struct Rc2 *t;
  struct Rc2 *old2;

  q = o->a;
  if (q) {
    FUN_08ad3088(q->lock);
    q->count = q->count + 1;
    FUN_08ad2810(q->lock);
  }
  old = self->a;
  self->a = q;
  if (old)
    FUN_088050b8(old);
  self->x = o->x;
  t = o->b;
  if (t)
    t->count = t->count + 1;
  old2 = self->b;
  self->b = t;
  t = old2;
  FUN_0890451c(&t);
  return self;
}
