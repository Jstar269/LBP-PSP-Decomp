/* LBPMATCH module=EBOOT.BIN start=0x08a145ac end=0x08a145d4 symbol=FUN_08a145ac lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
struct Obj_08a145ac { char pad[148]; float value; };
extern "C" void FUN_08a14454(Obj_08a145ac *o);
extern "C" void FUN_08a145ac(Obj_08a145ac *o)
{
  if (o->value == 0.0f)
    return;
  FUN_08a14454(o);
}
