/* LBPMATCH module=EBOOT.BIN start=0x089d2c68 end=0x089d2cdc symbol=FUN_089d2c68 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089d33a4(void *o);
extern void *DAT_08bc1630[];
struct Obj { void **vt; int pad[5]; int f24, f28, f32, f36, f40, f44, f48; };
extern "C" void FUN_089d2c68(Obj *o, int a, int b, int c)
{
  FUN_089d33a4(o);
  o->vt = DAT_08bc1630;
  o->f24 = a;
  o->f28 = b;
  o->f32 = -1;
  o->f36 = 0;
  o->f40 = 0;
  o->f44 = c;
  o->f48 = 0;
}
