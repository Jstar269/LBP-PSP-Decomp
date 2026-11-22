/* LBPMATCH module=EBOOT.BIN start=0x08965b48 end=0x08965c20 symbol=FUN_08965b48 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int (**vtable)(struct Obj *);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
  char pad2[144];
  struct Target *t;
};
struct Target;
struct TVT { int pad[7]; void (*f1c)(struct Target *, void *, void *, int, int); };
struct Target { const struct TVT *vt; };
struct Holder { struct Obj *owner; struct Obj *obj; };
extern void *DAT_08be84a8;
extern struct Obj *FUN_08aaab30(void *pool, void *key);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_08965b2c(void);

void FUN_08965b48(struct Holder *h, void *key)
{
  struct Obj *o;
  struct Obj *old;
  h->owner = 0;
  h->obj = 0;
  o = FUN_08aaab30(DAT_08be84a8, key);
  if (o != 0) {
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
  }
  old = h->owner;
  h->owner = o;
  if (old != 0)
    FUN_088050b8(old);
  if (o != 0)
    FUN_088050b8(o);
  h->owner->t->vt->f1c(h->owner->t, FUN_08965b2c, h, 0, 1);
}
