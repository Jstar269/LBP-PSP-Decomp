/* LBPMATCH module=EBOOT.BIN start=0x08969f6c end=0x08969ffc symbol=FUN_08969f6c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { int a; int b; unsigned c; int d; };
struct Wr { char *base; unsigned *cur; unsigned cap; };
struct A { char pad[44]; struct Wr *w; };
struct B { char pad[8]; struct Rec recs[1]; };
struct Bv {
  struct Rec recs[1];
};
struct Bw {
  char pad0[8];
  int pad1[8];
  int idx;
  unsigned *p;
};
extern unsigned *DAT_08bf0380;
extern void FUN_08969dac(struct A *a);
extern void FUN_08968e00(struct A *a, struct Bv *b);

void FUN_08969f6c(struct A *a, struct Bv *b)
{
  unsigned v;
  unsigned *q;
  ((unsigned *)((char *)b + (((struct Bw *)b)->idx << 4)))[2] = *DAT_08bf0380;
  FUN_08969dac(a);
  v = *((struct Bw *)b)->p;
  q = a->w->cur;
  a->w->cur = q + 2;
  q[0] = (((v & 0x0fffffff) >> 24) << 16) | 0x10000000;
  q[1] = (v & 0x00ffffff) | 0x0a000000;
  FUN_08968e00(a, b);
}
// NEAR: 2 words differ (36/36): the two command-word stores come out `sw a2,4(a3); sw v1,0(a3)`, retail `sw v1,0(a3); sw a2,4(a3)` (the ext/sll/or word first).
// Tried: both store orders, word locals, a Cmd struct for the cursor; the (char*)b + (idx << 4) address form fixed an addu operand order.
// near miss from lane api-lbp-b-20261121 (unreviewed)
