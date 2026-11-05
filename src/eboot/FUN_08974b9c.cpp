/* LBPMATCH module=EBOOT.BIN start=0x08974b9c end=0x08974bfc symbol=FUN_08974b9c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char *FUN_08a9e824(int n);
extern "C" void FUN_08b9eb88(void *p, int v, int n);
extern void *DAT_08bbd2a0[];
struct Obj { void **vt; int f4, f8; char *buf; int cap; };
extern "C" void FUN_08974b9c(Obj *o, int n)
{
  int bytes = n << 2;
  o->vt = DAT_08bbd2a0;
  o->cap = n;
  o->f4 = 0;
  o->f8 = 0;
  o->buf = 0;
  char *m = FUN_08a9e824(bytes);
  o->buf = m;
  FUN_08b9eb88(m, 0, bytes);
}
