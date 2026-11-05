/* LBPMATCH module=EBOOT.BIN start=0x08a0d0d4 end=0x08a0d0f0 symbol=FUN_08a0d0d4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body {
  int pad[0x44 / 4];
  unsigned int flag0 : 1;  /* +0x44 */
  unsigned int asleep : 1;
  unsigned int flags2 : 3;
  unsigned int bit5 : 1;
  int pad2[(0x5c - 0x48) / 4];
  unsigned int dirty : 1;  /* +0x5c */
};

extern "C" void FUN_08a0d0d4(Body *body)
{
  body->asleep = 0;
  body->dirty = 1;
}
