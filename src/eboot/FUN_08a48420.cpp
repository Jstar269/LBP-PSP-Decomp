/* LBPMATCH module=EBOOT.BIN start=0x08a48420 end=0x08a48490 symbol=FUN_08a48420 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a6b124(void *o, int a, int b, int c, int d);
extern void *DAT_08bc55c0[];
struct Tree { int x; int a4, a8; int *c12; int *c16; int d20; };
struct Obj { void **vt; int pad[146]; Tree t588; Tree t612; };
extern "C" void FUN_08a48420(Obj *o, int a, int b)
{
  FUN_08a6b124(o, a, b, 1, 11);
  Tree *t1 = &o->t588;
  Tree *t2 = &o->t612;
  t1->d20 = 0;
  t1->a4 = 0;
  t1->a8 = 0;
  t1->c12 = &t1->a4;
  t1->c16 = &t1->a4;
  o->vt = DAT_08bc55c0;
  t2->d20 = 0;
  t2->a4 = 0;
  t2->a8 = 0;
  t2->c12 = &t2->a4;
  t2->c16 = &t2->a4;
}
