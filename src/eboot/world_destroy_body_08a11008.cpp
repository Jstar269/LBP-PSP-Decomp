/* LBPMATCH module=EBOOT.BIN start=0x08a11008 end=0x08a110b4 symbol=FUN_08a11008 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane, then decomp-opus lane 2026-11-04, Claude Opus 5.5); a signed loop index compared with `(int)count` gives retail's `beq i,count` exit compare; exact match verified; names and types await human review. */
struct Body_08a0febc {
  ~Body_08a0febc();
};
struct World {
  int pad;
  Body_08a0febc *ground;        /* +0x4 */
  Body_08a0febc *bodies[2048];  /* +0x8 */
  unsigned int count;           /* +0x2008 */
  void Destroy(Body_08a0febc *body)
  {
    for (int i = 0; i != (int)count; ++i) {
      if (bodies[i] == body) {
        delete body;
        bodies[i] = bodies[--count];
        bodies[count] = 0;
        return;
      }
    }
  }
};

extern "C" void FUN_08a11008(World *world, Body_08a0febc *body)
{
  if (world->ground == body)
    return;
  world->Destroy(body);
}
