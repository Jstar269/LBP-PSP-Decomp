/* LBPMATCH module=EBOOT.BIN start=0x088b352c end=0x088b35a8 symbol=FUN_088b352c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08804f3c(void *h);
extern "C" void FUN_08804f34() __attribute__((weak));
struct IFace { int pad[2]; unsigned char b8; char pad2[8]; unsigned char b17; };
struct H { IFace *p; H(const H &); };
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(); virtual void m6(); virtual void m7(); virtual void m8(); virtual void m9(); virtual void m10(); virtual void m11(); virtual void m12(); virtual void m13(); virtual H m14(); };
struct VI { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(float f); virtual void v8(); virtual void v9(); };
extern "C" void FUN_088b352c(Obj *o, float f)
{
  H h = o->m14();
  if (h.p && FUN_08804f34 && !h.p->b8 && !h.p->b17) ((VI *)h.p)->v7(f);
  FUN_08804f3c(&h);
}
