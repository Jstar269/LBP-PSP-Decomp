/* LBPMATCH module=EBOOT.BIN start=0x0899e3bc end=0x0899e41c symbol=FUN_0899e3bc lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0890451c(void *p);
extern "C" void FUN_088050b8(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc04a8[];
struct Obj { void **vt; int pad[1]; void *f8; int pad2[3]; void *f24; int pad3[1]; char sub[1]; };
extern "C" void FUN_0899e3bc(Obj *o)
{
  o->vt = DAT_08bc04a8;
  FUN_0890451c(o->sub);
  if (o->f24) FUN_088050b8(o->f24);
  if (o->f8) FUN_088050b8(o->f8);
  FUN_08a9e0cc(o);
}
