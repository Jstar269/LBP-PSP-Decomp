/* LBPMATCH module=EBOOT.BIN start=0x08b8dd54 end=0x08b8dd9c symbol=FUN_08b8dd54 lang=c99 cc=gcc171-Os */
/* Hand-written reconstruction (external matching lane lbp2a); exact match verified under cc=gcc171-Os after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
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

extern int DAT_08be83fc;
extern void FUN_08baa16c();

void FUN_08b8dd54(void)
{
  int iVar1;
  
  for (iVar1 = DAT_08be83fc; iVar1 != 0; iVar1 = *(int *)(iVar1 + 4)) {
    FUN_08baa16c(*(int *)iVar1);
  }
  DAT_08be83fc = 0;
  return;
}
