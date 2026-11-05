/* LBPMATCH module=EBOOT.BIN start=0x08945068 end=0x089450cc symbol=FUN_08945068 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08944bf0(void *self);
extern "C" void FUN_0894a258(void *p);
extern void *DAT_08bb73c8[], *DAT_08bb73dc[];
struct Obj { void **vt; int pad[16]; void **f68; int f72, f76, f80, f84, f88; int *f92; int f96; };
extern "C" void FUN_08945068(Obj *o)
{
  FUN_08944bf0(o);
  FUN_0894a258(&o->f68);
  o->vt = DAT_08bb73c8;
  o->f68 = DAT_08bb73dc;
  o->f92 = &o->f88;
  o->f72 = 0;
  o->f76 = 0;
  o->f80 = 0;
  o->f84 = 0;
  o->f88 = 0;
  o->f96 = 0;
}
