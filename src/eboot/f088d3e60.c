/* LBPMATCH module=EBOOT.BIN start=0x088d3e60 end=0x088d3ef0 symbol=FUN_088d3e60 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction (external matching lane); exact match verified; names and types await human review. */
typedef unsigned char undefined;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulonglong;
typedef long long longlong;
typedef unsigned char uchar;
typedef unsigned char bool;
typedef unsigned long long undefined8;
typedef int code();
#define true 1
#define false 0
#define ABS(x) ((x) < 0 ? -(x) : (x))
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

extern undefined4 DAT_08be84a0;
extern undefined4 FUN_08adbb84();
extern void FUN_08b88fac();

void FUN_088d3e60(int param_1,undefined4 *param_2)
{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  
  *(int *)param_1 = param_1;
  puVar3 = (undefined4 *)*param_2;
  *(int *)(param_1 + 4) = param_1;
  for (; puVar3 != param_2; puVar3 = (undefined4 *)*puVar3) {
    piVar1 = (int *)FUN_08adbb84(DAT_08be84a0,0xc);
    char *end = (char *)piVar1 + 8;
    if (end != 0) {
      iVar2 = puVar3[2];
      piVar1[2] = iVar2;
      if (iVar2 != 0) {
        *(int *)(iVar2 + 4) = *(int *)(iVar2 + 4) + 1;
      }
    }
    FUN_08b88fac(piVar1,param_1);
  }
  return;
}


// Two differences in comparison against pointer plus eight; unsigned arithmetic also canonicalizes to compare against minus eight.
// near miss from lane lbp2a (unreviewed)
// lane fb-near1 2026-11-02: triaged at the seed's profile (MISMATCH status, 2 differing words); newest residual: 088d3ea0 2402fff8 li v0,-8 | 24420008 addiu v0,v0,8; seed kept as the best form, further matching not reached in this lane.
