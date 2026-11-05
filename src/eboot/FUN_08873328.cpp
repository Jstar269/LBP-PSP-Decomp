/* LBPMATCH module=EBOOT.BIN start=0x08873328 end=0x08873394 symbol=FUN_08873328 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0886e018(void *o, int a);
extern "C" void FUN_08873034(void *p);
extern "C" void FUN_08873040(void *p, void *q);
extern void *DAT_08bb0518[];
struct Obj { void **vt; int pad[2]; int f12; int pad1[8]; int f48; char s52[1]; };
extern "C" void FUN_08873328(Obj *o, void *unused, void *b)
{
  FUN_0886e018(o, 5);
  o->vt = DAT_08bb0518;
  FUN_08873034(o->s52);
  o->f48 = 0;
  FUN_08873040(o->s52, b);
  o->f12 = 2;
}
