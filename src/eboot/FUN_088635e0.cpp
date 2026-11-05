/* LBPMATCH module=EBOOT.BIN start=0x088635e0 end=0x08863640 symbol=FUN_088635e0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08879430(void *self, void *a, void (*fn)(), int n);
extern "C" void FUN_088630a4();
extern "C" void FUN_08863410(void *p, void *q);
extern void *DAT_08bafc58[], *DAT_08bafc90[];
struct Obj { void **vt; int pad[2]; void **vt2; int pad2[46]; int f200; char sub[1]; };
extern "C" void FUN_088635e0(Obj *o, void *a, void *q)
{
  FUN_08879430(o, a, FUN_088630a4, 3);
  o->vt = DAT_08bafc58;
  o->vt2 = DAT_08bafc90;
  o->f200 = 0;
  FUN_08863410(o->sub, q);
}
