/* LBPMATCH module=EBOOT.BIN start=0x0892f64c end=0x0892f6c0 symbol=FUN_0892f64c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_0892e8e0(void *s);
extern "C" void FUN_0892f3bc(void *s);
struct S { int pad[49]; int st; int pad2[1]; char lock[1]; };
extern S *DAT_08bedb18;
extern "C" void FUN_0892f64c()
{
  S *s = DAT_08bedb18;
  FUN_08ad3088(s->lock);
  switch (s->st) {
  case 0: FUN_0892e8e0(s); break;
  case 1: FUN_0892f3bc(s); break;
  }
  FUN_08ad2810(s->lock);
}
