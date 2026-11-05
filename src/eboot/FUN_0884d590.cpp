/* LBPMATCH module=EBOOT.BIN start=0x0884d590 end=0x0884d5f4 symbol=FUN_0884d590 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern "C" void FUN_0884d3bc(void *p);
extern "C" void FUN_08884d94(void *p);
extern void *DAT_08baee60[], *DAT_08baee38[], *DAT_08baee4c[];
struct Obj { void **vt; int f4; void **f8; char sub[1]; };
extern "C" void FUN_0884d590(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08baee60;
  o->f4 = 1;
  FUN_0894a258(&o->f8);
  o->vt = DAT_08baee38;
  o->f8 = DAT_08baee4c;
  FUN_0884d3bc(o->sub);
  FUN_08884d94(o);
}
