/* LBPMATCH module=EBOOT.BIN start=0x08a0d080 end=0x08a0d0d4 symbol=FUN_08a0d080 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct World {
  int pad[0x206c / 4];
  float clock; /* +0x206c */
};
struct Timer {
  int pad[0x1c / 4];
  float offset; /* +0x1c */
  float fixed;  /* +0x20 */
  int pad2[(0x44 - 0x24) / 4];
  bool bits0 : 1;
  bool bit1 : 1;
  bool bit2 : 1;
  bool bit3 : 1;
  bool bit4 : 1;
  bool paused : 1;
};
extern "C" World *FUN_08a10f1c(void);

extern "C" float FUN_08a0d080(Timer *timer)
{
  if (timer->paused)
    return timer->fixed;
  return FUN_08a10f1c()->clock + timer->offset;
}
