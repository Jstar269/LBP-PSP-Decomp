/* LBPMATCH module=EBOOT.BIN start=0x08a7735c end=0x08a77410 symbol=FUN_08a7735c lang=c99  cc=gcc404xs-O2 */
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

extern undefined4 * FUN_08a10f1c();
extern void FUN_08a12ec0();
extern undefined4 FUN_08a50fbc();
extern undefined4 FUN_08a52b38();
extern void FUN_08a7717c();
extern void FUN_08a9a5c8();
extern void FUN_08a7781c();

void FUN_08a7735c(int param_1)
{
  int iVar1;
  int *piVar2;
  int *piVar3;
  undefined4 *puVar4;
  
  FUN_08a9a5c8((int *)param_1);
  iVar1 = FUN_08a50fbc();
  piVar2 = (int *)FUN_08a52b38(iVar1,*(uint *)(param_1 + 0x19c));
  iVar1 = FUN_08a50fbc();
  piVar3 = (int *)FUN_08a52b38(iVar1,*(uint *)(param_1 + 0x1a0));
  puVar4 = FUN_08a10f1c();
  FUN_08a12ec0((int)puVar4,piVar2,piVar3,*(int **)(param_1 + 400),param_1,-1);
  FUN_08a7717c((undefined4 *)(param_1 + 0x180),(undefined4 *)(param_1 + 0x184),(undefined4 *)(param_1 + 0x188),(int)param_1 + 396);
  iVar1 = (**(code **)(**(int **)(param_1 + 400) + 0x24))(*(int **)(param_1 + 400));
  *(code **)(iVar1 + 0x38) = FUN_08a7781c;
  return;
}


// Two differing words remain in virtual-call register allocation; adding the receiver and using a C++ virtual call both leave v0 where retail uses v1.
// near miss from lane lbp2a (unreviewed)
// lane fb-near1 2026-11-02: triaged at the seed's profile (MISMATCH status, 2 differing words); newest residual: 08a773dc 8c820000 lw v0,0(a0) | 8c830000 lw v1,0(a0); seed kept as the best form, further matching not reached in this lane.
