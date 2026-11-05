/* LBPMATCH module=EBOOT.BIN start=0x0885c984 end=0x0885c9f8 symbol=FUN_0885c984 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0887e610(void *o, int n);
extern void *DAT_08baf6c8[];
struct Obj { void **vt; int pad[48]; int f196, f200; char b204, b205; };
extern "C" void FUN_0885c984(Obj *o, int a, int b, int n, bool x, bool y)
{
  FUN_0887e610(o, n);
  o->vt = DAT_08baf6c8;
  o->f196 = b;
  o->f200 = a;
  o->b204 = x;
  o->b205 = y;
}
