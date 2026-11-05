/* LBPMATCH module=EBOOT.BIN start=0x08a8a7a8 end=0x08a8a808 symbol=FUN_08a8a7a8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a89e98(void *p, int k);
extern "C" void FUN_08a8a61c(void *p, void *a, void *b);
extern "C" void *FUN_08a8a7a8(void *a, void *b)
{
  void *m = FUN_08a9e838(352);
  FUN_08a89e98(m, 48);
  FUN_08a8a61c(m, a, b);
  return m;
}
