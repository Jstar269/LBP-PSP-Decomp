/* LBPMATCH module=EBOOT.BIN start=0x08a090f8 end=0x08a0911c symbol=FUN_08a090f8 lang=c99 */
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


float FUN_08a090f8(int param_1)
{
  if (*(int *)(param_1 + 0x44))
    return *(float *)(*(int *)(param_1 + 0x4c) * 4 + *(int *)(param_1 + 0x44) + 0x2c);
  return 0.0f;
}
// 9 differing words: corrected float return; zero initialization scheduling adds a nop.
// near miss from lane lbp5a (unreviewed)
