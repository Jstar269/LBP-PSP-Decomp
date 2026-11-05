/* LBPMATCH module=EBOOT.BIN start=0x088fb3e8 end=0x088fb45c symbol=FUN_088fb3e8 lang=c++ rodata=0x08baf1c0 */
/* Hand-written reconstruction from the disassembly (lane lbp6b; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
struct Globals { char pad[0x7fc]; int v; };
struct Child {
  int field_4, field_8, field_c;
  virtual void m0(); virtual void m1(); virtual void m2();
  virtual void m3();
  virtual void m4(); virtual void m5();
  virtual void m6(float f);
};
struct Obj {
  char pad[0x24];
  Child *child;
  char pad2[0x70 - 0x28];
  char flag;
};
extern "C" Globals *DAT_08bf7304;
extern "C" int FUN_08a50fbc();
extern "C" void FUN_088f95a4(float f, int a, int b);
extern "C" void FUN_088fb3e8(Obj *o)
{
  FUN_08a50fbc();
  Child *c = o->child;
  c->field_c = DAT_08bf7304->v;
  c->m3();
  o->child->m6(0.0f);
  o->flag = 1;
  FUN_088f95a4(0.3f, (int)o + 0x104, 0);
}
