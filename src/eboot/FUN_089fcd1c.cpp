/* LBPMATCH module=EBOOT.BIN start=0x089fcd1c end=0x089fcd7c symbol=FUN_089fcd1c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_089fa9e0(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc2de8[];
struct Obj { void **vt; int pad[62]; void *x; void *y; };
extern "C" void FUN_089fcd1c(Obj *o)
{
  o->vt = DAT_08bc2de8;
  if (o->x) FUN_08a9e0c4(o->x);
  if (o->y) FUN_08a9e0c4(o->y);
  FUN_089fa9e0(o);
  FUN_08a9e0cc(o);
}
