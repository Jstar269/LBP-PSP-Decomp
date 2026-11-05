/* LBPMATCH module=EBOOT.BIN start=0x08a0fef0 end=0x08a0ff38 symbol=FUN_08a0fef0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body {
  void AddFlags(unsigned int m) { flags |= m; }
  unsigned int flags;
};
struct World {
  int pad;
  Body *ground; /* +0x4 */
};
extern "C" World *FUN_08a10f1c(void);

extern "C" void FUN_08a0fef0(Body *body, unsigned int mask)
{
  if (FUN_08a10f1c()->ground == body)
    return;
  body->AddFlags(mask);
}
