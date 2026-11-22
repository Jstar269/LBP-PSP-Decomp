/* LBPMATCH module=EBOOT.BIN start=0x088686bc end=0x0886875c symbol=FUN_088686bc lang=c99 cc=gcc404xs-Os rodata=0x08bb00cc */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bb0038;

void FUN_0885d290(unsigned int *self, int a, int b, int c);
void FUN_08b9eb88(unsigned int *p, int v, unsigned int n);
char *FUN_08887624(void);
int FUN_08ba0868(char *buf, int n, const char *fmt, char *s);
void FUN_0885d244(unsigned int *self);

void FUN_088686bc(unsigned int *param_1)
{
  unsigned int *base;
  unsigned int *puVar2;
  char *pcVar1;
  char auStack_110[256];

  puVar2 = param_1 + 0x35;
  FUN_0885d290(param_1, 3, 0, 5);
  *param_1 = (unsigned int)&DAT_08bb0038;
  base = param_1 + 0x34;
  FUN_08b9eb88(puVar2, 0, 16);
  base[4] = (unsigned int)puVar2;
  base[3] = (unsigned int)puVar2;
  base[5] = 0;
  base[1] = 0;
  base[2] = 0;
  pcVar1 = FUN_08887624();
  FUN_08ba0868(auStack_110, 0x100, "%s/filter/batch", pcVar1);
  FUN_0885d244(param_1);
  return;
}
// 35 of 40 words differ (lbpmatch 36/40). Retail keeps `this` in s2 and the field base (this+208) in s0 as separate
// registers; ours folds the base into `this` (one register, s0 = this), so the frame and all later offsets differ.
// Tried: base before/after the vtable store, base as a separate local, rodata=0x08bb00cc (the string), vtable as
// DAT_08bb0038 (the vtable sits there; it links without a symbol line). Next idea: make base not derivable from this.
// near miss from lane api-haiku-20261121 (unreviewed)
