/* LBPMATCH module=EBOOT.BIN start=0x08882928 end=0x08882934 symbol=FUN_08882928 lang=c99 cc=gcc171-O2 */
/* Clone of hst-decomp src/eboot/FUN_00070aa4.c (lbpxclone.py tag x1): addresses substituted; exact match verified; names and types await human review. */
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

extern undefined4 DAT_08bdb1f4;

void FUN_08882928(undefined4 param_1)
{
  DAT_08bdb1f4 = param_1;
  return;
}
