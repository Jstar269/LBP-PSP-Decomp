/* LBPMATCH module=EBOOT.BIN start=0x08a14cec end=0x08a14d38 symbol=FUN_08a14cec lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Obj_08a14 { char pad[152]; float value; };
extern "C" void FUN_08a13990(Obj_08a14 *o);
extern "C" void FUN_08a14a70(Obj_08a14 *o);
extern "C" void FUN_08a145d4(Obj_08a14 *o);
extern "C" void FUN_08a14210(Obj_08a14 *o);
extern "C" void FUN_08a14cec(Obj_08a14 *o)
{
  FUN_08a14a70(o);
  if (o->value != 0.0f)
    FUN_08a145d4(o);
}
