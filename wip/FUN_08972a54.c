/* LBPMATCH module=EBOOT.BIN start=0x08972a54 end=0x08972b38 symbol=FUN_08972a54 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int (**vtable)(struct Obj *);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
};
struct Self { char pad[12]; struct Obj *slots[164]; };
extern char *DAT_08bdb994[];
extern void *DAT_08be84a8;
extern struct Obj *FUN_08aaab30(void *pool, char *name);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_08977f24(void);

void FUN_08972a54(struct Self *self)
{
  int i;
  for (i = 0; i != 164; i++) {
    struct Obj *o;
    struct Obj *old;
    o = FUN_08aaab30(DAT_08be84a8, DAT_08bdb994[i]);
    if (o != 0) {
      FUN_08ad3088(o->lock);
      o->count = o->count + 1;
      FUN_08ad2810(o->lock);
      FUN_08ad3088(o->lock);
      o->count = o->count + 1;
      FUN_08ad2810(o->lock);
    }
    old = self->slots[i];
    self->slots[i] = o;
    if (old != 0)
      FUN_088050b8(old);
    if (o != 0)
      FUN_088050b8(o);
  }
  FUN_08977f24();
}
// NEAR: 55 words differ (54/57). Same pooled Ref assignment as 0x089726ec in a 164-iteration loop over a name table (DAT_08bdb994); retail keeps a separate counter (s3) next to the name pointer (s4) and the slot base (s2, incremented by 4),
// ours eliminates the counter and tests the name pointer against the end (3 words shorter). Tried: do/while with explicit pointers and counter, char* slot pointer, indexed for loop.
// near miss from lane api-lbp-b-20261121 (unreviewed)
