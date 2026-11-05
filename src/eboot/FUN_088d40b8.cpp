/* LBPMATCH module=EBOOT.BIN start=0x088d40b8 end=0x088d411c symbol=FUN_088d40b8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Sys { int pad[113]; void *f452; };
extern Sys *DAT_08beed14;
extern "C" int FUN_088cd8c0(void *a, void *b);
struct Obj { unsigned flags; int pad[9]; int state; /* +40 */ };
extern "C" void FUN_088d40b8(Obj *o)
{
  o->flags &= ~0x10u;
  if (o->state != 4 && o->state != 0 && FUN_088cd8c0(DAT_08beed14->f452, o))
    o->flags |= 0x10;
}
