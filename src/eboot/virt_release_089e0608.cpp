/* LBPMATCH module=EBOOT.BIN start=0x089e0608 end=0x089e0698 symbol=FUN_089e0608 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Child {
  unsigned flags;
  virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3();
  virtual void m4(); virtual void m5(); virtual void m6(); virtual void m7();
  virtual void m8(); virtual void m9(); virtual void m10();
};
struct Obj {
  virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3();
  virtual void m4(); virtual void m5(); virtual void m6(); virtual void m7();
  virtual void m8(); virtual void m9(); virtual void m10(); virtual void m11();
  virtual void m12(); virtual void m13(); virtual void m14(); virtual void m15();
  int pad8[2];
  Child *b;
  Child *a;
};
extern "C" void FUN_089e0608(Obj *o)
{
  o->m15();
  Child *a = o->a;
  if (a) {
    a->m10();
    a->flags &= ~0x200u;
  }
  Child *b = o->b;
  if (b) {
    b->flags |= 0x200;
    b->m9();
  }
}
