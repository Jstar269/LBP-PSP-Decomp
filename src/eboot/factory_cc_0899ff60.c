/* LBPMATCH module=EBOOT.BIN start=0x0899ff60 end=0x089a0018 symbol=FUN_0899ff60 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

extern void *FUN_08a9e838(int size);
extern void FUN_0899f9f4(void *o, int a);
extern void FUN_0899e9a8(void *o, int a, int b, int c, int d);

void *FUN_0899ff60(int a, unsigned char flag)
{
  void *o;

  o = FUN_08a9e838(204);
  FUN_0899f9f4(o, a);
  FUN_0899e9a8(o, 148, 0, 23, 0);
  FUN_0899e9a8(o, 150, 1, 25, 0);
  FUN_0899e9a8(o, 147, 2, 26, 0);
  if (flag)
    FUN_0899e9a8(o, 149, 3, 24, 0);
  return o;
}
