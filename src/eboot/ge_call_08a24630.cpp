/* LBPMATCH module=EBOOT.BIN start=0x08a24630 end=0x08a24670 symbol=FUN_08a24630 lang=c++ cc=gcc404xs-O2 */
/* Clone of src/eboot/ge_call_08a2397c.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct GeList_08a24630 { unsigned start; unsigned *cur; };
extern "C" GeList_08a24630 *DAT_08bdc188;
extern "C" void FUN_08a24630(unsigned *target)
{
  GeList_08a24630 *l = DAT_08bdc188;
  unsigned addr = *target;
  unsigned *c = l->cur;
  unsigned hi = 0x10000000 | (((addr >> 24) & 0xf) << 16);
  unsigned lo = 0x0a000000 | (addr & 0xffffff);
  c[0] = hi;
  c[1] = lo;
  l->cur = c + 2;
}
