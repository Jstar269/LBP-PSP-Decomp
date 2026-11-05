/* LBPMATCH module=EBOOT.BIN start=0x08a2397c end=0x08a239bc symbol=FUN_08a2397c lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct GeList_08a2397c { unsigned start; unsigned *cur; };
extern "C" GeList_08a2397c *DAT_08bdc18c;
extern "C" void FUN_08a2397c(unsigned *target)
{
  GeList_08a2397c *l = DAT_08bdc18c;
  unsigned addr = *target;
  unsigned *c = l->cur;
  unsigned hi = 0x10000000 | (((addr >> 24) & 0xf) << 16);
  unsigned lo = 0x0a000000 | (addr & 0xffffff);
  c[0] = hi;
  c[1] = lo;
  l->cur = c + 2;
}
