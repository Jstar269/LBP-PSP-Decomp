/* LBPMATCH module=EBOOT.BIN start=0x088e85e4 end=0x088e8648 symbol=FUN_088e85e4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bb4ce8[], *DAT_08bb3250[];
struct Obj { void **vt; int pad[1]; void *f8; int pad2[2]; void *f20; };
extern "C" void FUN_088e85e4(Obj *o)
{
  o->vt = DAT_08bb4ce8;
  if (o->f20) FUN_08a9e0cc(o->f20);
  if (o->f8) FUN_08a9e0cc(o->f8);
  o->vt = DAT_08bb3250;
  FUN_08a9e0cc(o);
}
