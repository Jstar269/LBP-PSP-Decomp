/* LBPMATCH module=EBOOT.BIN start=0x08a8c904 end=0x08a8c964 symbol=FUN_08a8c904 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a8c574(void *p, int k);
extern "C" void FUN_08a8c808(void *p, void *a, void *b);
extern "C" void *FUN_08a8c904(void *a, void *b)
{
  void *m = FUN_08a9e838(816);
  FUN_08a8c574(m, 51);
  FUN_08a8c808(m, a, b);
  return m;
}
