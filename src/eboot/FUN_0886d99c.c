/* LBPMATCH module=EBOOT.BIN start=0x0886d99c end=0x0886da74 symbol=FUN_0886d99c lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bef8f4;

void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);

void FUN_0886d99c(int *param_1)
{
  int *piVar2;
  int iVar1;
  int iVar4;

  piVar2 = (int *)param_1[17];
  param_1[17] = 0;
  if (piVar2 != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar1 = piVar2[1];
    iVar4 = iVar1 - 1;
    piVar2[1] = iVar4;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar4 == 0) {
      ((void (*)(int *))((int *)piVar2[0])[1])(piVar2);
    }
  }
  piVar2 = (int *)param_1[18];
  param_1[18] = 0;
  if (piVar2 != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar1 = piVar2[1];
    iVar4 = iVar1 - 1;
    piVar2[1] = iVar4;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar4 == 0) {
      ((void (*)(int *))((int *)piVar2[0])[1])(piVar2);
    }
  }
  param_1[3] = 3;
}
