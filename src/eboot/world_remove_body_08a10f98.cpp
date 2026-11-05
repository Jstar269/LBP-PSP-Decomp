/* LBPMATCH module=EBOOT.BIN start=0x08a10f98 end=0x08a11008 symbol=FUN_08a10f98 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body;
struct World {
  int pad;
  Body *ground;        /* +0x4 */
  Body *bodies[2048];  /* +0x8 */
  unsigned int count;  /* +0x2008 */
  void Remove(Body *body)
  {
    for (unsigned int i = 0; i < count; ++i) {
      if (bodies[i] == body) {
        bodies[i] = bodies[--count];
        bodies[count] = 0;
        return;
      }
    }
  }
};

extern "C" void FUN_08a10f98(World *world, Body *body)
{
  if (world->ground == body)
    return;
  world->Remove(body);
}
