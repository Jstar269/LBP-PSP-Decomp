/* LBPMATCH module=EBOOT.BIN start=0x0885d290 end=0x0885d2f0 symbol=FUN_0885d290 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0887e610(void *self, int n);
extern void *DAT_08baec60[];
struct Obj { void **vt; int pad[48]; int f196, f200; char b204, b205; };
extern "C" void FUN_0885d290(Obj *o, int a, int b, int n)
{
  FUN_0887e610(o, n);
  o->f196 = b;
  o->f200 = a;
  o->b204 = 1;
  o->b205 = 1;
  o->vt = DAT_08baec60;
}
