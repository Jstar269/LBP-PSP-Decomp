/* LBPMATCH module=EBOOT.BIN start=0x089d9b10 end=0x089d9b80 symbol=FUN_089d9b10 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0896a560(void *o, int a, int b, int c, int d, int e, int f);
extern void *DAT_08bc1930[];
struct Obj { void **vt; int pad[9]; int f40, f44, f48; };
extern "C" void FUN_089d9b10(Obj *o, int a, int b, int c)
{
  FUN_0896a560(o, a, b, 1, 1, 1, 0);
  o->vt = DAT_08bc1930;
  o->f40 = a;
  o->f44 = c;
  o->f48 = b;
}
