/* LBPMATCH module=EBOOT.BIN start=0x0896e114 end=0x0896e1e8 symbol=FUN_0896e114 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { char pad0[12]; int state; short pad1; short s18; short pad2; short s22; };
struct Self {
  int pad0;
  unsigned flags;
  char pad1[16];
  struct Rec recs[1];
};
struct Self2 {
  char pad0[84];
  struct Rec *cur;
  _Bool on;
  char pad1;
  short s90;
};
extern int FUN_0896d578(void *self, int c, struct Rec *rec, int a, int b, int s, int d);
extern void FUN_0896d7b8(void *self);

static inline void set_state(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)self + idx * 24;
  struct Rec *rec = (struct Rec *)(e + 24);
  unsigned f = self->flags;
  short sv;
  *(short *)(e + 42) = (short)d;
  rec->state = c;
  if ((f & 0x800) == 0) {
    struct Self2 *s2 = (struct Self2 *)self;
    sv = 0;
    if (s2->on != 0 && s2->cur == rec)
      sv = s2->s90;
    rec->s22 = FUN_0896d578(self, c, rec, 2, 6, sv, d);
    FUN_0896d7b8(self);
  }
}

void FUN_0896e114(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)(idx * 24 + (int)self);
  struct Rec *rec = (struct Rec *)(e + 24);
  struct Self2 *s2 = (struct Self2 *)self;
  s2->s90 = (short)d;
  s2->on = c == 1;
  s2->cur = rec;
  if (c == 1) {
    if (*(int *)(e + 36) == 0)
      set_state(self, idx, 3, 0);
  }
  if ((self->flags & 0x800) == 0)
    FUN_0896d7b8(self);
}
// NEAR: 1 word differs (53/53): the state store comes out `sw v0,12(s1)` (through the record pointer), retail `sw v0,36(t3)` (through the shared self + idx*24 base).
// Structure: the second function inlines set_state (the previous 0x0896db58 shape) with c = 3, d = 0; `on` must be a _Bool (type-based aliasing lets the inlined test reuse the stored value);
// the address `idx * 24 + (int)self` (index-first addu) is a cast idiom. With the state stored as *(int *)(e + 36) the sh/sw pair swaps (2 words).
// near miss from lane api-lbp-b-20261121 (unreviewed)
