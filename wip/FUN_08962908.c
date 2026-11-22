/* LBPMATCH module=EBOOT.BIN start=0x08962908 end=0x089629e8 symbol=FUN_08962908 lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct In { char pad0[12]; short s[4]; void *obj; };
struct Self { int pad; struct In *in; float f8; };
extern const float DAT_08bb880c;
extern void *FUN_08a9e838(int n);
extern void FUN_089e1b90(float a, float b, void *o, int c, short *s);
extern void FUN_089e1a14(float a, float b, void *o, int c, short *s);

void FUN_08962908(float x, struct Self *self)
{
  float y = x * DAT_08bb880c;
  struct In *in = self->in;
  short *sp = in->s;
  if (in->obj != 0) {
    FUN_089e1a14(1.0f, y, in->obj, 2, sp);
  } else {
    void *o = FUN_08a9e838(80);
    FUN_089e1b90(1.0f, y, o, 2, sp);
    in->obj = o;
  }
  if (0.9f <= x)
    self->f8 = 1.0f;
}
// NEAR: 18 words differ (56/56). Retail hoists the 225.0 constant's lui (DAT_08bb880c, a different rodata block, declared extern here) above the s4 save, loads $f0 before `lw s1,4(a0)`,
// moves `a2` before the branch and sets $f13 before $f12 in both arms (ours f12 first, a2 inside the arms). Layout (non-null arm falls through) matches.
// Tried: obj == 0 arm first (46 differ), y as local before/after `in`, y inline at both calls (41), short *sp local, gcc404xs-O2 (54).
// near miss from lane api-lbp-b-20261121 (unreviewed)
