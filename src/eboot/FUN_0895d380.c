/* LBPMATCH module=EBOOT.BIN start=0x0895d380 end=0x0895d3a4 symbol=FUN_0895d380 lang=c99 cc=gcc402x-O1 */
/* Hand-written reconstruction (external matching lane lbp5a); exact match verified under cc=gcc402x-O1 after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
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


void FUN_0895d380(int param_1,uchar param_2)
{
  int p = *(int *)(param_1 + 0x44);
  *(uchar *)(param_1 + 0x33) = param_2;
  if (param_2 != '\0') {
    *(undefined4 *)(p + 0xb4) = 0x60000000;
    return;
  }
  *(undefined4 *)(p + 0xb4) = 0;
  return;
}
