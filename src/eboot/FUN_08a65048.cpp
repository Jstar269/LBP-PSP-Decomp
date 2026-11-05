/* LBPMATCH module=EBOOT.BIN start=0x08a65048 end=0x08a650b8 symbol=FUN_08a65048 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a31dc8(void *m, int k);
extern "C" void FUN_08a6aed4(void *m, void *a, void *b);
extern void *DAT_08bc7250[];
struct Obj { void **vt; int pad[97]; int f392; };
extern "C" Obj *FUN_08a65048(void *a, void *b)
{
  Obj *m = (Obj *)FUN_08a9e838(592);
  FUN_08a31dc8(m, 27);
  m->vt = DAT_08bc7250;
  m->f392 = 0;
  FUN_08a6aed4(m, a, b);
  return m;
}
