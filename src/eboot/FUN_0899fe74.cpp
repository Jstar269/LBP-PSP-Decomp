/* LBPMATCH module=EBOOT.BIN start=0x0899fe74 end=0x0899fed8 symbol=FUN_0899fe74 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08967488(void *p);
extern void *DAT_08be85f8;
struct Item {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
  virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
  virtual void stop();
  unsigned flags;
};
struct Obj { int pad[12]; Item *item; };
extern "C" void FUN_0899fe74(Obj *o)
{
  Item *it = o->item;
  if (it) {
    it->stop();
    it->flags &= ~0x200u;
  }
  if (DAT_08be85f8) FUN_08967488(DAT_08be85f8);
}
