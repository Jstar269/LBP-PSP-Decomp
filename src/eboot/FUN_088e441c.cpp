/* LBPMATCH module=EBOOT.BIN start=0x088e441c end=0x088e447c symbol=FUN_088e441c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Pair { int a, b; };
extern int DAT_08bb440c;
extern "C" int FUN_088e3e74(void *a, int b, int c, int d);
extern "C" void FUN_088e66cc(void *p, int a);
extern "C" void FUN_088f37d0(void *p, Pair *q);
extern void *DAT_08beee0c;
extern "C" void FUN_088e441c(void *self, Pair *p)
{
  Pair loc;
  loc.b = p->b;
  loc.a = p->a;
  if (FUN_088e3e74(self, 21, 1, DAT_08bb440c)) {
    FUN_088e66cc(DAT_08beee0c, 4);
    FUN_088f37d0(DAT_08beee0c, &loc);
  }
}
