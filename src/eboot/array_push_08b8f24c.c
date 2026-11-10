/* LBPMATCH module=EBOOT.BIN start=0x08b8f24c end=0x08b8f288 symbol=FUN_08b8f24c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp4d, Claude Sonnet 5.5); statement order found by lbpnearperm.py tag np85c; names and types await human review. */
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


int FUN_08b8f24c(int param_1)
{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0x14);
  iVar2 = 0;
  if (iVar1 < *(int *)(param_1 + 0x18)) {
    iVar2 = *(int *)(param_1 + 0x10) + iVar1 * 0xc;
    *(int *)(param_1 + 0x14) = iVar1 + 1;
  }
  return iVar2;
}
