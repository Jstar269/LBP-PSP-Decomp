/* LBPMATCH module=EBOOT.BIN start=0x08a2337c end=0x08a2339c symbol=FUN_08a2337c lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Target {
  int handle;
};
extern "C" Target *DAT_08bdc18c;
extern "C" void FUN_08af3864(int a, int handle, int b, int c, int d);

extern "C" void FUN_08a2337c(void)
{
  FUN_08af3864(0, DAT_08bdc18c->handle, 0, 0, 0);
}
