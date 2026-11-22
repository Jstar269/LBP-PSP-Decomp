/* LBPMATCH module=EBOOT.BIN start=0x0897f4b0 end=0x0897f590 symbol=FUN_0897f4b0 lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Self {
  char pad0[4];
  int f4;
  int f8;
  unsigned char flag;
  char pad1[7];
  int f20;
  char pad2[4];
  float v28[1];
  char pad3[12];
  int f44;
  int f48;
  char pad4[12];
  float v64[1];
  char pad5[8];
  float v76[1];
  char pad6[16];
  int f96;
  char pad7[4];
  float v104[1];
  char pad8[56];
  unsigned char off;
};
extern int *FUN_089f683c(void);
extern void FUN_0897f268(float s, void *self, int a, int b, void *v);
extern void FUN_0897ce50(void *self);

void FUN_0897f4b0(struct Self *self)
{
  int *p;
  float s;
  if (self->off == 0) {
    p = FUN_089f683c();
    s = 1.0f;
    {
      int n = *p;
      self->flag = 1 < n;
    }
    FUN_0897f268(s, self, 1, self->f48, self->v64);
    if (self->f4 < self->f8) {
      FUN_0897f268(s, self, 1, self->f20, self->v28);
      FUN_0897f268(s, self, 1, self->f44, self->v76);
    }
    if (self->flag != 0)
      FUN_0897f268(s, self, 1, self->f96, self->v104);
    FUN_0897ce50(self);
  }
}
// NEAR: 10 words differ (56/56): scheduling only. Retail hoists the 1.0f constant load (`lui v1; lwc1 $f20`) above the load of *FUN_089f683c() and places `lw a2,44(s0)` after the `mov.s $f12,$f20` of the third call; ours loads *p first and the argument load before the mov.s.
// Tried: a local float s = 1.0f at several places, a local int for *p.
// near miss from lane api-lbp-b-20261121 (unreviewed)
