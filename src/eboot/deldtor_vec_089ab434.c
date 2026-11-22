/* LBPMATCH module=EBOOT.BIN start=0x089ab434 end=0x089ab4d0 symbol=FUN_089ab434 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Elem { void *ref; int a; int b; };
struct Vec { struct Elem *start; struct Elem *finish; struct Elem *eos; };
struct Self { void *vt; char pad[108]; struct Vec v; };

extern void *DAT_08bc0738[];
extern void *DAT_08bc0538[];
extern void FUN_088050b8(void *p);
extern void FUN_08a9e0cc(void *p);
extern void FUN_0896cd08(struct Self *self);

void FUN_089ab434(struct Self *self)
{
  struct Vec *v = &self->v;
  struct Elem *p = v->start;
  struct Elem *e = v->finish;
  void *r;

  self->vt = DAT_08bc0738;
  while (p != e) {
    r = p->ref;
    p++;
    if (r != 0)
      FUN_088050b8(r);
  }
  if (v->start != 0)
    FUN_08a9e0cc(v->start);
  self->vt = DAT_08bc0538;
  FUN_0896cd08(self);
  FUN_08a9e0cc(self);
}
