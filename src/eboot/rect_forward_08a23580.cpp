/* LBPMATCH module=EBOOT.BIN start=0x08a23580 end=0x08a235a0 symbol=FUN_08a23580 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Rect {
  int x;
  int y;
  int w;
  int h;
};
extern "C" void *DAT_08bdc188;
extern "C" void FUN_08a234e8(void *target, int x, int y, int w, int h);

extern "C" void FUN_08a23580(Rect *rect)
{
  FUN_08a234e8(DAT_08bdc188, rect->x, rect->y, rect->w, rect->h);
}
