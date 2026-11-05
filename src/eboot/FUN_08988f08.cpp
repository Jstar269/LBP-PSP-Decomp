/* LBPMATCH module=EBOOT.BIN start=0x08988f08 end=0x08988f6c symbol=FUN_08988f08 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088050b8(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern void *DAT_08bbf5e0[], *DAT_08babc90[];
struct Obj { void **vt; int pad[2]; void *a; void *b; };
extern "C" void FUN_08988f08(Obj *o)
{
  o->vt = DAT_08bbf5e0;
  if (o->b) FUN_088050b8(o->b);
  if (o->a) FUN_088050b8(o->a);
  o->vt = DAT_08babc90;
  FUN_08a9e0cc(o);
}
