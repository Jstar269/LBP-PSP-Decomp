/* LBPMATCH module=EBOOT.BIN start=0x08a23e60 end=0x08a23ea0 symbol=FUN_08a23e60 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct GeList_08a23e60 { unsigned start; unsigned *cur; };
extern "C" void *FUN_08a23e60(unsigned size, GeList_08a23e60 *l)
{
  unsigned *c = l->cur;
  unsigned *data = c + 2;
  unsigned *e = data + (size >> 2);
  unsigned end = (unsigned)e;
  c[0] = 0x10000000 | (((end >> 24) & 0xf) << 16);
  c[1] = 0x08000000 | (end & 0xffffff);
  l->cur = (unsigned *)end;
  return data;
}
