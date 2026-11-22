/* LBPMATCH module=EBOOT.BIN start=0x089835d0 end=0x08983664 symbol=FUN_089835d0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern void *FUN_08a9e838(int n);
extern void FUN_089ce064(void *o, int a);
extern void FUN_089db2b8(void *o, void *a, int b, int c, int d);
extern void *FUN_08983418(void);
extern void FUN_089da9ac(void *o, void *p);

void *FUN_089835d0(int a, int b, int c)
{
  void *x = FUN_08a9e838(52);
  void *y;
  FUN_089ce064(x, c);
  y = FUN_08a9e838(76);
  FUN_089db2b8(y, x, a, b, 2);
  FUN_089da9ac(y, FUN_08983418());
  return y;
}
