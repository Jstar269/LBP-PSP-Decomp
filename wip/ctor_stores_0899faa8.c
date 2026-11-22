/* LBPMATCH module=EBOOT.BIN start=0x0899faa8 end=0x0899fb64 symbol=FUN_0899faa8 lang=c99 */
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

void FUN_0899faa8(struct S *self, int a, int e)
{
  struct N *h;
  struct B *b;
  struct C *c;

  FUN_0899fc08(self, a);
  h = &self->n;
  b = &self->b;
  c = &self->c;
  self->vt = DAT_08bc04d8;
  b->y = DAT_08bab59c;
  self->b.x = DAT_08baf9fc;
  self->c.x = DAT_08bab59c;
  h->l1 = h;
  b->fn = FUN_08966c20;
  b->flag = 0;
  c->fn = FUN_08966c20;
  self->i192 = -1;
  self->i196 = e;
  self->n.l3 = h;
  self->n.l0 = h;
  self->n.l2 = h;
  b->s0 = 0;
  b->s1 = 0;
  b->s2 = 0;
  self->i152 = 0;
  c->y = DAT_08bab59c;
  c->flag = 0;
  c->i0 = 0;
  c->i1 = 0;
  c->i2 = 0;
  FUN_0899ec58(self);
}
// NEAR 29 words (47/47): sibling of wip/ctor_stores_0899f9f4.c (same layout). Retail keeps the stores in source order across the
// sub-object base registers (a1 = this+112, a2 = this+128, a3 = this+164: stores through different base registers never swap);
// ours schedules them freely because GCC sees all three bases as this+const. Source order below is retail's asm order.
// near miss from lane api-lbp-c-20261121 (unreviewed)
