/* LBPMATCH module=EBOOT.BIN start=0x0885c4ec end=0x0885c550 symbol=FUN_0885c4ec lang=c++ rodata=0x08baf684 */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0885d290(void *o, int a, int b, int n);
extern "C" const char *FUN_08887624(void);
extern "C" int FUN_08ba0868(char *buf, int n, const char *fmt, ...);
extern "C" void FUN_0885d244(void *o, const char *path);
extern void *DAT_08baf648[];
struct Obj { void **vt; };
extern "C" void FUN_0885c4ec(Obj *o)
{
  char buf[256];
  FUN_0885d290(o, 2, 0, 5);
  o->vt = DAT_08baf648;
  FUN_08ba0868(buf, 256, "%s/grief", FUN_08887624());
  FUN_0885d244(o, buf);
}
