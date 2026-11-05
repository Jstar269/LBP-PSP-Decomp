/* LBPMATCH module=EBOOT.BIN start=0x088f7e70 end=0x088f7e88 symbol=FUN_088f7e70 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bb4ef8[];
struct P { void *vt; int f4, f8; };
extern "C" void FUN_08a9e0cc(struct P *o);
extern "C" void FUN_088f7e70(struct P *o)
{
  o->vt = DAT_08bb4ef8;
  o->f4 = 0;
  o->f8 = 0;
  FUN_08a9e0cc(o);
}
