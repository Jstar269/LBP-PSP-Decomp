/* LBPMATCH module=EBOOT.BIN start=0x08a50ce0 end=0x08a50d54 symbol=FUN_08a50ce0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a8585c(void *m, int a, int b, int c, int d);
extern "C" void FUN_08a50ce0(int *p, int x, void **out, unsigned char flag)
{
  void *m = FUN_08a9e838(832);
  FUN_08a8585c(m, *p, x, flag, 43);
  *out = m;
}
