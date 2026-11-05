/* LBPMATCH module=EBOOT.BIN start=0x08a1f8a4 end=0x08a1f908 symbol=FUN_08a1f8a4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08a1dd70(void *m, int s);
extern void *DAT_08bc3898[];
struct Obj { void **vt; int pad[4]; int f20; };
extern "C" Obj **FUN_08a1f8a4(Obj **out, int s)
{
  Obj *m = (Obj *)FUN_08a9e838(24);
  FUN_08a1dd70(m, s);
  m->vt = DAT_08bc3898;
  m->f20 = s;
  *out = m;
  return out;
}
