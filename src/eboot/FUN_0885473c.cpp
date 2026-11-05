/* LBPMATCH module=EBOOT.BIN start=0x0885473c end=0x08854794 symbol=FUN_0885473c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088545dc(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern "C" void FUN_088532c0(void *tree, int root);
extern void *DAT_08baed80[];
struct Tree { int a, b, root; void clear() { FUN_088532c0(this, root); } };
struct Obj { void **vt; int pad[2]; void *res; /* +12 */ Tree tree; /* +16 */ };
extern "C" void FUN_0885473c(Obj *o)
{
  void *r = o->res;
  o->vt = DAT_08baed80;
  if (r) {
    FUN_088545dc(r);
    FUN_08a9e0cc(r);
  }
  o->tree.clear();
}
