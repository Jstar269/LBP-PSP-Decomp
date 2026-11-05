/* LBPMATCH module=EBOOT.BIN start=0x0898c2a0 end=0x0898c304 symbol=FUN_0898c2a0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088050b8(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bbfad8[], *DAT_08babc90[];
struct Obj { void **vt; int pad[5]; void *a; void *b; };
extern "C" void FUN_0898c2a0(Obj *o)
{
  o->vt = DAT_08bbfad8;
  if (o->b) FUN_088050b8(o->b);
  if (o->a) FUN_088050b8(o->a);
  o->vt = DAT_08babc90;
  FUN_08a9e0cc(o);
}
