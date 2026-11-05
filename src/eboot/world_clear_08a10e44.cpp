/* LBPMATCH module=EBOOT.BIN start=0x08a10e44 end=0x08a10f1c symbol=_ZN14World_08a10e445ClearEv lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body_08a0febc {
  ~Body_08a0febc();
};
struct World_08a10e44 {
  void Clear();
  int pad;
  Body_08a0febc *ground;        /* +0x4 */
  Body_08a0febc *bodies[2048];  /* +0x8 */
  unsigned int count;           /* +0x2008 */
  int *extra;                   /* +0x200c */
  unsigned int extraCount;      /* +0x2010 */
};

void World_08a10e44::Clear()
{
  bool groundDeleted = false;
  for (unsigned int i = 0; i < count; ++i) {
    if (bodies[i] == ground)
      groundDeleted = true;
    delete bodies[i];
    bodies[i] = 0;
  }
  count = 0;
  if (extra) {
    delete[] extra;
    extra = 0;
  }
  extraCount = 0;
  if (!groundDeleted)
    delete ground;
  ground = 0;
}
