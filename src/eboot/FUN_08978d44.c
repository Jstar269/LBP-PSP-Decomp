/* LBPMATCH module=EBOOT.BIN start=0x08978d44 end=0x08978df8 symbol=FUN_08978d44 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern void *FUN_08a9e838(int n);
extern void FUN_08978cb8(void *o, int a, int b, int c);
extern void FUN_0897a248(void *o, int a, int b, int c);

int *FUN_08978d44(int a, int b, int c, unsigned char d)
{
  int *parent = (int *)FUN_08a9e838(48);
  int *child;
  int idx;
  int *arr;
  FUN_08978cb8(parent, a, 1, c);
  child = (int *)FUN_08a9e838(56);
  FUN_0897a248(child, 0, b, d ^ 1);
  idx = child[3];
  arr = (int *)parent[6];
  child[2] = (int)parent;
  arr[idx] = (int)child;
  if (idx == 0)
    parent[8] = arr[0];
  return parent;
}
