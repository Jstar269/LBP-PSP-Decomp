/* LBPMATCH module=EBOOT.BIN start=0x089733b4 end=0x08973468 symbol=FUN_089733b4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int (**vtable)(struct Obj *);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
};
struct Item { int pad[3]; int kind; };
struct Holder { struct Item *item; };
extern void *DAT_08be84a8;
extern void *DAT_08be8600;
extern struct Obj *FUN_08aaa70c(void *pool, void *key);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_089e584c(void *a, struct Obj **r);

int FUN_089733b4(struct Holder *h)
{
  void *g;
  int r = 0;
  void *key = (char *)h->item + 4;
  if (h->item->kind == 21) {
    struct Obj *o;
    g = DAT_08be8600;
    o = FUN_08aaa70c(DAT_08be84a8, key);
    if (o != 0) {
      FUN_08ad3088(o->lock);
      o->count = o->count + 1;
      FUN_08ad2810(o->lock);
    }
    {
      struct Obj *ref = o;
      FUN_089e584c(g, &ref);
      r = 1;
      if (ref != 0) {
        FUN_088050b8(ref);
        r = 1;
      }
    }
  }
  return r;
}
