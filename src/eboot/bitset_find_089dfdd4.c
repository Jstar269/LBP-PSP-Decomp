/* LBPMATCH module=EBOOT.BIN start=0x089dfdd4 end=0x089dfec0 symbol=FUN_089dfdd4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Link { int a, b, c; };
struct S {
  int pad0[6];
  struct Link m24;
  struct Link *f36;
  int pad40[2];
  int f48;
};
struct P { int pad0; unsigned flags; };
extern int DAT_08bdbd18[];
extern int FUN_0896e618(struct P *p);
extern void FUN_089dea4c(struct S *t, struct P *p, struct Link **out, unsigned *idx);

static inline int find(const struct P *p, unsigned *idx, const struct S *t)
{
  unsigned i = 1;
  const int *tab = DAT_08bdbd18;
  for (;;) {
    unsigned bit = 1u << i;
    if (p->flags & bit) {
      if (*tab == t->f48) {
        *idx = i;
        return 1;
      }
      return 0;
    }
    i++;
    tab++;
    if (i == 5)
      return 0;
  }
}

void FUN_089dfdd4(struct S *t, struct P *p, struct Link **out, unsigned *idx)
{
  if (FUN_0896e618(p) && find(p, idx, t) && t->f36 != &t->m24)
    *out = t->f36;
  if (*out == &t->m24)
    FUN_089dea4c(t, p, out, idx);
}
