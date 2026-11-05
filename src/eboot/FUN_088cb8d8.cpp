/* LBPMATCH module=EBOOT.BIN start=0x088cb8d8 end=0x088cb948 symbol=FUN_088cb8d8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088b2dd0(void *o, int a, int b);
extern void *DAT_08bb3fd0[];
extern char DAT_08bee6cc, DAT_08bee6cd, DAT_08bee6ce;
struct Obj { void **vt; int pad[4]; int f20, f24, f28, f32, f36, f40; };
extern "C" void FUN_088cb8d8(Obj *o)
{
  FUN_088b2dd0(o, 12, 2);
  o->f40 = 8;
  DAT_08bee6cd = 1;
  o->vt = DAT_08bb3fd0;
  DAT_08bee6cc = 0;
  DAT_08bee6ce = 0;
  o->f24 = 0;
  o->f28 = 0;
  o->f32 = 0;
  o->f36 = 0;
  o->f20 = 0;
}
