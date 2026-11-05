/* LBPMATCH module=EBOOT.BIN start=0x089cddd8 end=0x089cde38 symbol=FUN_089cddd8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089e15dc(void *a, void *b, int c);
extern "C" void FUN_088050b8(void *p);
extern "C" void FUN_089d1988(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bc1368[];
extern void *DAT_08bf0380;
struct Obj { void **vt; int pad[6]; int f28; void *f32; };
extern "C" void FUN_089cddd8(Obj *o)
{
  o->vt = DAT_08bc1368;
  FUN_089e15dc(DAT_08bf0380, &o->f32, o->f28);
  if (o->f32) FUN_088050b8(o->f32);
  FUN_089d1988(o);
  FUN_08a9e0cc(o);
}
