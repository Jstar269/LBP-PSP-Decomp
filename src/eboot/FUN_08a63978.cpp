/* LBPMATCH module=EBOOT.BIN start=0x08a63978 end=0x08a639d8 symbol=FUN_08a63978 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a62340(void *p, int k);
extern "C" void FUN_08a636d0(void *p, void *a, void *b);
extern "C" void *FUN_08a63978(void *a, void *b)
{
  void *m = FUN_08a9e838(576);
  FUN_08a62340(m, 22);
  FUN_08a636d0(m, a, b);
  return m;
}
