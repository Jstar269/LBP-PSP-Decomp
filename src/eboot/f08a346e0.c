/* LBPMATCH module=EBOOT.BIN start=0x08a346e0 end=0x08a34728 symbol=FUN_08a346e0 lang=c99 cc=gcc404xs-O2 */
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

extern undefined4 FUN_08a50fbc();
extern void FUN_08a53240();

void FUN_08a346e0(int *param_1,uchar param_2)
{
  int iVar1;
  if (param_2 != '\0') {
    void (*fn)(int *) = *(void (**)(int *))(*param_1 + 0x7c);
    fn(param_1);
  }
  iVar1 = FUN_08a50fbc();
  FUN_08a53240(iVar1,(int)param_1);
  return;
}
// 2 words differ: retail loads the vtable slot into v1 (lw v1,124(v0); jalr v1), ours reuses v0.
// tried: dropped-this fix applied (this passed), C++ class with 32 virtuals, local function pointer, intermediate vptr local: all give v0
// near miss from lane lbp3c (unreviewed)
// lane fb-near1 2026-11-02: triaged at the seed's profile (MISMATCH status, 2 differing words); newest residual: 08a346fc 8c42007c lw v0,124(v0) | 8c43007c lw v1,124(v0); seed kept as the best form, further matching not reached in this lane.
