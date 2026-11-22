/* LBPMATCH module=EBOOT.BIN start=0x089726ec end=0x089727c4 symbol=FUN_089726ec lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int (**vtable)(struct Obj *);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
};
struct Self { char pad[676]; struct Obj *p; };
extern void *DAT_08be84a8;
extern void FUN_089f1a64(int n);
extern void FUN_089f19c0(int n);
extern void FUN_089717a0(void *buf, struct Self *s, int a);
extern struct Obj *FUN_08aaaa80(void *pool, void *key);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_08a9bff8(void *buf);

void FUN_089726ec(struct Self *self)
{
  int buf[8];
  struct Obj *o;
  struct Obj *old;
  FUN_089f1a64(14);
  FUN_089717a0(buf, self, 0);
  o = FUN_08aaaa80(DAT_08be84a8, buf);
  if (o != 0) {
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
  }
  old = self->p;
  self->p = o;
  if (old != 0)
    FUN_088050b8(old);
  if (o != 0)
    FUN_088050b8(o);
  FUN_089f19c0(14);
  FUN_08a9bff8(buf);
}
