/* LBPMATCH module=EBOOT.BIN start=0x088c0ccc end=0x088c0d44 symbol=FUN_088c0ccc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089f1a64(void *p);
extern "C" void FUN_089f19c0(void *p);
struct Ctx { struct In *in; };
struct In { int pad[15]; void *f60; };
struct Hdr { int pad[3]; int f12; };
struct Src { Hdr *h; };
struct T { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void m13(Src *a, void *b, int c); };
extern "C" void FUN_088c0ccc(Src *a, void *b, Ctx *c, T *t)
{
  FUN_089f1a64(c->in->f60);
  t->m13(a, b, a->h->f12);
  FUN_089f19c0(c->in->f60);
}
