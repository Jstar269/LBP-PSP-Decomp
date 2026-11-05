/* LBPMATCH module=EBOOT.BIN start=0x0887f128 end=0x0887f178 symbol=FUN_0887f128 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void go(); };
struct Item { int pad[3]; Sub sub; };
struct Slot { Item *p; int pad[12]; };
struct Obj { int pad[358]; Slot slots[1]; };
extern "C" int FUN_0887f128(Obj *o, int i)
{
  Item *it = o->slots[i].p;
  if (!it) return 0;
  Sub *sp = &it->sub;
  sp->go();
  return 1;
}
