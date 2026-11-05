/* LBPMATCH module=EBOOT.BIN start=0x08873608 end=0x0887367c symbol=FUN_08873608 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0886e018(void *o, int z);
extern void *DAT_08bb0548[];
struct Obj { void **vt; int pad[11]; void *(*create)(void *); void (*release)(void *); int f56; void *res; void *arg; };
extern "C" void FUN_08873608(Obj *o, void *arg, void *(*create)(void *), void (*release)(void *))
{
  FUN_0886e018(o, 0);
  o->release = release;
  o->create = create;
  o->vt = DAT_08bb0548;
  o->f56 = 0;
  o->res = create(arg);
  o->arg = arg;
}
