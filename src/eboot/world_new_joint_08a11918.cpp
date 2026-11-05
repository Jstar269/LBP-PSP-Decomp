/* LBPMATCH module=EBOOT.BIN start=0x08a11918 end=0x08a11998 symbol=FUN_08a11918 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Joint;
struct World {
  int pad[2];
  Joint *joints[2048]; /* +0x8 */
  unsigned int count;  /* +0x2008 */
};
extern "C" void *FUN_08a9e838(unsigned int size);
extern "C" void FUN_08a10664(Joint *joint, void *b, void *a);

extern "C" Joint *FUN_08a11918(World *world, void *a, void *b)
{
  Joint *joint = (Joint *)FUN_08a9e838(272);
  FUN_08a10664(joint, b, a);
  if (world->count < 2048)
    world->joints[world->count++] = joint;
  return joint;
}
