/* LBPMATCH module=EBOOT.BIN start=0x0884bd24 end=0x0884bd98 symbol=FUN_0884bd24 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08879430(void *o, void *a, void (*fn)(), int n);
extern "C" void FUN_0884bd08();
extern void *DAT_08baec90[], *DAT_08baecc8[];
struct Obj { void **vt; int pad[2]; void **vt2; int pad2[46]; int arr[64]; int f456, f460, f464; };
extern "C" void FUN_0884bd24(Obj *o, void *a)
{
  FUN_08879430(o, a, FUN_0884bd08, 3);
  int *p = o->arr;
  o->vt = DAT_08baec90;
  
  o->vt2 = DAT_08baecc8;
  
  int i = 64;
  
  do { *p++ = 0; } while (--i);
  o->f464 = -1;
  o->f460 = 64;
  o->f456 = 0;
}
