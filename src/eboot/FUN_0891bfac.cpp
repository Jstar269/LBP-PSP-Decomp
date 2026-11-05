/* LBPMATCH module=EBOOT.BIN start=0x0891bfac end=0x0891c00c symbol=FUN_0891bfac lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct P { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual int a(); virtual void v5(); virtual int b(); };
struct Obj { int pad[42]; P *p; int f172; };
extern "C" int FUN_0891bfac(Obj *o)
{
  int r = o->p->a();
  o->f172 = o->p->b() + 16;
  return r;
}
