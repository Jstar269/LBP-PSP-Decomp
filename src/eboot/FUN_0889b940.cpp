/* LBPMATCH module=EBOOT.BIN start=0x0889b940 end=0x0889b9a8 symbol=FUN_0889b940 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_0889b734(void *m);
extern "C" void FUN_0889ab98(void *a);
struct Obj { int f0, f4; };
extern Obj *DAT_08bee250;
extern "C" void FUN_0889b940(void *a)
{
  if (!DAT_08bee250) {
    Obj *m = (Obj *)FUN_08a9e838(24);
    FUN_0889b734(m);
    DAT_08bee250 = m;
  }
  DAT_08bee250->f4 = 3;
  FUN_0889ab98(a);
}
