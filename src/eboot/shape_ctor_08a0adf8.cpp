/* LBPMATCH module=EBOOT.BIN start=0x08a0adf8 end=0x08a0ae2c symbol=FUN_08a0adf8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Shape {
  void *const *vtable;
  int pad[(0x60 - 4) / 4];
  int extra; /* +0x60 */
};
extern "C" void *const DAT_08bc3018[];
extern "C" void FUN_08a08e24(Shape *shape);

extern "C" void FUN_08a0adf8(Shape *shape)
{
  FUN_08a08e24(shape);
  shape->vtable = DAT_08bc3018;
  shape->extra = 0;
}
