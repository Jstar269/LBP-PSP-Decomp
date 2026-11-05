/* LBPMATCH module=EBOOT.BIN start=0x089a00ec end=0x089a0160 symbol=FUN_089a00ec lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_0899f9f4(void *m, void *a);
extern "C" void FUN_0899e9a8(void *m, int a, int b, int c, int d);
extern "C" void *FUN_089a00ec(void *a)
{
  void *m = FUN_08a9e838(204);
  FUN_0899f9f4(m, a);
  FUN_0899e9a8(m, 152, 10, 554, 0);
  FUN_0899e9a8(m, 151, 11, 555, 0);
  return m;
}
