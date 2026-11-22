/* LBPMATCH module=EBOOT.BIN start=0x0896e1e8 end=0x0896e2bc symbol=FUN_0896e1e8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { char pad0[8]; int state; short pad1[2]; short s16; short pad2; short s20; };
struct Self {
  int pad0;
  unsigned flags;
  char pad1[16];
  struct Rec recs[1];
};
struct Self2 {
  char pad0[76];
  struct Rec *cur;
  _Bool on;
  char pad1;
  short s82;
};
extern int FUN_0896d578(void *self, int c, struct Rec *rec, int a, int b, int s, int d);
extern void FUN_0896d7b8(void *self);

static inline void set_state(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)self + idx * 24;
  struct Rec *rec = (struct Rec *)(e + 24);
  unsigned f = self->flags;
  short sv;
  *(short *)(e + 40) = (short)d;
  rec->state = c;
  if ((f & 0x800) == 0) {
    struct Self2 *s2 = (struct Self2 *)self;
    sv = 0;
    if (s2->on != 0 && s2->cur == rec)
      sv = s2->s82;
    rec->s20 = FUN_0896d578(self, c, rec, 0, 4, sv, d);
    FUN_0896d7b8(self);
  }
}

void FUN_0896e1e8(struct Self *self, int idx, int c, int d)
{
  char *e = (char *)(idx * 24 + (int)self);
  struct Rec *rec = (struct Rec *)(e + 24);
  struct Self2 *s2 = (struct Self2 *)self;
  s2->s82 = (short)d;
  s2->on = c == 1;
  s2->cur = rec;
  if (c == 1) {
    if (*(int *)(e + 32) == 0)
      set_state(self, idx, 3, 0);
  }
  if ((self->flags & 0x800) == 0)
    FUN_0896d7b8(self);
}
// NEAR: 1 word differs (53/53): the state store goes through the record pointer (`sw v0,8(s1)`), retail through the shared base (`sw v0,32(t3)`). Sibling of wip/FUN_0896e114.c (same notes: _Bool on, set_state inlined with c = 3, d = 0, index-first address via an int cast).
// near miss from lane api-lbp-b-20261121 (unreviewed)
