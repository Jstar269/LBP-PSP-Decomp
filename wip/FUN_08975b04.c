/* LBPMATCH module=EBOOT.BIN start=0x08975b04 end=0x08975b94 symbol=FUN_08975b04 lang=c99 rodata=0x08bac1b8 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct In { char pad[12]; int a; int b; };
struct Self { int pad; struct In *in; float scale; };
extern float FUN_08b84224(float x);

unsigned FUN_08975b04(struct Self *self, int n, unsigned char flag1, unsigned char flag2)
{
  int a = self->in->a;
  unsigned v = a * n;
  if (flag1 != 0)
    v = v - (a - self->in->b);
  if (flag2 != 0)
    v = (unsigned)FUN_08b84224(self->scale * (float)(int)v);
  return v;
}
// NEAR: 6 words differ (36/36). Control flow, int arithmetic and the unsigned conversion (compare with 2^31, subtract, or 0x80000000) match; only FP register roles differ: retail keeps the converted count in $f0 and the scale in $f12 (mul.s $f12,$f0,$f12) and the 2^31 constant in $f1,
// ours puts the converted count in $f12, the scale in $f0 and the constant in $f12. Tried: operand order of the multiply, locals for scale and the converted value.
// near miss from lane api-lbp-b-20261121 (unreviewed)
