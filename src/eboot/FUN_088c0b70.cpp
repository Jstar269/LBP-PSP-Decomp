/* LBPMATCH module=EBOOT.BIN start=0x088c0b70 end=0x088c0be4 symbol=FUN_088c0b70 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089f1a64(void *p);
extern "C" void FUN_089f19c0(void *p);
struct Ctx { struct In *in; };
struct In { int pad[15]; void *f60; };
struct T { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void m13(void *a, void *b, int k); };
extern "C" void FUN_088c0b70(void *a, void *b, Ctx *c, T *t)
{
  FUN_089f1a64(c->in->f60);
  t->m13(a, b, 1);
  FUN_089f19c0(c->in->f60);
}
