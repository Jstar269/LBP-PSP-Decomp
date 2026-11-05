/* LBPMATCH module=EBOOT.BIN start=0x088b1c4c end=0x088b1cc4 symbol=FUN_088b1c4c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088b2dd0(void *o, int a, int b);
extern "C" void FUN_08b9eb88(void *p, int v, int n);
extern "C" void FUN_08baa764(int a, int b, int c);
extern void *DAT_08bb30f8[];
struct Tree { int x; int a4, a8; int *c12; int *c16; int d20;
  void init() {
    d20 = 0;
    a4 = 0;
    a8 = 0;
    c12 = &a4;
    c16 = &a4;
  } };
struct Obj { void **vt; int pad[4]; Tree t; };
extern "C" void FUN_088b1c4c(Obj *o)
{
  FUN_088b2dd0(o, 2, 1);
  o->vt = DAT_08bb30f8;
  FUN_08b9eb88(&o->t.a4, 0, 16);
  o->t.init();
  FUN_08baa764(333, 333, 166);
}
