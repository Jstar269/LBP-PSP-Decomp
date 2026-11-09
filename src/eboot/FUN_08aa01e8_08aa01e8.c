/* LBPMATCH module=EBOOT.BIN start=0x08aa01e8 end=0x08aa0264 symbol=FUN_08aa01e8 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction (external matching lane lbp2b); exact match verified under cc=gcc404xs-Os after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
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


float FUN_08aa01e8(float param_1,float param_2,uint param_3,uint param_4)
{
  if (param_3 < param_4) {
    param_2 = param_1 + ((param_2 - param_1) * (float)param_3) / (float)param_4;
  }
  return param_2;
}
