/* LBPMATCH module=EBOOT.BIN start=0x088455d4 end=0x0884562c symbol=FUN_088455d4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08845538(void *p);
extern "C" void FUN_0884436c(void *self, void *n);
extern "C" void *FUN_088455d4(void *self)
{
  void *n = FUN_08a9e838(72);
  FUN_08845538(n);
  void *r = 0;
  if (n) {
    FUN_0884436c(self, n);
    r = n;
  }
  return r;
}
