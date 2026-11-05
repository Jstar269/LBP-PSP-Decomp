/* LBPMATCH module=EBOOT.BIN start=0x0886c3bc end=0x0886c414 symbol=FUN_0886c3bc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0887c328(void *g, void *s);
extern void *DAT_08bedba4;
struct Sub { int a, state; };
struct Obj { int pad[50]; char flag; char pad2[3]; Sub sub; /* +204 */ };
extern "C" int FUN_0886c3bc(Obj *o)
{
  Sub *s = &o->sub;
  FUN_0887c328(DAT_08bedba4, s);
  if (s->state == 6) o->flag = 1;
  return 1;
}
