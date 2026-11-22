/* LBPMATCH module=EBOOT.BIN start=0x0899f584 end=0x0899f628 symbol=FUN_0899f584 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Elem { char data[48]; };
struct P { int a; int b; float c; };
struct Src { struct P p; int pad; struct Elem e[3]; };
struct Dst {
  void *vt;
  short s0;
  short s1;
  int v;
  int pad;
  struct P p;
  int pad2;
  struct Elem e[3];
};

static inline void setp(struct P *d, const struct P *s)
{
  d->a = s->a;
  d->b = s->b;
  d->c = s->c;
}

extern void *DAT_08bc0528[];
extern void FUN_0898c0e8(struct Elem *d, const struct Elem *s);

void FUN_0899f584(struct Dst *self, int v, const struct Src *src)
{
  int i;
  struct Elem *d;
  const struct Elem *s;

  self->v = v;
  self->s0 = -1;
  self->s1 = 1;
  self->vt = DAT_08bc0528;
  setp(&self->p, &src->p);
  d = self->e;
  s = src->e;
  i = 3;
  do {
    FUN_0898c0e8(d, s);
    i--;
    d++;
    s++;
  } while (i != 0);
  self->s0 = 176;
}
