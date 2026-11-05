/* LBPMATCH module=EBOOT.BIN start=0x0889b768 end=0x0889b7b8 symbol=FUN_0889b768 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_0889b734(void *p);
extern void *DAT_08bee250;
extern "C" void *FUN_0889b768(void)
{
  if (DAT_08bee250 == 0) {
    void *n = FUN_08a9e838(24);
    FUN_0889b734(n);
    DAT_08bee250 = n;
  }
  return DAT_08bee250;
}
