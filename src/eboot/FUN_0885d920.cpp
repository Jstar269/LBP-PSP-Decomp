/* LBPMATCH module=EBOOT.BIN start=0x0885d920 end=0x0885d978 symbol=FUN_0885d920 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0887c328(void *g, void *s);
extern "C" void FUN_0885d72c(void *o);
extern void *DAT_08bedba4;
struct Sub { int a, state; };
struct Obj { int pad[59]; Sub sub; /* +236 */ };
extern "C" int FUN_0885d920(Obj *o)
{
  Sub *s = &o->sub;
  FUN_0887c328(DAT_08bedba4, s);
  if (s->state == 6) FUN_0885d72c(o);
  return 1;
}
