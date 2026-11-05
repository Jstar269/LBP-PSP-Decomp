/* LBPMATCH module=EBOOT.BIN start=0x08a23350 end=0x08a2337c symbol=FUN_08a23350 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_08af3bd8(int a, int b);
extern "C" int FUN_08a23350(int x)
{
  if (x == 0) return FUN_08af3bd8(4, 0);
  if (x == 1) return FUN_08af3bd8(0, 0);
  return 1;
}
