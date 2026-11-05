/* LBPMATCH module=EBOOT.BIN start=0x08957650 end=0x089576c8 symbol=FUN_08957650 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_089508d0(void *p);
extern "C" void FUN_08957390(void *p);
extern void *DAT_08bb7d68[], *DAT_08bb7e10[], *DAT_08bb7e64[];
struct Elem { int a, b; };
struct Obj { void **vt; void **vt4; int pad[10]; Elem arr[4]; };
extern "C" void FUN_08957650(Obj *o)
{
  o->vt = DAT_08bb7d68;
  FUN_089508d0(&o->vt4);
  o->vt = DAT_08bb7e10;
  o->vt4 = DAT_08bb7e64;
  Elem *p = o->arr;
  int n = 4;
  do { FUN_08957390(p); p++; } while (--n);
}
