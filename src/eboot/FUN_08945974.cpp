/* LBPMATCH module=EBOOT.BIN start=0x08945974 end=0x089459e8 symbol=FUN_08945974 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08944bf0(void *o);
extern "C" void FUN_08a9c304(void *o, void *t);
extern "C" void FUN_0894a258(void *o);
extern void *DAT_08bb7270[], *DAT_08bb7288[], *DAT_08bb729c[];
struct T { int a, b; };
struct Obj { void **vt; int pad[17]; int sub72[6]; int f96, f100; void **f104; int f108; };
extern "C" void FUN_08945974(Obj *o)
{
  FUN_08944bf0(o);
  o->vt = DAT_08bb7270;
  T t;
  t.a = 0;
  t.b = 0;
  FUN_08a9c304(o->sub72, &t);
  o->f96 = 0;
  o->f100 = 0;
  FUN_0894a258(&o->f104);
  o->vt = DAT_08bb7288;
  o->f104 = DAT_08bb729c;
  o->f108 = 1;
}
