/* LBPMATCH module=EBOOT.BIN start=0x088dec3c end=0x088decb0 symbol=FUN_088dec3c lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Slots {
  char pad[0xc];
  int ids[4]; /* +0xc */
};
extern "C" void FUN_088de758(Slots *slots, int index, int id);

extern "C" void FUN_088dec3c(Slots *slots)
{
  for (int i = 0; i < 4; ++i) {
    if (slots->ids[i] != -1) {
      FUN_088de758(slots, i, slots->ids[i]);
      slots->ids[i] = -1;
    }
  }
}
