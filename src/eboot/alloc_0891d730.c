/* LBPMATCH module=EBOOT.BIN start=0x0891d730 end=0x0891d774 symbol=FUN_0891d730 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
extern void *FUN_08a9e838(int);

int FUN_0891d730(int *p)
{
  int *q = FUN_08a9e838(8);
  int ok = q != 0;

  q[1] = 0;
  p[43] = 8;
  q[0] = 0;
  p[42] = (int)q;
  return ok;
}
