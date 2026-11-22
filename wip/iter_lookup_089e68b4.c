/* LBPMATCH module=EBOOT.BIN start=0x089e68b4 end=0x089e6968 symbol=FUN_089e68b4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Pair { void *p; int v; };
struct Obj24 { char d[24]; };
struct Obj32 { char d[24]; long long it; };
extern void *DAT_08bdbd04[];
extern void FUN_08a9c164(struct Obj24 *o);
extern int FUN_08a9b8a4(struct Pair *out, void *p);
extern long long FUN_08a9d6a8(struct Obj24 *o, struct Pair *k);
extern void FUN_08a9c304(struct Obj32 *o, long long *it);
extern int FUN_08a9d438(struct Obj32 *o, int n);
extern void FUN_08a9bff8(void *o);

int FUN_089e68b4(int idx)
{
  struct Pair key;
  struct Obj24 a;
  struct Obj32 b;
  long long tmp;
  void *p;
  int v, res;
  long long r;
  FUN_08a9c164(&a);
  p = DAT_08bdbd04[idx];
  v = FUN_08a9b8a4(&key, p);
  key.p = p;
  key.v = v;
  r = FUN_08a9d6a8(&a, &key);
  b.it = r;
  tmp = r;
  FUN_08a9c304(&b, &tmp);
  res = FUN_08a9d438(&b, -1);
  FUN_08a9bff8(&b);
  FUN_08a9bff8(&a);
  return res;
}
// NEAR 9 words: only the s0/s1 roles of p (table entry) and &b (the 32-byte local) are swapped; all stores, calls and the long long return (v0:v1) match.
// Tried: store order of tmp/b.it (fixed). Not tried: reordering declarations (stack layout stays).
// near miss from lane api-lbp-d-20261121 (unreviewed)
