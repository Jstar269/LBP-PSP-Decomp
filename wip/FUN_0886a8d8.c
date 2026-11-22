/* LBPMATCH module=EBOOT.BIN start=0x0886a8d8 end=0x0886a99c symbol=FUN_0886a8d8 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
int FUN_08868ed8(int param_1, unsigned int *param_2);
void FUN_0884d5f4(int *out, unsigned int *src);
int FUN_0886a604(int param_1, int param_2, int *param_3);
int FUN_08868d78(int *node);
int FUN_08b9e714(unsigned char *a, unsigned char *b, unsigned int n);

int FUN_0886a8d8(int param_1, unsigned int *param_2)
{
  int iVar1;
  unsigned int iVar2;
  int auStack_20[3];
  int local_14;

  iVar1 = FUN_08868ed8(param_1, param_2);
  if (iVar1 != param_1 + 4) {
    if (param_2[2] == 0 && *(int *)(iVar1 + 0x18) == 0) {
      iVar2 = FUN_08b9e714((unsigned char *)(param_2[1] + 0xc), (unsigned char *)(*(int *)(iVar1 + 0x14) + 0xc), 20);
    }
    else {
      iVar2 = param_2[2] - *(int *)(iVar1 + 0x18);
    }
    if ((iVar2 >> 31) == 0) {
      return iVar1 + 0x1c;
    }
  }
  FUN_0884d5f4(auStack_20, param_2);
  local_14 = 0;
  iVar1 = FUN_0886a604(param_1, iVar1, auStack_20);
  FUN_08868d78(auStack_20);
  return iVar1 + 0x1c;
}
// NEAR: lbpmatch 38 words differ (44/49). The first 10 words (prologue, the FUN_08868ed8 call, the beq) match.
// Still differs: the memcmp arm. Retail tests the FUN_08b9e714 result directly (srl+beqz at a shared join after
// an out-of-line memcmp block that rejoins with `j`); ours sees *(iVar1+0x18)==0 on that path, reuses its v0
// and emits `move v1,v0; subu v1,v1,v0`, then bgez. Ruled out: the srl-style sign test (same), unsigned iVar2
// (same), cc=gcc404xs-O2 (52/49, worse: duplicated epilogues and the join differ).
// near miss from lane api-haiku-20261121 (unreviewed)
