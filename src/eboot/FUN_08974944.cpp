/* LBPMATCH module=EBOOT.BIN start=0x08974944 end=0x089749b8 symbol=FUN_08974944 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08974380(void *o, void *a);
extern void *DAT_08bbd200[], *DAT_08bbd248[];
struct Obj { void **vt; int f4, f8, f12, f16, f20, f24; void *f28; void *f32; };
extern "C" void FUN_08974944(Obj *o, void *b, int c, void *d)
{
  o->vt = DAT_08bbd200;
  o->f12 = c;
  o->f4 = 0;
  o->f8 = 0;
  o->f16 = 0;
  o->f20 = 0;
  o->f24 = 0;
  if (d) FUN_08974380(o, d);
  o->vt = DAT_08bbd248;
  o->f28 = b;
  o->f32 = 0;
}
