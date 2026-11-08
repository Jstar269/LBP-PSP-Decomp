/* LBPMATCH module=EBOOT.BIN start=0x089af4e4 end=0x089af564 symbol=FUN_089af4e4 lang=c99 cc=gcc404xs-Os */
/* Clone of src/eboot/f0880f9b0.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
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

extern undefined4 DAT_08be84a0;
extern undefined4 FUN_08adbb84();
extern void FUN_08b88fac();

void FUN_089af4e4(int param_1,undefined4 *param_2)
{
  int *piVar1;
  undefined4 *puVar2;
  
  *(int *)param_1 = param_1;
  puVar2 = (undefined4 *)*param_2;
  *(int *)(param_1 + 4) = param_1;
  for (; puVar2 != param_2; puVar2 = (undefined4 *)*puVar2) {
    piVar1 = (int *)FUN_08adbb84(DAT_08be84a0,0xc);
    char *end = (char *)piVar1 + 8;
    if (end != 0) {
      piVar1[2] = puVar2[2];
    }
    FUN_08b88fac(piVar1,param_1);
  }
  return;
}

// Primary profile initial candidate: 2 differing words; disassembly and diff retained in try/lane-work.
// near miss from lane lbp1b (unreviewed)
// lane fb-near1 2026-11-02: retail keeps addiu v0,v0,8; beqz v0 for the (ptr+8)!=0 test; gcc404xs folds it to li v0,-8; beq v1,v0. Compiler fold difference, no spellable source form. Parked.
