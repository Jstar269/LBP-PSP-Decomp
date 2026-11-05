/* LBPMATCH module=EBOOT.BIN start=0x08944ac8 end=0x08944b2c symbol=FUN_08944ac8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern void *DAT_08bb7508[], *DAT_08bb7440[], *DAT_08bb7454[];
struct Obj { void **vt; int f4; void **f8; int f12, f16; char f20; };
extern "C" void FUN_08944ac8(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08bb7508;
  o->f4 = 1;
  FUN_0894a258(&o->f8);
  o->vt = DAT_08bb7440;
  o->f8 = DAT_08bb7454;
  o->f12 = 0;
  o->f16 = 0;
  o->f20 = 0;
}
