/* LBPMATCH module=EBOOT.BIN start=0x0896dbfc end=0x0896dca0 symbol=FUN_0896dbfc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { char pad0[8]; int c; short pad1[2]; short e; };
struct Self {
  int pad0;
  unsigned flags;
  char pad1[16];
  struct Rec recs[1];
};
struct Self2 {
  char pad0[76];
  struct Rec *cur;
  unsigned char on;
  char pad1;
  short s82;
};
extern int FUN_0896d578(void *self, int c, struct Rec *rec, int a, int b, int s, int d);
extern void FUN_0896d7b8(void *self);

void FUN_0896dbfc(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)self + idx * 24;
  struct Rec *rec = (struct Rec *)(e + 24);
  unsigned f = self->flags;
  short sv;
  *(short *)(e + 40) = (short)d;
  *(int *)(e + 32) = c;
  if ((f & 0x800) == 0) {
    struct Self2 *s2 = (struct Self2 *)self;
    sv = 0;
    if (s2->on != 0 && s2->cur == rec)
      sv = s2->s82;
    rec->e = FUN_0896d578(self, c, rec, 0, 4, sv, d);
    FUN_0896d7b8(self);
  }
}
// NEAR: 34 words differ (40/41). Sibling of wip/FUN_0896db58.c (same function with other field offsets and call constants 0, 4); same register-role difference (retail puts e in a0 and reads through the s0 copy of self).
// near miss from lane api-lbp-b-20261121 (unreviewed)
