/* LBPMATCH module=EBOOT.BIN start=0x089720f8 end=0x089721d4 symbol=FUN_089720f8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Self {
  int pad0;
  void *a[2];
  void *b[164];
  char pad1[676 - 12 - 164 * 4];
  void *p676;
  char pad2[700 - 680];
  unsigned char flags;
};
extern void FUN_08970ce4(void) __attribute__((weak));
extern void FUN_088050b8(void *p);
extern void FUN_0897107c(struct Self *s);
extern void FUN_08975ab4(void *p);
extern void FUN_08a9e0cc(void *p);

void FUN_089720f8(struct Self *self)
{
  void *p;
  int i;
  p = self->p676;
  if (p != 0 && FUN_08970ce4) {
    self->p676 = 0;
    FUN_088050b8(p);
  }
  {
    void **q = self->b;
    i = 0;
    do {
      i = i + 1;
      p = *q;
      *q = 0;
      q = q + 1;
      if (p != 0)
        FUN_088050b8(p);
    } while (i != 164);
  }
  FUN_0897107c(self);
  self->flags = self->flags & 0xfe;
  for (i = 0; i != 2; i++) {
    p = self->a[i];
    if (p != 0) {
      FUN_08975ab4(p);
      FUN_08a9e0cc(p);
      self->a[i] = 0;
    }
  }
}
// NEAR: 23 words differ (55/55). Structure matches (weak function test `FUN_08970ce4` before the first delete, 164-slot clear loop, FUN_0897107c, flag clear via ins, two-slot delete loop).
// Retail allocates self in s1 with the loop pointer in s0 and the counters in s2/s3 (one `move s0,s1` at loop entry); ours puts self in s0 and the loop pointer in s1.
// Tried: indexed loops, explicit pointer do/while loop.
// near miss from lane api-lbp-b-20261121 (unreviewed)
