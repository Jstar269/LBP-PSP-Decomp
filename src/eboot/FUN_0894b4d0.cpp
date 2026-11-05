/* LBPMATCH module=EBOOT.BIN start=0x0894b4d0 end=0x0894b530 symbol=FUN_0894b4d0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a8d8(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bb7798[], *DAT_08bb7778[];
struct Obj { void **vt; int pad[1]; void *f8; };
extern "C" void FUN_0894b4d0(Obj *o)
{
  void *r = o->f8;
  o->vt = DAT_08bb7798;
  if (r) {
    FUN_0894a8d8(r);
    FUN_08a9e0cc(r);
  }
  o->vt = DAT_08bb7778;
  FUN_08a9e0cc(o);
}
