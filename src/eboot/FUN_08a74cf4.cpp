/* LBPMATCH module=EBOOT.BIN start=0x08a74cf4 end=0x08a74d54 symbol=FUN_08a74cf4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a74cb0(void *p, int k);
extern "C" void FUN_08a74690(void *p, void *a, void *b);
extern "C" void *FUN_08a74cf4(void *a, void *b)
{
  void *m = FUN_08a9e838(400);
  FUN_08a74cb0(m, 36);
  FUN_08a74690(m, a, b);
  return m;
}
