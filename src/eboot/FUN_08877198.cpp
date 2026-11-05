/* LBPMATCH module=EBOOT.BIN start=0x08877198 end=0x088771e8 symbol=FUN_08877198 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void *FUN_08a9e824(int n);
struct Obj { int pad[15]; void *buf; int size; };
extern "C" void FUN_08877198(void *unused, int n, Obj *o)
{
  if (o->buf) FUN_08a9e0c4(o->buf);
  o->size = n;
  o->buf = FUN_08a9e824(n);
}
