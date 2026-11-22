/* LBPMATCH module=EBOOT.BIN start=0x088783f0 end=0x088784ac symbol=FUN_088783f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;

extern char DAT_08bb0758[];
extern void FUN_0886e018(void *, int);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_088050b8(void *);

void FUN_088783f0(void *param_1, int *param_2)
{
  u32 *self = (u32 *)param_1;
  char *reference;

  FUN_0886e018(param_1, 4);
  self[12] = 0;
  reference = (char *)*param_2;
  self[0] = (u32)DAT_08bb0758;
  char *lock = (char *)((u32)reference + 0x14);
  self[13] = 0;
  self[14] = 0;
  self[15] = 0;
  self[16] = 0;
  self[17] = 0;
  self[18] = 0;
  self[19] = 0;
  if (reference != 0) {
    FUN_08ad3088(lock);
    *(int *)(reference + 8) = *(int *)(reference + 8) + 1;
    FUN_08ad2810(lock);
  }
  {
    void *old = (void *)self[12];
    self[12] = (u32)reference;
    if (old != 0) {
      FUN_088050b8(old);
    }
  }
}
