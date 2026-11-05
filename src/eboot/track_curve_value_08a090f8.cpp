/* LBPMATCH module=EBOOT.BIN start=0x08a090f8 end=0x08a0911c symbol=FUN_08a090f8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
struct Curve {
  int pad[0x2c / 4];
  float values[1]; /* +0x2c */
};
struct Track {
  int pad[0x44 / 4];
  Curve *curve; /* +0x44 */
  int pad2;
  int index;    /* +0x4c */
};

extern "C" float FUN_08a090f8(Track *track)
{
  if (!track->curve)
    return 0.0f;
  return track->curve->values[track->index];
}
