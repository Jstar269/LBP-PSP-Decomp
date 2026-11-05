/* LBPMATCH module=EBOOT.BIN start=0x089acbc8 end=0x089acc28 symbol=FUN_089acbc8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0896cd08(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc0860[], *DAT_08bc0538[];
struct Res { virtual void v0(); virtual void release(); };
struct Obj { void **vt; int pad[53]; Res *res; };
extern "C" void FUN_089acbc8(Obj *o)
{
  Res *r = o->res;
  o->vt = DAT_08bc0860;
  if (r) r->release();
  o->vt = DAT_08bc0538;
  FUN_0896cd08(o);
  FUN_08a9e0cc(o);
}
