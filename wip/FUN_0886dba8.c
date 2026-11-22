/* LBPMATCH module=EBOOT.BIN start=0x0886dba8 end=0x0886dc68 symbol=FUN_0886dba8 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bef8f4;

void *FUN_08a9e838(unsigned int n);
void *FUN_0887367c(void *obj, int a1);
void FUN_0886daf8(int *slot, int **ref);
void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);

void FUN_0886dba8(int param_1, int param_2, char *param_3)
{
  int *piVar3;
  int *local_30;
  int *s1;
  int iVar2;

  piVar3 = (int *)(param_3 + 0x44);
  local_30 = (int *)FUN_0887367c(FUN_08a9e838(0x44), param_2);
  FUN_0886daf8(piVar3, &local_30);
  s1 = local_30;
  if (s1 != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar2 = s1[1] - 1;
    s1[1] = iVar2;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar2 == 0) {
      ((void (*)(int *))((int *)s1[0])[1])(s1);
    }
  }
  {
    int *vt = (int *)*(int *)piVar3;
    ((void (*)(int *))((int *)vt[0])[7])(vt);
  }
}
// NEAR: lbpmatch 42 words differ (46/48 compared). Matches: the allocator/factory call pair, the stack local stored
// straight from v0 (no object register), the release test on the stored decremented count (bnez), the virtual call
// through *param_3+68 (slot 7).
// Still differs: the prologue (retail `move s2,a2; addiu s2,s2,68` and `move s1,a1`; ours folds the add and keeps a1
// in s0), so every later register is swapped (retail s1=param_2, s0=result).
// near miss from lane api-haiku-20261121 (unreviewed)
