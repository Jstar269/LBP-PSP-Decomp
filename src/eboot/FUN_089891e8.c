/* LBPMATCH module=EBOOT.BIN start=0x089891e8 end=0x089892b8 symbol=FUN_089891e8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { int pad0[2]; int count; int pad1[2]; int lock[1]; };
struct Self { char pad[28]; int f28; };
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_08a65c38(int a, struct Obj **ref, int b);

static inline void addref(struct Obj *o)
{
  if (o != 0) {
    FUN_08ad3088(o->lock);
    o->count = o->count + 1;
    FUN_08ad2810(o->lock);
  }
}

void FUN_089891e8(struct Self *self, struct Obj **pref, int b)
{
  struct Obj *tmp;
  struct Obj *o = *pref;
  int a = self->f28;
  addref(o);
  tmp = o;
  addref(o);
  FUN_08a65c38(a, &tmp, b);
  if (tmp != 0)
    FUN_088050b8(tmp);
  if (o != 0)
    FUN_088050b8(o);
}
