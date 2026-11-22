/* LBPMATCH module=EBOOT.BIN start=0x089a0018 end=0x089a00ec symbol=FUN_089a0018 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

extern void *FUN_08a9e838(int size);
extern void FUN_0899faa8(void *o, int a, int b);
extern void FUN_0899e898(void *o, int a, int b, int c, int d, int e);

void *FUN_089a0018(int a, unsigned char flag)
{
  void *o;

  o = FUN_08a9e838(204);
  FUN_0899faa8(o, a, 22001);
  FUN_0899e898(o, 163, 0, 169, 0, 0);
  if (!flag)
    FUN_0899e898(o, 160, 1, 170, 1, 0);
  FUN_0899e898(o, 161, 2, 171, 1, 0);
  if (!flag)
    FUN_0899e898(o, 162, 3, 168, 1, 0);
  return o;
}
