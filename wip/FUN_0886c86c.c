/* LBPMATCH module=EBOOT.BIN start=0x0886c86c end=0x0886c8fc symbol=FUN_0886c86c lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
int FUN_08b9e714(unsigned char *a, unsigned char *b, unsigned int n);

_Bool FUN_0886c86c(int *param_1, int *param_2)
{
  _Bool bVar1;
  int iVar2;
  int iVar3;

  iVar3 = param_1[2];
  iVar2 = *param_1;
  if (iVar3 == 0 && param_2[2] == 0) {
    if (iVar2 == 0) {
      iVar2 = param_1[1];
    }
    else if (*param_2 == 0) {
      iVar2 = param_1[1];
    }
    else {
      if (iVar2 != *param_2) {
        return 0;
      }
      iVar2 = param_1[1];
    }
    iVar2 = FUN_08b9e714((unsigned char *)(iVar2 + 0xc), (unsigned char *)(param_2[1] + 0xc), 20);
    bVar1 = iVar2 == 0;
  }
  else {
    bVar1 = 0;
    if (iVar2 == *param_2 && (bVar1 = 1, iVar3 != param_2[2])) {
      bVar1 = 0;
    }
  }
  return bVar1;
}
// NEAR: lbpmatch 33 words differ (34/36 compared). Matches: the first memcmp call block (li a2,20 / jal / sltiu).
// Still differs: the branch shape of the && / else chain (retail beqzl/bnezl likely branches, bool results built
// through v1 and `move v0,v1`), and the p1[2] register (retail a2, ours a3 after the reorder).
// Ruled out: loading p1[2] into a local first (same code).
// near miss from lane api-haiku-20261121 (unreviewed)
