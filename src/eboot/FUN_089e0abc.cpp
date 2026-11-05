/* LBPMATCH module=EBOOT.BIN start=0x089e0abc end=0x089e0b1c symbol=FUN_089e0abc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0896b250(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc1c40[];
struct Res { virtual void v0(); virtual void release(); };
struct Obj { void **vt; unsigned flags; int pad[4]; Res *res; /* +24 */ };
extern "C" void FUN_089e0abc(Obj *o)
{
  unsigned fl = o->flags;
  o->vt = DAT_08bc1c40;
  if ((fl & 0x1000) && o->res) o->res->release();
  FUN_0896b250(o);
  FUN_08a9e0cc(o);
}
