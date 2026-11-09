/* LBPMATCH module=EBOOT.BIN start=0x088755c4 end=0x08875664 symbol=FUN_088755c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); matching idiom: VLA reserve is length+9 bytes, whose toolchain lowering yields the retail aligned stack adjustment; exact match verified; names and types await human review. */
typedef unsigned int u32;

extern int FUN_08baa744(void *, void *, void *, char **, u32 *);
extern void FUN_08ba10e8(void *, const void *, u32);
extern u32 FUN_08878884(u32, const char *);

u32 FUN_088755c4(char *param_1, u32 *param_2, void *param_3)
{
  char *local_40;
  u32 local_3c;
  int count = FUN_08baa744(*(void **)(param_1 + 0x10), *(void **)(param_1 + 0x14), param_3, &local_40, &local_3c);
  u32 result = 0;

  if (count > 0) {
    char buffer[local_3c + 9];
    FUN_08ba10e8(buffer, local_40, local_3c);
    buffer[local_3c] = 0;
    result = FUN_08878884(param_2[0], buffer);
  }
  return result;
}
