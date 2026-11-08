/* LBPMATCH module=EBOOT.BIN start=0x0895a500 end=0x0895a56c symbol=FUN_0895a500 lang=c99 cc=gcc404xs-Os */
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

extern undefined4 DAT_08befad0;
extern undefined DAT_08befb98;
extern undefined4 DAT_08befba0;
extern undefined4 FUN_08b8dfd8(char *param_1);
extern void FUN_08b8e050(undefined1 *param_1);
extern undefined4 FUN_08b9ca80(undefined4 param_1);
extern void FUN_0895a86c();

undefined4 * FUN_0895a500(void)
{
  int iVar1;
  
  if ((*(signed char *)&DAT_08befb98 == 0) && (iVar1 = FUN_08b8dfd8((char *)(&DAT_08befb98)), iVar1 != 0)) {
    DAT_08befad0 = &DAT_08befba0;
    DAT_08befba0 = 0;
    FUN_08b8e050((undefined1 *)(&DAT_08befb98));
    FUN_08b9ca80((undefined4)(FUN_0895a86c));
  }
  return &DAT_08befba0;
}

// residual: twin of 0x0895b184: retail `lb v1,-1128(v0)` vs ours `lbu v0` on DAT_08befb98; same global, same shape, same tie. parked.
// near miss from lane lbp2b (unreviewed)
// lane fb-near1 2026-11-02: signed char fixes the lb, but the load result goes to v0 (ours) vs v1 (retail); nested-if form identical. Seed + signed char both 2 words; parked.
