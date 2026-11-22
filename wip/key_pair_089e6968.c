/* LBPMATCH module=EBOOT.BIN start=0x089e6968 end=0x089e6a00 symbol=FUN_089e6968 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Pair { void *p; int v; };
extern void *DAT_08bdb7f8;
extern void *DAT_08bdbd2c;
extern void *DAT_08bdbd30;
extern int FUN_08a9b8a4(struct Pair *out, void *p);
extern void FUN_08a9c3ac(void *obj, struct Pair *k);
extern void FUN_08a9c604(void *obj, struct Pair *k);

void FUN_089e6968(int flag, void *obj)
{
  struct Pair k1;
  struct Pair k2;
  void *p = DAT_08bdb7f8;
  void *q;
  int v;
  v = FUN_08a9b8a4(&k1, p);
  k1.p = p;
  k1.v = v;
  FUN_08a9c3ac(obj, &k1);
  q = flag == 0 ? DAT_08bdbd30 : DAT_08bdbd2c;
  v = FUN_08a9b8a4(&k2, q);
  k2.p = q;
  k2.v = v;
  FUN_08a9c604(obj, &k2);
}
// NEAR 2 words: the first pair store order only. Retail: `sw s0,0(sp); jal 0x8a9c3ac; sw v0,4(sp)` (p stored first, call result in the delay slot); ours: `sw v0,4(sp); ...; jal; sw s0,0(sp)`. The identical source shape for the second pair (k2) matches, and src/eboot/slot_update_089e67b8.c has the same shape matching.
// Tried: source order of the two stores, block-scoped v, separate v/w, declaration order of k1/k2, a copy of obj.
// near miss from lane api-lbp-d-20261121 (unreviewed)
