/* LBPMATCH module=EBOOT.BIN start=0x08865ec0 end=0x08865f30 symbol=FUN_08865ec0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0885d290(void *o, int a, int b, int c);
extern "C" void FUN_0886469c(void *p);
extern "C" int FUN_08887624();
extern "C" void FUN_08ba0868(char *buf, int n, const char *fmt, int v);
extern "C" void FUN_0885d244(void *o, const char *s);
extern void *DAT_08bafe78[];
extern char DAT_08baff44[];
struct Obj { void **vt; int pad[51]; char s208[1]; };
extern "C" void FUN_08865ec0(Obj *o)
{
  char buf[256];
  FUN_0885d290(o, 2, 0, 5);
  o->vt = DAT_08bafe78;
  FUN_0886469c(o->s208);
  FUN_08ba0868(buf, 256, DAT_08baff44, FUN_08887624());
  FUN_0885d244(o, buf);
}
