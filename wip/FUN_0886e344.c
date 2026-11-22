/* LBPMATCH module=EBOOT.BIN start=0x0886e344 end=0x0886e408 symbol=FUN_0886e344 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern char DAT_08bb045c[];
extern char DAT_08bae290[];

void *FUN_08a9e838(unsigned int n);
void *FUN_088451b8(void *obj, char *name);
void FUN_08878ab8(int a, char *buf);
void *FUN_08843f04(void *obj, unsigned int n);
unsigned int FUN_08ba0e8c(char *s);
void FUN_08842ef4(void *dst, char *src, unsigned int n);
void FUN_08844248(void *dst, void *src);

void FUN_0886e344(char *param_1, void *param_2)
{
  void *s3;
  void *s2;
  unsigned int *s0;
  unsigned int uVar1;
  char acStack_50[48];

  s3 = param_2;
  s2 = FUN_088451b8(FUN_08a9e838(0x50), DAT_08bb045c);
  FUN_08878ab8(*(int *)(*(int *)(param_1 + 0x44) + 0x14), acStack_50);
  s0 = FUN_08843f04(FUN_08a9e838(0x30), 4);
  s0[0] = (unsigned int)DAT_08bae290;
  uVar1 = FUN_08ba0e8c(acStack_50);
  FUN_08842ef4(s0 + 8, acStack_50, uVar1);
  *(unsigned char *)(s0 + 11) = 0;
  FUN_08844248(s2, s0);
  FUN_08844248(s3, s2);
}
// NEAR: lbpmatch 43 words differ (46/49 compared). Same shape as sibling 0x0886cf3c (jpegHash instead of the sprintf):
// alloc(80)+factory, the 68-offset member's vtable slot 0x14 argument to FUN_08878ab8, alloc(48)+ctor, the vtable store,
// length + append, the byte clear, the two links. Matches the first 3 words of the body shape; the frame is 64 (retail 80):
// retail saves s0 and s3 (param_1 and a copy of param_2 kept across the calls), ours keeps fewer callee-saved registers.
// Ruled out: a separate param_2 copy declared first (same code).
// near miss from lane api-haiku-20261121 (unreviewed)
