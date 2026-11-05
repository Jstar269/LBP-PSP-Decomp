/* LBPMATCH module=EBOOT.BIN start=0x08a82750 end=0x08a827b0 symbol=FUN_08a82750 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a35244(void *p, void *q);
extern "C" void FUN_08a352f8(void *o, void *a, void *b, void *c);
struct Obj { int pad[203]; char sub[1]; };
extern "C" void FUN_08a82750(Obj *o, void *a, void *b, void *c)
{
  FUN_08a35244(o->sub, b);
  FUN_08a352f8(o, a, b, c);
}
