/* LBPMATCH module=EBOOT.BIN start=0x089956e0 end=0x08995778 symbol=FUN_089956e0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Ref;
struct RVT { void (*dtor)(struct Ref *); };
struct Ref { const struct RVT *vt; int count; int kind; };
struct Self { char pad[28]; struct Ref *r; };
extern void FUN_08adb8a4(void *p);
extern void FUN_08995520(struct Self *s, int a);
extern void FUN_089742dc(float dt, struct Self *s);

void FUN_089956e0(struct Self *self, float dt)
{
  struct Ref *p = self->r;
  int kind = p->kind;
  if (kind != 2) {
    int z = kind == 0;
    self->r = 0;
    if (p != 0) {
      int n = p->count - 1;
      p->count = n;
      if (n == 0) {
        p->vt->dtor(p);
        FUN_08adb8a4(p);
      }
    }
    FUN_08995520(self, z);
  }
  FUN_089742dc(dt, self);
}
// NEAR: 27 words differ (37/38). Structure matches (kind test against 2, release through a copy of the member with vt[0] and FUN_08adb8a4, tail call FUN_089742dc(dt, self) with the float argument kept in $f20);
// retail sets `sltiu s2,a1,1` before the null test and puts the `sw zero,28(a0)` clear in the beqz delay slot with the reload of the refcount after it; ours orders the refcount load and the clear differently (one word longer).
// near miss from lane api-lbp-b-20261121 (unreviewed)
