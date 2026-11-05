/* LBPMATCH module=EBOOT.BIN start=0x0899f628 end=0x0899f68c symbol=FUN_0899f628 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_0899f584(void *p, void *a, void *b);
extern "C" void **FUN_0899f628(void **out, void *a, void *b)
{
  void *n = FUN_08a9e838(176);
  FUN_0899f584(n, a, b);
  *out = n;
  return out;
}
