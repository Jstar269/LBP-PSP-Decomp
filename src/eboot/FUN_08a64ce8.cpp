/* LBPMATCH module=EBOOT.BIN start=0x08a64ce8 end=0x08a64d5c symbol=FUN_08a64ce8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a64be8(void *m, int k);
extern "C" void FUN_08a6aed4(void *m, void *src, void *a);
extern "C" void FUN_08a9c5b0(void *d, void *s);
struct Obj { int pad[147]; int f588; char sub[1]; };
extern "C" Obj *FUN_08a64ce8(Obj *src, void *a)
{
  Obj *m = (Obj *)FUN_08a9e838(624);
  FUN_08a64be8(m, 26);
  FUN_08a6aed4(m, src, a);
  m->f588 = src->f588;
  FUN_08a9c5b0(m->sub, src->sub);
  return m;
}
