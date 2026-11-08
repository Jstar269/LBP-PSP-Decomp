/* LBPMATCH module=EBOOT.BIN start=0x0894a33c end=0x0894a380 symbol=FUN_0894a33c lang=c99 */
/* Clone of src/eboot/alloc_0891d730.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
extern void *FUN_08a9e838(int);

int FUN_0894a33c(int *p)
{
  int *q = FUN_08a9e838(8);
  int ok = q != 0;

  q[1] = 0;
  p[43] = 8;
  q[0] = 0;
  p[42] = (int)q;
  return ok;
}
