/* LBPMATCH module=EBOOT.BIN start=0x08a68114 end=0x08a68174 symbol=FUN_08a68114 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a672c0(void *p, int k);
extern "C" void FUN_08a67e0c(void *p, void *a, void *b);
extern "C" void *FUN_08a68114(void *a, void *b)
{
  void *m = FUN_08a9e838(848);
  FUN_08a672c0(m, 28);
  FUN_08a67e0c(m, a, b);
  return m;
}
