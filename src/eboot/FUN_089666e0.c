/* LBPMATCH module=EBOOT.BIN start=0x089666e0 end=0x089667c0 symbol=FUN_089666e0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj {
  int pad0;
  int pad1;
  int count;
  int pad2[2];
  int lock[1];
};
struct Inner { int pad; int pad1; float *f; };
struct Mid { int pad; struct Inner *in; };
struct Ctx { char pad0[24]; struct Obj *ref; char pad1[656]; struct Mid *mid; };
extern struct Ctx *DAT_08be85fc;
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void *FUN_08a9e838(int n);
extern void FUN_089ce348(void *o, struct Obj **r, int a, int b);
extern void FUN_089dd50c(float f, void *o, void *p, int a, int b, int c, int d);

void *FUN_089666e0(void)
{
  struct Obj *ref = DAT_08be85fc->ref;
  struct Mid *m = DAT_08be85fc->mid;
  struct Obj *tmp = ref;
  void *a;
  void *b;
  float f;
  if (ref != 0) {
    FUN_08ad3088(ref->lock);
    ref->count = ref->count + 1;
    FUN_08ad2810(ref->lock);
  }
  a = FUN_08a9e838(208);
  FUN_089ce348(a, &tmp, 0, 0);
  if (tmp != 0)
    FUN_088050b8(tmp);
  f = *m->in->f;
  b = FUN_08a9e838(128);
  FUN_089dd50c(f, b, a, 0, 0, 0, 0);
  return b;
}
