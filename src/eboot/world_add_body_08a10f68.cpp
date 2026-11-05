/* LBPMATCH module=EBOOT.BIN start=0x08a10f68 end=0x08a10f98 symbol=FUN_08a10f68 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body;
struct World {
  int pad[2];
  Body *bodies[2048];  /* +0x8 */
  unsigned int count;  /* +0x2008 */
};

extern "C" bool FUN_08a10f68(World *world, Body *body)
{
  if (world->count < 2048) {
    world->bodies[world->count++] = body;
    return true;
  }
  return false;
}
