/* LBPMATCH module=EBOOT.BIN start=0x089ea7d0 end=0x089ea80c symbol=FUN_089ea7d0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Meter {
  int value;
};
struct Gauge {
  void *vtable;
  Meter meter;           /* +0x4 */
  int pad[(0x94 - 0x8) / 4];
  float thresholds[15];  /* +0x94 */
};
extern "C" void FUN_08a1fd7c(Meter *meter, int level);

extern "C" void FUN_089ea7d0(Gauge *gauge, float value)
{
  int i;
  for (i = 0; i < 15; ++i)
    if (gauge->thresholds[i] < value)
      break;
  FUN_08a1fd7c(&gauge->meter, 16 - i);
}
