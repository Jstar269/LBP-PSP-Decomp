/* LBPMATCH module=EBOOT.BIN start=0x08877148 end=0x08877198 symbol=FUN_08877148 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_0894a260(void *p);
extern void *DAT_08bb0738[], *DAT_08bb0748[];
struct Obj { void **vt; int pad[1]; void *res; };
extern "C" void FUN_08877148(Obj *o)
{
  o->vt = DAT_08bb0738;
  if (o->res) FUN_08a9e0c4(o->res);
  o->vt = DAT_08bb0748;
  FUN_0894a260(o);
}
