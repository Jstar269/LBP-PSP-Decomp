/* LBPMATCH module=EBOOT.BIN start=0x0899aad4 end=0x0899ab64 symbol=FUN_0899aad4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Bits {
  char pad0[8];
  int off;
  unsigned char *mask;
  char pad1[2];
  unsigned short count;
};

void FUN_0899aad4(struct Bits *b, short *out)
{
  int i;
  int q;
  int r;
  unsigned char *p;

  for (i = 0; q = i / 8, p = (unsigned char *)b + q, i < (int)b->count; i++) {
    int off = b->off;
    unsigned char *m = b->mask;
    int b1, b2;
    r = i % 8;
    b1 = (p[off] >> r) & 1;
    b2 = (m[q] >> r) & 1;
    if (b1 != 0 && b2 != 0) {
      *out = (short)i;
      out++;
    }
  }
}
// NEAR 37 words (37/36): same instruction mix and loop rotation as retail (header: q = i/8, p = b+q, i < count; body: r = i%8,
// two bit tests, store short i), but retail's registers are b=t0, i=a3, r=a2, q=t1 where ours are b=a3, i=a2, q=t0 and one extra word.
// The header comma expression `q = i / 8, p = b + q, i < n` is what gives the rotated loop (a plain for/break gives the test first).
// near miss from lane api-lbp-c-20261121 (unreviewed)
