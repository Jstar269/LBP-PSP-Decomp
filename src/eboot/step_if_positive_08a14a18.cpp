/* LBPMATCH module=EBOOT.BIN start=0x08a14a18 end=0x08a14a40 symbol=FUN_08a14a18 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
struct Obj_08a14a18 { char pad[152]; float value; };
extern "C" void FUN_08a145d4(Obj_08a14a18 *o);
extern "C" void FUN_08a14a18(Obj_08a14a18 *o)
{
  if (o->value > 0.0f)
    FUN_08a145d4(o);
}
