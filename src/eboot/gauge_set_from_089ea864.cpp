/* LBPMATCH module=EBOOT.BIN start=0x089ea864 end=0x089ea8bc symbol=FUN_089ea864 lang=c++ cc=gcc404xs-O2 */
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
struct Source {
  int id;
};

static inline void setFromValue(Gauge *gauge, float value)
{
  int i;
  for (i = 0; i < 15; ++i)
    if (gauge->thresholds[i] < value)
      break;
  FUN_08a1fd7c(&gauge->meter, 16 - i);
}

extern "C" void FUN_089ea864(Gauge *gauge, Source *source, float value)
{
  if (source && source->id < 0)
    setFromValue(gauge, value);
  else
    FUN_08a1fd7c(&gauge->meter, 0);
}
