/* LBPMATCH module=EBOOT.BIN start=0x0899385c end=0x089938fc symbol=FUN_0899385c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct Tg;
struct TVT { int pad[2]; void (*f8)(struct Tg *t); };
struct Tg { const struct TVT *vt; int pad[1]; struct Rc *rc; int f12; int f16; };
struct Self { struct Tg *tg; };
struct P { struct Rc *r; };
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

void FUN_0899385c(struct Self *self, int a, struct P *p, int d)
{
  struct Rc *r = p->r;
  struct Tg *t = self->tg;
  struct Rc *old;
  t->f16 = a;
  addref(r);
  old = t->rc;
  t->rc = r;
  if (old != 0)
    FUN_088050b8(old);
  t = self->tg;
  t->f12 = d;
  t->vt->f8(t);
}
