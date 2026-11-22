/* LBPMATCH module=EBOOT.BIN start=0x08997164 end=0x0899721c symbol=FUN_08997164 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Rc { int a; int b; int count; int c; int d; char lock[4]; };
struct D { char base[24]; struct Rc *ra; struct Rc *rb; };

extern void FUN_08a9c0b4(struct D *self, const struct D *o);
extern void FUN_08ad3088(void *lock);
extern void FUN_08ad2810(void *lock);

void FUN_08997164(struct D *self, const struct D *o)
{
  struct Rc *q;

  FUN_08a9c0b4(self, o);
  q = o->ra;
  self->ra = q;
  if (q) {
    FUN_08ad3088(q->lock);
    q->count = q->count + 1;
    FUN_08ad2810(q->lock);
  }
  q = o->rb;
  self->rb = q;
  if (q) {
    FUN_08ad3088(q->lock);
    q->count = q->count + 1;
    FUN_08ad2810(q->lock);
  }
}
