/* LBPMATCH module=EBOOT.BIN start=0x08875a40 end=0x08875ab4 symbol=FUN_08875a40 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *o);
extern void *DAT_08bb0680[];
struct Obj { void **vt; int f4, f8; char f12; char pad[3]; int f16, f20, f24, f28, f32, f36; };
extern "C" void FUN_08875a40(Obj *o, int a, int b)
{
  FUN_0894a258(o);
  o->f4 = 1;
  o->vt = DAT_08bb0680;
  o->f16 = a;
  o->f24 = b;
  o->f20 = -1;
  o->f8 = 0;
  o->f12 = 0;
  o->f32 = 0;
  o->f28 = 0;
  o->f36 = 0;
}
