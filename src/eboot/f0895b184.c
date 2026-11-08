/* LBPMATCH module=EBOOT.BIN start=0x0895b184 end=0x0895b1fc symbol=FUN_0895b184 lang=c99 cc=gcc404xs-Os */
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

extern undefined DAT_08befb90;
extern undefined DAT_08befbb0;
extern undefined4 DAT_08befc04;
extern void FUN_0895945c(int param_1,undefined4 param_2);
extern void FUN_0895a8c4(int param_1);
extern undefined4 FUN_08b8dfd8(char *param_1);
extern void FUN_08b8e050(undefined1 *param_1);
extern undefined4 FUN_08b9ca80(undefined4 param_1);
extern void FUN_0895af50();

void FUN_0895b184(undefined4 param_1)
{
  int iVar1;
  
  if (((*(signed char *)&DAT_08befb90) == 0) && (iVar1 = FUN_08b8dfd8((char *)(&DAT_08befb90)), iVar1 != 0)) {
    FUN_0895a8c4((int)((int)&DAT_08befbb0));
    FUN_08b8e050((undefined1 *)(&DAT_08befb90));
    FUN_08b9ca80((undefined4)(FUN_0895af50));
  }
  FUN_0895945c((int)(DAT_08befc04),(undefined4)(param_1));
  return;
}

// residual: 2 words - retail `lb v1,-1136(v0)` (signed byte load, dest v1) vs ours `lbu v0,...` (unsigned, dest v0)
// tried: extern char (sign right, but GCC then anchors s0=&g and the whole address/colour layout changes: 28 differ),
//        (char) cast in the condition (front end folds it back to lbu), comparison against 0 vs '\0'.
// conclusion: the load width and the base-register choice move together; no spelling keeps the undefined-type
//   address form while making the load signed -> allocator/type tie, parked.
// near miss from lane lbp2b (unreviewed)
// lane fb-near1 2026-11-02: triaged at the seed's profile (MISMATCH status, 2 differing words); newest residual: 0895b1a0 9042fb90 lbu v0,-1136(v0) | 8043fb90 lb v1,-1136(v0); seed kept as the best form, further matching not reached in this lane.
