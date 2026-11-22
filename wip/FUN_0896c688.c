/* LBPMATCH module=EBOOT.BIN start=0x0896c688 end=0x0896c774 symbol=FUN_0896c688 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Cnt { int pad; unsigned short n; short pad1; unsigned char flag; char pad2[3]; };
struct Self {
  int pad0;
  unsigned flags;
  int pad1;
  short x;
  short y;
  int pad2[2];
  char g24[24];
  char g48[24];
  char pad3[28];
  short x2;
  short y2;
};
struct Out {
  char pad[64];
  struct Cnt c64;
  struct Cnt c76;
  struct Cnt c88;
};
extern void FUN_089692b0(struct Out *o, short *p);
extern void FUN_0896c1b0(struct Out *o, short x, short y);
extern void FUN_0896c348(void *g, struct Out *o);

static inline void drop(struct Cnt *c)
{
  c->flag = 1;
  c->n = c->n - 1;
}

void FUN_0896c688(struct Self *self, struct Out *o)
{
  int has;
  unsigned f;
  if ((self->flags & 0x400) != 0)
    FUN_089692b0(o, &self->x);
  FUN_0896c1b0(o, self->x, self->y);
  has = *(int *)((char *)self + 100) != 0;
  if (has)
    FUN_0896c1b0(o, self->x2, self->y2);
  FUN_0896c348(&self->g24, o);
  if (has)
    drop(&o->c64);
  FUN_0896c348(&self->g48, o);
  {
    struct Cnt *c = &o->c64;
    unsigned short nn = c->n - 1;
    f = self->flags;
    c->n = nn;
    c->flag = 1;
  }
  if ((f & 0x400) != 0)
    drop(&o->c88);
}
// NEAR: 4 words differ (59/59): in the second counter drop retail issues `lhu v1,4(v0)` before `lw a0,4(s0)` (flags) and `addiu v1,v1,-1` before `andi a0,a0,0x400`; ours loads the flags first.
// Tried: drop() helper (flag, then n) for every drop (10 words), explicit n-then-flag with the flags read before/after, a temp for the decremented count.
// near miss from lane api-lbp-b-20261121 (unreviewed)
