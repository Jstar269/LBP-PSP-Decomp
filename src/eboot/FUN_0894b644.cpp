/* LBPMATCH module=EBOOT.BIN start=0x0894b644 end=0x0894b6b8 symbol=FUN_0894b644 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bb77b8[], *DAT_08bb7778[];
struct E { int a, b; };
struct Vec { E *begin, *end; };
struct Obj { void **vt; int pad; void *f8; int pad2[2]; Vec vec; };
extern "C" void FUN_0894b644(Obj *o)
{
  Vec *v = &o->vec;
  o->vt = DAT_08bb77b8;
  E *p = o->vec.begin;
  for (; p != v->end; p++) ;
  if (v->begin) FUN_08a9e0cc(v->begin);
  if (o->f8) FUN_08a9e0cc(o->f8);
  o->vt = DAT_08bb7778;
}
