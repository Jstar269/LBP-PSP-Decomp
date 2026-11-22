/* LBPMATCH module=EBOOT.BIN start=0x0896c774 end=0x0896c85c symbol=FUN_0896c774 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Stack { unsigned *base; unsigned short idx; short pad; unsigned char flag; };
struct Out { char pad[76]; struct Stack st; };
struct Self { char pad[104]; unsigned color; };
struct Tmp { unsigned a; unsigned b; unsigned r; };
extern void FUN_0896c688(struct Self *self, struct Out *o);

static inline short count(struct Stack *s)
{
  return s->idx + 1;
}

void FUN_0896c774(struct Self *self, struct Out *o)
{
  unsigned c = self->color;
  struct Stack *st = &o->st;
  if (count(st) != 0) {
    struct Tmp t;
    unsigned char *p = (unsigned char *)&t;
    int i;
    t.a = self->color;
    t.b = st->base[(short)st->idx];
    t.r = 0;
    i = 0;
    do {
      i = i + 1;
      p[8] = ((int)p[0] * p[4]) / 255;
      p = p + 1;
    } while (i != 4);
    c = t.r;
  }
  {
    short n = count(st);
    st->flag = 1;
    st->base[n] = c;
    st->idx = n;
  }
  FUN_0896c688(self, o);
  st->flag = 1;
  st->idx = st->idx - 1;
}
// NEAR: 17 words differ (58/58). Structure matches (count() = (short)(idx + 1) with an unsigned short index, the colour modulate loop as a do/while with a separate counter, signed /255 via mult 0x80808081).
// Remaining: scheduling of the stack-temporary init stores relative to the base[idx] address, and v0/v1 register roles in the push (base[n] = c; idx = n).
// Tried: for vs do/while (for counts down), t.a/t.b order, unsigned vs signed division.
// near miss from lane api-lbp-b-20261121 (unreviewed)
