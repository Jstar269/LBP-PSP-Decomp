/* LBPMATCH module=EBOOT.BIN start=0x0889cfac end=0x0889d024 symbol=FUN_0889cfac lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Item { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void go(void *a); };
struct Obj { int hdr; Item *items[50]; int count; };
extern "C" void FUN_0889cfac(Obj *o, void *arg)
{
  for (int i = 0; i < o->count; i++) o->items[i]->go(arg);
}
