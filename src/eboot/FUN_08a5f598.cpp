/* LBPMATCH module=EBOOT.BIN start=0x08a5f598 end=0x08a5f608 symbol=FUN_08a5f598 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a31dc8(void *m, int k);
extern "C" void FUN_08a5f3dc(void *m, void *a, void *b);
extern void *DAT_08bc6978[];
struct Obj { void **vt; int pad[97]; int f392; };
extern "C" Obj *FUN_08a5f598(void *a, void *b)
{
  Obj *m = (Obj *)FUN_08a9e838(576);
  FUN_08a31dc8(m, 17);
  m->vt = DAT_08bc6978;
  m->f392 = 0;
  FUN_08a5f3dc(m, a, b);
  return m;
}
