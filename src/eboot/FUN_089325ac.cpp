/* LBPMATCH module=EBOOT.BIN start=0x089325ac end=0x08932618 symbol=FUN_089325ac lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern void *DAT_08bb6d18[], *DAT_08bb6ce0[], *DAT_08bb6cf4[];
struct Obj { void **vt; int f4; void **vt8; int f12, f16, f20, f24; };
extern "C" void FUN_089325ac(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08bb6d18;
  o->f4 = 1;
  FUN_0894a258(&o->vt8);
  o->vt = DAT_08bb6ce0;
  o->vt8 = DAT_08bb6cf4;
  o->f20 = 2;
  o->f12 = 0;
  o->f16 = 0;
  o->f24 = 0;
}
