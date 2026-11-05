/* LBPMATCH module=EBOOT.BIN start=0x08924240 end=0x089242a0 symbol=FUN_08924240 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern "C" void FUN_0894a260(void *p);
extern void *DAT_08bb6b18[], *DAT_08bb6b28[];
struct Obj { void **vt; int pad[13]; int f56; void *f60; };
extern "C" void FUN_08924240(Obj *o)
{
  o->vt = DAT_08bb6b18;
  if (o->f60) {
    FUN_08a9e0c4(o->f60);
    o->f60 = 0;
  }
  o->vt = DAT_08bb6b28;
  o->f56 = 0;
  FUN_0894a260(o);
  FUN_08a9e0cc(o);
}
