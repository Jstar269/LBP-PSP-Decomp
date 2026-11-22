/* LBPMATCH module=EBOOT.BIN start=0x0886d8ec end=0x0886d99c symbol=FUN_0886d8ec lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bef8f4;

void FUN_08873454(int p);
void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);
void FUN_0888256c(void);

void FUN_0886d8ec(int *param_1, int *param_2)
{
  void (*pcVar1)(int *);
  int *piVar2;
  int iVar3;
  int iVar4;

  piVar2 = param_2;
  if (*(int *)(*param_1 + 0x40) == 0) {
    FUN_08873454(param_2[0x11]);
    FUN_08873454(param_2[0x12]);
    pcVar1 = *(void (**)(int *))(*piVar2 + 0x14);
  }
  else {
    pcVar1 = *(void (**)(int *))(*piVar2 + 0x10);
  }
  (*pcVar1)(piVar2);
  FUN_08ad3088(&DAT_08bef8f4);
  iVar3 = piVar2[1];
  piVar2[2] = piVar2[2] - 1;
  iVar4 = iVar3 - 1;
  piVar2[1] = iVar4;
  FUN_08ad2810(&DAT_08bef8f4);
  if (iVar4 == 0) {
    (*(void (**)(int *))(*piVar2 + 4))(piVar2);
  }
  FUN_0888256c();
}
// NEAR: lbpmatch 42 words differ (43/44 compared). Matches: the virtual dispatch (slot 0x10/0x14 by the flag), the
// refcount block with the decremented value stored and tested (bnez), the tail call to 0x0888256c and the release
// (slot 1). Still differs: retail copies param_2 into s2 in the prologue, before the flag test (`move s2,a1` first
// word); ours sinks the copy into the arms, so the rest is shifted by one word. Retail's unlock delay slot holds
// `sw s0,4(s2)` as ours does.
// Ruled out: piVar2 for every later use (same layout), the decrement tested as `== 1` (wrong test).
// near miss from lane api-haiku-20261121 (unreviewed)
