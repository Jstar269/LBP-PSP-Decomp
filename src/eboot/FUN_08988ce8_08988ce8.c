/* LBPMATCH module=EBOOT.BIN start=0x08988ce8 end=0x08988e00 symbol=FUN_08988ce8 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly and Ghidra pseudocode (lane lbp2b, DeepSeek); declaration order found by lbpnearperm.py tag np85c; names and types await human review. */
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

extern void FUN_08970b6c();
extern undefined4 * FUN_08981e68();
extern void FUN_089dcfac();

void FUN_08988ce8(int param_1)
{
  undefined4 *puVar1;
  int iVar2;
  code *pcVar3;
  int *piVar4;
  undefined4 *local_28 [2];
  undefined4 *local_30;
  
  undefined4 local_2c;
  local_2c = 0xffffffff;
  FUN_08981e68(local_28,0x1399,&local_2c);
  puVar1 = local_28[0];
  if (local_28[0] != (undefined4 *)0x0) {
    *(short *)((int)local_28[0] + 6) = *(short *)((int)local_28[0] + 6) + 1;
  }
  piVar4 = *(int **)(param_1 + 8);
  for (; piVar4 != (int *)0x0; piVar4 = (int *)piVar4[2]) {
    local_30 = puVar1;
    pcVar3 = *(code **)(*piVar4 + 8);
    if (puVar1 != (undefined4 *)0x0) {
      *(short *)((int)puVar1 + 6) = *(short *)((int)puVar1 + 6) + 1;
    }
    iVar2 = (*pcVar3)(piVar4,&local_30);
    if (local_30 != (undefined4 *)0x0) {
      FUN_08970b6c(local_30);
    }
    if (iVar2 == 0) break;
  }
  if (puVar1 != (undefined4 *)0x0) {
    FUN_08970b6c(puVar1);
  }
  iVar2 = *(int *)(param_1 + 0x14);
  FUN_089dcfac(*(int *)(*(int *)((3 - *(short *)(iVar2 + 0x94)) * 4 + *(int *)(iVar2 + 0x90)) + 0x3c
                       ),local_28[0][3]);
  if (local_28[0] != (undefined4 *)0x0) {
    FUN_08970b6c(local_28[0]);
  }
  return;
}
