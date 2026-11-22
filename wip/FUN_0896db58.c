/* LBPMATCH module=EBOOT.BIN start=0x0896db58 end=0x0896dbfc symbol=FUN_0896db58 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { char pad0[12]; int c; short pad1; short d; short pad2; short e; };
struct Self {
  int pad0;
  unsigned flags;
  char pad1[16];
  struct Rec recs[1];
};
struct Self2 {
  char pad0[84];
  struct Rec *cur;
  unsigned char on;
  char pad1;
  short s90;
};
extern int FUN_0896d578(void *self, int c, struct Rec *rec, int a, int b, int s, int d);
extern void FUN_0896d7b8(void *self);

void FUN_0896db58(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)self + idx * 24;
  struct Rec *rec = (struct Rec *)(e + 24);
  unsigned f = self->flags;
  short sv;
  *(short *)(e + 42) = (short)d;
  *(int *)(e + 36) = c;
  if ((f & 0x800) == 0) {
    struct Self2 *s2 = (struct Self2 *)self;
    sv = 0;
    if (s2->on != 0 && s2->cur == rec)
      sv = s2->s90;
    rec->e = FUN_0896d578(self, c, rec, 2, 6, sv, d);
    FUN_0896d7b8(self);
  }
}
// NEAR: 34 words differ (40/41, retail has one more word). Retail computes e = self + idx*24 into a0 itself (`addu a0,v0,a0; move v0,a0`) after copying self to s0 and reads the later fields through s0;
// ours puts e in v0 and keeps reading through a0. `d` must be an int parameter (retail passes a3 on without seh).
// Tried: Rec array index, char*-based address with explicit offsets, short vs int d.
// near miss from lane api-lbp-b-20261121 (unreviewed)
