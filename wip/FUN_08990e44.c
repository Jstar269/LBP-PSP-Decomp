/* LBPMATCH module=EBOOT.BIN start=0x08990e44 end=0x08990ef8 symbol=FUN_08990e44 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { int pad0; int kind; int count; int pad1[2]; int lock[1]; };
struct Self { char pad0[40]; struct Rc *rc; char pad1[12]; int f56; };
extern int DAT_08bf0b18;
extern int DAT_08bf0b10;
extern int DAT_08bf0b14;
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);
extern void FUN_088050b8(void *o);

static inline void addref(struct Rc *r)
{
  if (r != 0) {
    FUN_08ad3088(r->lock);
    r->count = r->count + 1;
    FUN_08ad2810(r->lock);
  }
}

int FUN_08990e44(struct Self *self)
{
  int r = 73;
  struct Rc *o = self->rc;
  int k;
  addref(o);
  k = o->kind;
  if (k != DAT_08bf0b18) {
    r = 36;
    if (k != DAT_08bf0b10 && k != DAT_08bf0b14)
      r = self->f56;
  }
  if (o != 0)
    FUN_088050b8(o);
  return r;
}
// NEAR: 13 words differ (41/45). Everything matches except that retail keeps the `if (o != 0)` test before the final release although o->kind was dereferenced earlier (same pattern as wip/FUN_089744cc.c): the project compiler deletes the dominated null test.
// Evidence for a profile difference (-fno-delete-null-pointer-checks style); see docs/LANE-NOTES.md.
// near miss from lane api-lbp-b-20261121 (unreviewed)
