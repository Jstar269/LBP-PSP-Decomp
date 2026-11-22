/* LBPMATCH module=EBOOT.BIN start=0x08978658 end=0x089786f4 symbol=FUN_08978658 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Ref;
struct RVT { void (*dtor)(struct Ref *); };
struct Ref { const struct RVT *vt; int count; };
struct Res { char pad[52]; int f52; };
struct Self { char pad0[20]; void *p20; char pad1[8]; int f32; char pad2[8]; struct Ref *r44; };
extern struct Res *FUN_0896be94(void *a, int b, struct Ref **r);
extern void FUN_08adb8a4(void *p);
extern void FUN_089757c0(int idx);

void FUN_08978658(struct Self *self)
{
  struct Ref *tmp = self->r44;
  void *a = self->p20;
  int b = self->f32;
  struct Res *r;
  struct Ref *p;
  if (tmp != 0)
    tmp->count = tmp->count + 1;
  r = FUN_0896be94(a, b, &tmp);
  p = tmp;
  if (p != 0) {
    int n = p->count - 1;
    p->count = n;
    if (n == 0) {
      p->vt->dtor(p);
      FUN_08adb8a4(p);
    }
  }
  if (r != 0)
    FUN_089757c0(r->f52);
}
// NEAR: 33 words differ (38/39) from scheduling only: retail loads self->p20 into v0 first and moves it to a0 in the delay slot, stores the temporary `sw v1,0(sp)` after the argument loads; ours loads a1 first and a0 last and stores the temporary early.
// The release uses a copy `p = tmp` (retail keeps it in s0) as in lbp's other refcount releases.
// near miss from lane api-lbp-b-20261121 (unreviewed)
