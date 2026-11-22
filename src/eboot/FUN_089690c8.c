/* LBPMATCH module=EBOOT.BIN start=0x089690c8 end=0x0896918c symbol=FUN_089690c8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Cnt { int pad; unsigned short n; short pad1; unsigned char flag; char pad2[3]; };
struct Wr { char *base; unsigned *cur; unsigned cap; };
struct Self {
  char pad0[5 - 0];
  unsigned char f5;
  char pad1[44 - 6];
  struct Wr *w;
  char pad2[64 - 48];
  struct Cnt c64;
  struct Cnt c76;
  struct Cnt c88;
  struct Cnt c100;
};
extern char DAT_08bac190[];
extern void FUN_089f7f74(char *s, int a, int b);
extern void FUN_08baa28c(void);

static inline void drop(struct Cnt *c)
{
  c->flag = 1;
  c->n = c->n - 1;
}

void FUN_089690c8(struct Self *self)
{
  struct Wr *w;
  unsigned *p;
  unsigned used = 0;
  if (self->f5 != 0) {
    drop(&self->c88);
    drop(&self->c76);
    drop(&self->c64);
    drop(&self->c100);
  }
  w = self->w;
  p = w->cur;
  *p = 0x0b000000;
  p = p + 1;
  w->cur = p;
  if (p != 0)
    used = (char *)p - w->base;
  if (w->cap < used)
    FUN_089f7f74(DAT_08bac190, 0xffff, 1);
  FUN_08baa28c();
}
