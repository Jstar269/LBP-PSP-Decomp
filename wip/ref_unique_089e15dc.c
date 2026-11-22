/* LBPMATCH module=EBOOT.BIN start=0x089e15dc end=0x089e16ac symbol=FUN_089e15dc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Shared { int pad0; int pad4; int rc; int pad12[2]; int lock; };
struct Ref { struct Shared *p; };
struct Obj { unsigned x; int pad4[4]; int list; };
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_089e1548(void *list, struct Ref *r);
extern void FUN_088050b8(struct Shared *s);

static inline int unique(struct Shared *s)
{
  int c;
  FUN_08ad3088(&s->lock);
  c = s->rc;
  FUN_08ad2810(&s->lock);
  return c == 1;
}

static inline void reset(struct Ref *r)
{
  struct Shared *q = r->p;
  r->p = 0;
  if (q != 0)
    FUN_088050b8(q);
}

void FUN_089e15dc(struct Obj *o, struct Ref *r, unsigned k)
{
  struct Shared *s = r->p;
  if (s != 0) {
    if (o->x - k >= 2 || !unique(s)) {
      reset(r);
    } else {
      FUN_089e1548(&o->list, r);
    }
  }
}
// NEAR 26 words (50/52 words): same content; retail lays the two sibcall blocks (push tail, release tail) AFTER the inline release code with `bnez q,reltail; j END`, ours puts the release tail inline and is 2 words shorter.
// Tried: early return form, if/else, inline reset() helper, !(A&&B) forms. Same shape as 0x089e04a8 (extra `j END; lw ra` for a return path in retail).
// near miss from lane api-lbp-d-20261121 (unreviewed)
