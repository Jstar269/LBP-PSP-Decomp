/* LBPMATCH module=EBOOT.BIN start=0x08a10554 end=0x08a105a8 symbol=FUN_08a10554 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Joint;
extern "C" void *FUN_08a9e838(unsigned int size);
extern "C" void FUN_08a10450(Joint *joint, void *a, void *b);

extern "C" Joint *FUN_08a10554(void *a, void *b)
{
  Joint *joint = (Joint *)FUN_08a9e838(272);
  FUN_08a10450(joint, a, b);
  return joint;
}
