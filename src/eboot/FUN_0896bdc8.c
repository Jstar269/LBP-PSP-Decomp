/* LBPMATCH module=EBOOT.BIN start=0x0896bdc8 end=0x0896be94 symbol=FUN_0896bdc8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { void (*dtor)(struct Obj *); int pad[5]; void (*f18)(struct Obj *); };
struct Obj { const struct VT *vt; int count; };
struct Self { int pad; short s4; short pad1; int pad2; struct Obj *p12; };
extern void FUN_0896bc30(struct Self *s, struct Obj **out);
extern void FUN_08adb8a4(struct Obj *o);

void FUN_0896bdc8(struct Self *self)
{
  while (self->s4 != -1) {
    struct Obj *o = 0;
    struct Obj *p;
    FUN_0896bc30(self, &o);
    p = o;
    if (p != 0) {
      int n = p->count - 1;
      p->count = n;
      if (n == 0) {
        p->vt->dtor(p);
        FUN_08adb8a4(p);
      }
    }
  }
  if (self->p12 != 0) {
    struct Obj *p;
    self->p12->vt->f18(self->p12);
    p = self->p12;
    self->p12 = 0;
    if (p != 0) {
      int n = p->count - 1;
      p->count = n;
      if (n == 0) {
        p->vt->dtor(p);
        FUN_08adb8a4(p);
      }
    }
  }
}
