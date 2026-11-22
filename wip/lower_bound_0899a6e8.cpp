/* LBPMATCH module=EBOOT.BIN start=0x0899a6e8 end=0x0899a7a8 symbol=FUN_0899a6e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct E {
  unsigned a;
  unsigned b;
  int c;
  int d;
  short e;
  short f;
};

struct Tab {
  int pad;
  int count;
  int off;
};

extern "C" bool FUN_0899a3c4(const E *e, const E *key) __attribute__((pure));

extern "C" E *FUN_0899a6e8(Tab *t, unsigned a, unsigned b)
{
  E key;
  key.a = a;
  key.b = b;
  key.c = -1;
  key.e = -1;
  key.f = -1;
  E *first = (E *)((char *)t + t->off);
  E *last = first + t->count;
  int len = last - first;
  while (len > 0) {
    int half = len >> 1;
    E *mid = first + half;
    if (FUN_0899a3c4(mid, &key)) {
      first = mid + 1;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}
// NEAR 52 words (53/48): the std::lower_bound shape (20-byte elements, key struct with c=-1 and two shorts -1, comp call 0x0899a3c4)
// compiles, but the exact pointer-difference division `(count*20 >> 2) * -0x33333333` is expanded by our compiler as an 8-insn
// shift/add chain where retail uses `lui/ori 0xcccccccd; mult; mflo`: a multiply-by-constant cost difference, not expressible in C.
// Loop shape also differs (retail's second iteration head `sra s0,s1,1` follows a branch-likely `move s1,s0`).
// near miss from lane api-lbp-c-20261121 (unreviewed)
