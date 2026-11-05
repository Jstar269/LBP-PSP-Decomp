/* LBPMATCH module=EBOOT.BIN start=0x088b36c0 end=0x088b3728 symbol=FUN_088b36c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern "C" void FUN_0883bd5c(void *p, int a);
extern void *DAT_08bb3320[], *DAT_08bb32f8[], *DAT_08bb330c[];
struct Obj { void **vt; int f4; void **vt8; char s12[8]; int f20; };
extern "C" void FUN_088b36c0(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08bb3320;
  o->f4 = 1;
  FUN_0894a258(&o->vt8);
  o->vt = DAT_08bb32f8;
  o->vt8 = DAT_08bb330c;
  FUN_0883bd5c(o->s12, 0);
  o->f20 = 0;
}
