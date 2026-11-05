/* LBPMATCH module=EBOOT.BIN start=0x08954d20 end=0x08954d80 symbol=FUN_08954d20 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct P { virtual void v0(); virtual void v1(); virtual int a(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual int b(); };
struct Obj { int pad[42]; P *p; int f172; };
extern "C" int FUN_08954d20(Obj *o)
{
  int r = o->p->a();
  o->f172 = o->p->b() + 20;
  return r;
}
