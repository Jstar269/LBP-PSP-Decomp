/* LBPMATCH module=EBOOT.BIN start=0x08972b70 end=0x08972c4c symbol=FUN_08972b70 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int (**vtable)(struct Obj *);
  int pad0;
  int count;
  int pad1[2];
  int lock[1];
};
struct Self { char pad[680]; struct Obj *p; };
extern void *DAT_08be84a8;
extern void FUN_089717a0(void *buf, struct Self *s, int a);
extern struct Obj *FUN_08aaaa80(void *pool, void *key);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_08a9bff8(void *buf);

void FUN_08972b70(struct Self *self)
{
  int buf[8];
  struct Obj *o;
  struct Obj *old;
  old = self->p;
  if (old != 0) {
    self->p = 0;
    FUN_088050b8(old);
  }
  FUN_089717a0(buf, self, 1);
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
  FUN_08a9bff8(buf);
}
