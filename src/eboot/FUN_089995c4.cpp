/* LBPMATCH module=EBOOT.BIN start=0x089995c4 end=0x08999628 symbol=FUN_089995c4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08974824(void *self, void *a, void *b);
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_088112d8(void *p);
extern void *DAT_08bc0178[];
struct Obj { void **vt; int pad[3]; int f16; int pad2[2]; int f28; void *f32; };
extern "C" void FUN_089995c4(Obj *o, void *a)
{
  FUN_08974824(o, a, 0);
  o->vt = DAT_08bc0178;
  o->f28 = -1;
  o->f16 = 14;
  void *n = FUN_08a9e838(320);
  FUN_088112d8(n);
  o->f32 = n;
}
