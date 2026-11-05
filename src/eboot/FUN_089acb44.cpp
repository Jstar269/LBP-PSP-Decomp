/* LBPMATCH module=EBOOT.BIN start=0x089acb44 end=0x089acbb8 symbol=FUN_089acb44 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0899fc08(void *o, void *a);
extern "C" void FUN_0896df60(void *o, void *a, int b);
extern "C" void FUN_0896dbfc(void *o, int a, int b, int c);
extern "C" void FUN_0896e114(void *o, int a, int b, int c);
extern void *DAT_08bc0810[];
struct Obj { void **vt; int pad[27]; void *f112; };
extern "C" void FUN_089acb44(Obj *o, void *a, void *b)
{
  FUN_0899fc08(o, a);
  o->vt = DAT_08bc0810;
  o->f112 = b;
  FUN_0896df60(o, b, 1);
  FUN_0896dbfc(o, 1, 2, 0);
  FUN_0896e114(o, 1, 1, 0);
}
