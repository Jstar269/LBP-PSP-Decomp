/* LBPMATCH module=EBOOT.BIN start=0x089d845c end=0x089d84bc symbol=FUN_089d845c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08970b6c(void *p);
extern "C" void FUN_089e0a0c(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc17a0[];
struct Obj { void **vt; int pad[11]; void *a; void *b; };
extern "C" void FUN_089d845c(Obj *o)
{
  o->vt = DAT_08bc17a0;
  if (o->b) FUN_08970b6c(o->b);
  if (o->a) FUN_08970b6c(o->a);
  FUN_089e0a0c(o);
  FUN_08a9e0cc(o);
}
