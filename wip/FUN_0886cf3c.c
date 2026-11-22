/* LBPMATCH module=EBOOT.BIN start=0x0886cf3c end=0x0886d008 symbol=FUN_0886cf3c lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern char DAT_08bb0430[];
extern char DAT_08bbef90[];
extern char DAT_08bae290[];

void *FUN_08a9e838(unsigned int n);
void *FUN_088451b8(void *obj, char *name);
void FUN_08ba0868(char *buf, unsigned int size, char *fmt, ...);
void *FUN_08843f04(void *obj, unsigned int n);
unsigned int FUN_08ba0e8c(char *s);
void FUN_08842ef4(void *dst, char *src, unsigned int n);
void FUN_08844248(void *dst, void *src);

void FUN_0886cf3c(char *param_1, void *param_2)
{
  char acStack_60[64];
  void *s2;
  unsigned int *s0;
  unsigned int uVar1;

  s2 = FUN_088451b8(FUN_08a9e838(0x50), DAT_08bb0430);
  FUN_08ba0868(acStack_60, 0x40, DAT_08bbef90, *(unsigned int *)(param_1 + 0x3c));
  s0 = FUN_08843f04(FUN_08a9e838(0x30), 4);
  s0[0] = (unsigned int)DAT_08bae290;
  uVar1 = FUN_08ba0e8c(acStack_60);
  FUN_08842ef4(s0 + 8, acStack_60, uVar1);
  *(unsigned char *)(s0 + 11) = 0;
  FUN_08844248(s2, s0);
  FUN_08844248(param_2, s2);
}
// NEAR: lbpmatch 45 words differ (48/51). Matches: the allocator/ctor call sequence, the string symbols, the vtable store.
// Still differs: the frame (retail 96, ours 80: Ghidra passes in_t0..in_t3 to the variadic FUN_08ba0868, so the caller
// reserves the 16-byte argument area that our variadic declaration does not), saved-register roles (retail s3 = param_2).
// Not tried: an explicit 8-argument variadic call (would need the in_t0..in_t3 stores retail does not make).
// near miss from lane api-haiku-20261121 (unreviewed)
