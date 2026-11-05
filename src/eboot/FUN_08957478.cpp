/* LBPMATCH module=EBOOT.BIN start=0x08957478 end=0x089574e0 symbol=FUN_08957478 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_08b9eb88(void *p, int v, int n);
extern void *DAT_08bb7d58[];
struct Obj { void **vt; int f4; char buf[20]; int f28; };
extern "C" Obj **FUN_08957478(Obj **out)
{
  Obj *m = (Obj *)FUN_08a9e838(32);
  m->f4 = 1;
  m->vt = DAT_08bb7d58;
  m->f28 = 32;
  FUN_08b9eb88(m->buf, 0, 20);
  *out = m;
  return out;
}
