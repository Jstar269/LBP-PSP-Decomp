/* LBPMATCH module=EBOOT.BIN start=0x0899950c end=0x089995c4 symbol=FUN_0899950c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Ctl { int first; int b; int c; int d; int e; void *res; };
struct Ev { int a; unsigned flags; };

extern struct Ctl *DAT_08bf5e44;
extern int *DAT_08bf5df8;

extern void FUN_08a61fa4(void *a, int b);
extern void FUN_08a20de8(int *p);
extern void FUN_08a9e0cc(int *p);
extern void FUN_089757c0(int a);
extern unsigned char FUN_08999158(void *a, struct Ev *b);

int FUN_0899950c(void *self, struct Ev *ev)
{
  struct Ctl *c;
  int *p;
  int f;

  if ((ev->flags & 0x1000) != 0 || (ev->flags & 0x20000) != 0) {
    c = DAT_08bf5e44;
    if (c->first != 0) {
      if (c->res != 0) {
        FUN_08a61fa4(c->res, c->first);
        c->res = 0;
      }
      p = DAT_08bf5df8;
      if (p != 0) {
        FUN_08a20de8(p);
        FUN_08a9e0cc(p);
      }
      f = c->first;
      c->first = 0;
      DAT_08bf5df8 = (int *)f;
      FUN_089757c0(45);
      return 0;
    }
  }
  return FUN_08999158(self, ev);
}
// NEAR 2 words: retail issues `sw v0,DAT_08bf5df8` before the jal and fills its delay slot with `sw zero,0(s1)`; ours swaps the two.
// Ruled out: source order of the two stores, a local for the loaded value. The callee returns unsigned char (any int return gets
// sibcalled: retail's non-tail jal needs the QImode return to block the tail call).
// near miss from lane api-lbp-c-20261121 (unreviewed)
