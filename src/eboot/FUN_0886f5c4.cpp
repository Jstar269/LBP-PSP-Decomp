/* LBPMATCH module=EBOOT.BIN start=0x0886f5c4 end=0x0886f630 symbol=FUN_0886f5c4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08957550(void *m);
struct B { virtual void m0(); virtual void m1(); virtual void m2(int a); virtual void m3(); virtual void m4(); virtual void m5(); virtual void m6(); virtual void m7(); virtual void m8(); };
struct Owner { int pad[35]; B *f140; };
extern "C" void FUN_0886f5c4(void *unused, int a, Owner *o)
{
  B *m = (B *)FUN_08a9e838(16);
  FUN_08957550(m);
  o->f140 = m;
  m->m2(a);
  o->f140->m8();
}
