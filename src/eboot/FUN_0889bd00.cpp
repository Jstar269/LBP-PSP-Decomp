/* LBPMATCH module=EBOOT.BIN start=0x0889bd00 end=0x0889bd54 symbol=FUN_0889bd00 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_0889d194(void *o);
extern "C" void FUN_0889d024(void *p);
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void cb(void *x);
  int cnt;
};
extern "C" void FUN_0889bd00(Obj *o, void *x)
{
  o->cnt++;
  FUN_0889d024(FUN_0889d194(o));
  o->cb(x);
}
