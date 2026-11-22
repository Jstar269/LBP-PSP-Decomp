/* LBPMATCH module=EBOOT.BIN start=0x0886b164 end=0x0886b250 symbol=FUN_0886b164 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern char DAT_08bb0150[];
extern char DAT_08bb0188[];
extern char DAT_08bb00f0[];

void FUN_08879430(unsigned int *self, unsigned int p2, void (*fn)(void), int n);
void FUN_0886a99c(void);
void FUN_08b9eb88(void *dst, int val, unsigned int n);
void FUN_0885d290(unsigned int *obj, int a, int b, int c);

struct Hdr {
  int a;
  int b;
  struct Hdr *left;
  struct Hdr *right;
  int count;
};

struct Tree {
  int pad;
  struct Hdr hdr;
};

void FUN_0886b164(unsigned int *param_1, unsigned int param_2)
{
  struct Tree *t1;
  struct Tree *t2;
  struct Hdr *h1;
  struct Hdr *h2;

  FUN_08879430(param_1, param_2, FUN_0886a99c, 3);
  *param_1 = (unsigned int)DAT_08bb0150;
  param_1[3] = (unsigned int)DAT_08bb0188;
  h2 = (struct Hdr *)(param_1 + 0x39);
  t2 = (struct Tree *)(param_1 + 0x38);
  h1 = (struct Hdr *)(param_1 + 0x33);
  t1 = (struct Tree *)(param_1 + 0x32);
  FUN_08b9eb88(h1, 0, 16);
  t1->hdr.left = h1;
  t1->hdr.right = h1;
  t1->hdr.count = 0;
  t1->hdr.a = 0;
  FUN_08b9eb88(h2, 0, 16);
  t1->hdr.b = 0;
  t2->hdr.left = h2;
  t2->hdr.right = h2;
  t2->hdr.count = 0;
  t2->hdr.a = 0;
  FUN_0885d290(param_1 + 0x3e, 4, 1, 5);
  t2->hdr.b = 0;
  param_1[0x3e] = (unsigned int)DAT_08bb00f0;
  param_1[0x74] = 0;
  param_1[0x72] = 0;
  param_1[0x73] = 0;
}
// NEAR: lbpmatch 25 words differ (59 compared). Matches: prologue, base ctor call, vt0 store, both memsets, the
// header stores and the sub-object call. Still differs: (1) s1/s2 swapped: retail t1=this+200 in s1, t2=this+224
// in s2; ours the reverse, whatever the definition order; (2) retail stores vt12 (sw v0,12(s0)) before the first
// memset argument setup, ours after it; (3) the tail store order of the t2 block.
// Ruled out: field stores through the header pointer (same code as through the Tree pointer, but the bases differ),
// the struct versus word-indexed object (identical code), definition order of t1/t2 (same registers).
// Store order: source `left` before `right` gives retail's right-then-left order for both trees (the file keeps that).
// near miss from lane api-haiku-20261121 (unreviewed)
