/* LBPMATCH module=EBOOT.BIN start=0x08944a0c end=0x08944a6c symbol=FUN_08944a0c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern void *DAT_08bb74e0[], *DAT_08bb7468[], *DAT_08bb747c[];
struct Obj { void **vt; int f4; void **f8; int f12, f16; };
extern "C" void FUN_08944a0c(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08bb74e0;
  o->f4 = 1;
  FUN_0894a258(&o->f8);
  o->vt = DAT_08bb7468;
  o->f8 = DAT_08bb747c;
  o->f12 = 0;
  o->f16 = 0;
}
