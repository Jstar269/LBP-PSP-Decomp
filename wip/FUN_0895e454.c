/* LBPMATCH module=EBOOT.BIN start=0x0895e454 end=0x0895e530 symbol=FUN_0895e454 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct P2 { int a; int b; };
struct Obj {
  int (**vtable)(struct Obj *, void *, void *, int, int);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
};
struct Src {
  int pad0[4];
  struct P2 pr;
  int pad1[4];
  int f40;
  int f44;
  int pad2[1];
  int f52;
};
struct Holder {
  struct Src *key;
};
extern void *DAT_08be84a8;
extern struct Obj *FUN_08aaaa80(void *pool, void *key);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(struct Obj *o);

void FUN_0895e454(struct Holder *h)
{
  struct P2 p;
  int q;
  struct Obj *o = FUN_08aaaa80(DAT_08be84a8, (char *)h->key + 16);
  if (o != 0) {
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
    FUN_088050b8(o);
  }
  int (*fn)(struct Obj *, void *, void *, int, int) = o->vtable[6];
  struct Src *k = h->key;
  q = k->f44;
  p = k->pr;
  int z = k->f52 == 0;
  fn(o, &p, &q, k->f40, z);
  FUN_088050b8(o);
}
// NEAR: 2 words differ (lw v0,24(t2) vtable-slot load and lw a3,40(v1) argument load swapped; retail loads the slot first).
// Tried: local key pointer, local fn pointer before/after the field loads, q before p, struct assignment p = k->pr (needed to get the paired loads), z local.
// near miss from lane api-lbp-b-20261121 (unreviewed)
