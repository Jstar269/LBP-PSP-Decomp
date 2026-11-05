/* LBPMATCH module=EBOOT.BIN start=0x08906d74 end=0x08906dd4 symbol=FUN_08906d74 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0899e41c(void *self);
extern "C" void FUN_08a9c014(void *p);
extern void *DAT_08bb5dd0[];
struct Obj { void **vt; int pad[8]; int f36, f40, f44; char s48[24]; int f72; int pad76; char s80[24]; int f104; };
extern "C" void FUN_08906d74(Obj *o, int a)
{
  FUN_0899e41c(o);
  o->vt = DAT_08bb5dd0;
  o->f40 = a;
  o->f36 = 0;
  o->f44 = 0;
  FUN_08a9c014(o->s48);
  o->f72 = 0;
  FUN_08a9c014(o->s80);
  o->f104 = 0;
}
