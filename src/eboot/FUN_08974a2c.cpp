/* LBPMATCH module=EBOOT.BIN start=0x08974a2c end=0x08974aa4 symbol=FUN_08974a2c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08970b6c(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bbd200[], *DAT_08bbd248[], *DAT_08bab5b8[];
struct Res { virtual void v0(); virtual void rel(); };
struct Obj { void **vt; int pad[4]; Res *f20; int pad2[2]; void *f32; };
extern "C" void FUN_08974a2c(Obj *o)
{
  o->vt = DAT_08bbd248;
  if (o->f32) FUN_08970b6c(o->f32);
  Res *r = o->f20;
  o->vt = DAT_08bbd200;
  if (r) r->rel();
  o->vt = DAT_08bab5b8;
  FUN_08a9e0cc(o);
}
