/* LBPMATCH module=EBOOT.BIN start=0x089497f0 end=0x08949854 symbol=FUN_089497f0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern "C" void FUN_0894a260(void *p);
extern void *DAT_08bb76e0[], *DAT_08bb7730[];
struct Obj { void **vt; int pad[10]; int f44, f48; void *f52; };
extern "C" void FUN_089497f0(Obj *o)
{
  o->vt = DAT_08bb76e0;
  if (o->f52) {
    FUN_08a9e0c4(o->f52);
    o->f52 = 0;
    o->f44 = 0;
    o->f48 = 0;
  }
  o->vt = DAT_08bb7730;
  FUN_0894a260(o);
  FUN_08a9e0cc(o);
}
