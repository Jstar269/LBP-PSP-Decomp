/* LBPMATCH module=EBOOT.BIN start=0x08875e88 end=0x08875f1c symbol=FUN_08875e88 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;

extern void *FUN_08a9e838(u32);
extern void FUN_0887367c(void *, void *);
extern void FUN_08873608(void *, void *, void *, u32);

void **FUN_08875e88(void **param_1, char *param_2, void *param_3)
{
  if (*(int *)(param_2 + 0x28) != 0) {
    void *allocation = FUN_08a9e838(0x44);
    FUN_08873608(allocation, param_3, *(void **)(param_2 + 0x28), *(u32 *)(param_2 + 0x2c));
    *param_1 = allocation;
  } else {
    void *allocation = FUN_08a9e838(0x44);
    FUN_0887367c(allocation, param_3);
    *param_1 = allocation;
  }
  return param_1;
}
