/* LBPMATCH module=EBOOT.BIN start=0x0899f9f4 end=0x0899faa8 symbol=FUN_0899f9f4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct B {
  float x;
  float y;
  unsigned char flag;
  char pad;
  short s0;
  short s1;
  short s2;
  void (*fn)(void);
};

struct C {
  float x;
  float y;
  unsigned char flag;
  char pad[3];
  int i0;
  int i1;
  int i2;
  void (*fn)(void);
};

struct N { struct N *l0; struct N *l1; struct N *l2; struct N *l3; };

struct S {
  void *vt;
  char pad0[108];
  struct N n;
  struct B b;
  int pad148;
  int i152;
  char pad156[8];
  struct C c;
  int i192;
  int i196;
  unsigned char b200;
};

extern const float DAT_08bab59c;
extern const float DAT_08baf9fc;
extern void *DAT_08bc04d8[];
extern void FUN_08966c20(void);
extern void FUN_0899fc08(struct S *self, int a);
extern void FUN_0899ec58(struct S *self);

void FUN_0899f9f4(struct S *self, int a)
{
  struct N *h;
  struct B *b;
  struct C *c;

  FUN_0899fc08(self, a);
  h = &self->n;
  b = &self->b;
  c = &self->c;
  b->y = DAT_08bab59c;
  self->b.x = DAT_08baf9fc;
  self->c.x = DAT_08bab59c;
  h->l1 = h;
  b->flag = 0;
  b->fn = FUN_08966c20;
  c->flag = 0;
  c->fn = FUN_08966c20;
  self->vt = DAT_08bc04d8;
  self->i196 = -1;
  self->n.l3 = h;
  self->n.l0 = h;
  self->n.l2 = h;
  b->s0 = 0;
  b->s1 = 0;
  b->s2 = 0;
  self->i152 = 0;
  c->y = DAT_08bab59c;
  c->i0 = 0;
  c->i1 = 0;
  c->i2 = 0;
  self->i192 = -1;
  self->b200 = 0;
  FUN_0899ec58(self);
}
// NEAR 33 words (45/45): same stores and sub-object base registers (a1=&n, a2=&b, a3=&c), only the scheduler's store order differs.
// Retail keeps the stores in the order of the asm (b.y, b.x, c.x, h->prev = h (sw a1,4(a1)), b.flag, b.fn, c.flag, c.fn, vt, ...),
// ours moves them freely; float constants are global loads (lui/lwc1), 1.0 in $f1 first. Tried -O2 (34 words), flat self-> stores (35).
// near miss from lane api-lbp-c-20261121 (unreviewed)
