/* LBPMATCH module=EBOOT.BIN start=0x0886b250 end=0x0886b2c0 symbol=FUN_0886b250 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08879430(void *o, void *a, void (*fn)(), int n);
extern "C" void FUN_08869b5c();
extern "C" void FUN_0885eac0(void *p);
extern "C" void FUN_0885d290(void *p, int a, int b, int c);
extern void *DAT_08bb01a8[], *DAT_08bb01e0[], *DAT_08bb0120[];
struct Obj { void **vt; int pad[2]; void **vt12; int pad1[46]; char s200[8]; void **vt208; };
extern "C" void FUN_0886b250(Obj *o, void *a)
{
  FUN_08879430(o, a, FUN_08869b5c, 3);
  o->vt = DAT_08bb01a8;
  o->vt12 = DAT_08bb01e0;
  FUN_0885eac0(o->s200);
  FUN_0885d290(&o->vt208, 0, 0, 6);
  o->vt208 = DAT_08bb0120;
}
