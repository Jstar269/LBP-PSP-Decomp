/* LBPMATCH module=EBOOT.BIN start=0x08863590 end=0x088635e0 symbol=FUN_08863590 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_0885c9f8(void *p);
extern void *DAT_08bafc28[], *DAT_08baec60[];
struct Obj { void **vt; int pad[51]; void *res; };
extern "C" void FUN_08863590(Obj *o)
{
  o->vt = DAT_08bafc28;
  if (o->res) FUN_08a9e0c4(o->res);
  o->vt = DAT_08baec60;
  FUN_0885c9f8(o);
}
