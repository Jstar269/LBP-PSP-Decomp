/* LBPMATCH module=EBOOT.BIN start=0x08868a18 end=0x08868a88 symbol=FUN_08868a18 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08879430(void *o, void *a, void (*fn)(), int n);
extern "C" void FUN_0886829c();
extern "C" void FUN_088686bc(void *p);
extern "C" void FUN_08845538(void *p);
extern void *DAT_08bb0068[], *DAT_08bb00a0[];
struct Obj { void **vt; int pad[2]; void **vt12; int pad1[46]; char s200[232]; int f432; char s436[232]; char s668[72]; int f740; char b744, p[3]; int f748; };
extern "C" void FUN_08868a18(Obj *o, void *a)
{
  FUN_08879430(o, a, FUN_0886829c, 3);
  o->vt = DAT_08bb0068;
  o->vt12 = DAT_08bb00a0;
  FUN_088686bc(o->s200);
  FUN_088686bc(o->s436);
  FUN_08845538(o->s668);
  o->f740 = 0;
  o->f748 = 0;
  o->b744 = 0;
  o->f432 = 0;
}
