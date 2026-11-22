/* LBPMATCH module=EBOOT.BIN start=0x0886c924 end=0x0886c9e0 symbol=FUN_0886c924 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern char DAT_08bb03a0[];
extern char DAT_08bb03d0[];
extern int DAT_08bef8f4;

void FUN_08a9e0c4(void *p);
void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);
void FUN_0894a260(unsigned int *self);
void FUN_08a9e0cc(void *p);

void FUN_0886c924(unsigned int *param_1)
{
  int iVar1;
  int *piVar2;

  param_1[0] = (unsigned int)DAT_08bb03a0;
  if (*(unsigned char *)(param_1 + 11) != 0 && param_1[9] != 0) {
    FUN_08a9e0c4((void *)param_1[9]);
  }
  piVar2 = (int *)param_1[5];
  if (piVar2 != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar1 = piVar2[1] - 1;
    piVar2[1] = iVar1;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar1 == 0) {
      int *vt = (int *)piVar2[0];
      ((void (*)(int *))vt[2])(piVar2);
    }
  }
  param_1[0] = (unsigned int)DAT_08bb03d0;
  FUN_0894a260(param_1);
  FUN_08a9e0cc(param_1);
}
