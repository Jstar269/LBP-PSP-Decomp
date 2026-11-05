/* LBPMATCH module=EBOOT.BIN start=0x089db2b8 end=0x089db324 symbol=FUN_089db2b8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089d831c(void *o, int a, int b, int c);
extern void *DAT_08bc1970[];
struct Obj { void **vt; unsigned f4; int pad[12]; int f56, f60, f64, f68, f72; };
extern "C" void FUN_089db2b8(Obj *o, int a, int b, int c, int d)
{
  FUN_089d831c(o, a, d, c);
  o->f4 |= 0x4000;
  o->vt = DAT_08bc1970;
  o->f56 = b;
  o->f60 = c;
  o->f68 = 0;
  o->f72 = 0;
  o->f64 = 0;
}
