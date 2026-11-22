/* LBPMATCH module=EBOOT.BIN start=0x089e67b8 end=0x089e68b4 symbol=FUN_089e67b8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { int a; int state; int pad[6]; };
struct Glob { char pad[168]; struct Rec r[4]; };
struct Pair { void *p; int v; };
extern struct Glob *DAT_08be8600;
extern void *DAT_08bdbcf4[];
extern int FUN_0898dd88(void *self, int idx);
extern void FUN_0898dd7c(void *self);
extern int FUN_08a9b8a4(struct Pair *out, void *p);
extern void FUN_0898dc98(struct Pair *pr, void (*cb)(void *, int), int idx);
extern void FUN_089e67b8(void *self, int idx);

void FUN_089e67b8(void *self, int idx)
{
  struct Glob *g = DAT_08be8600;
  int i;
  if (g->r[idx].state == 1) {
    int v = FUN_0898dd88(self, idx);
    g->r[idx].state = v != 0 ? 0 : 4;
    g->r[idx].a = v;
  } else if (g->r[idx].state == 3) {
    FUN_0898dd7c(self);
    g->r[idx].state = 4;
  }
  for (i = 1; i != 4; i++) {
    if (g->r[i].state == 2) {
      struct Pair pr;
      void *p = DAT_08bdbcf4[i];
      g->r[i].state = 1;
      int v = FUN_08a9b8a4(&pr, p);
      pr.p = p;
      pr.v = v;
      FUN_0898dc98(&pr, FUN_089e67b8, i);
      return;
    }
  }
}
