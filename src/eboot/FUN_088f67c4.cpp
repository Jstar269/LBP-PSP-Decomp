/* LBPMATCH module=EBOOT.BIN start=0x088f67c4 end=0x088f6828 symbol=FUN_088f67c4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088b2dd0(void *self, void *a, int b);
extern void *DAT_08bb4e80[];
struct Obj { void **vt; int pad[4]; void *f20, *f24; int f28; char f32; char pad2[3]; int f36; };
extern "C" void FUN_088f67c4(Obj *o, void *a, void *b, void *base)
{
  FUN_088b2dd0(o, base, 1);
  o->vt = DAT_08bb4e80;
  o->f20 = a;
  o->f24 = b;
  o->f28 = 0;
  o->f32 = 0;
  o->f36 = 0;
}
