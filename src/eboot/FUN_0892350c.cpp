/* LBPMATCH module=EBOOT.BIN start=0x0892350c end=0x08923584 symbol=FUN_0892350c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern "C" void FUN_08b9eb88(void *p, int v, int n);
extern void *DAT_08bb6af0[], *DAT_08bb6ab8[], *DAT_08bb6acc[];
struct Obj { void **vt; int f4; void **vt8; char buf[708]; int f720, f724, f728, f732, f736; };
extern "C" void FUN_0892350c(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08bb6af0;
  o->f4 = 1;
  FUN_0894a258(&o->vt8);
  o->vt = DAT_08bb6ab8;
  o->vt8 = DAT_08bb6acc;
  o->f720 = 0;
  o->f724 = 0;
  o->f728 = 0;
  o->f732 = 0;
  o->f736 = 0;
  FUN_08b9eb88(o->buf, 0, 708);
}
