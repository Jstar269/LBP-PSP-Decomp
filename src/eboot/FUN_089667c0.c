/* LBPMATCH module=EBOOT.BIN start=0x089667c0 end=0x089668b4 symbol=FUN_089667c0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
/* The three float arguments are read through DAT_ symbols (their literals sit in three separate rodata blocks, which one rodata= key cannot place). */
struct Obj {
  int pad0;
  int pad1;
  int count;
  int pad2[2];
  int lock[1];
};
struct Ctx { char pad0[40]; struct Obj *a; struct Obj *b; };
extern struct Ctx *DAT_08be85fc;
extern const float DAT_08bab59c;
extern const float DAT_08baf1c0;
extern const float DAT_08bb885c;
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);
extern void FUN_089e0214(float a, float b, float c, void *self, struct Obj **x, struct Obj **y);

void FUN_089667c0(void *self)
{
  struct Obj *rb;
  struct Obj *ra;
  struct Obj *p = DAT_08be85fc->a;
  ra = p;
  if (p != 0) {
    FUN_08ad3088(p->lock);
    p->count = p->count + 1;
    FUN_08ad2810(p->lock);
  }
  p = DAT_08be85fc->b;
  rb = p;
  if (p != 0) {
    FUN_08ad3088(p->lock);
    p->count = p->count + 1;
    FUN_08ad2810(p->lock);
  }
  FUN_089e0214(DAT_08bab59c, DAT_08baf1c0, DAT_08bb885c, self, &ra, &rb);
  if (rb != 0)
    FUN_088050b8(rb);
  if (ra != 0)
    FUN_088050b8(ra);
}
