/* LBPMATCH module=EBOOT.BIN start=0x089a4b6c end=0x089a4c6c symbol=FUN_089a4b6c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Obj;
struct VT {
  char pad[260];
  unsigned (*get1)(struct Obj *);
  void (*set1)(struct Obj *, unsigned);
  int (*get2)(struct Obj *);
  void (*set2)(struct Obj *, unsigned);
};
struct Obj {
  struct VT *vt;
  char pad0[145];
  unsigned char flag;
};
struct Prop { unsigned id; unsigned b; };

void FUN_089a4b6c(struct Obj *o, struct Prop *p, int mode)
{
  _Bool set;

  if (mode == 2)
    return;
  set = mode == 0;
  switch (p->id) {
  case 19001:
    if (set)
      o->vt->set1(o, p->b);
    else
      p->b = o->vt->get1(o);
    break;
  case 19002:
    if (set)
      o->vt->set1(o, p->b);
    else
      p->b = o->vt->get1(o);
    break;
  case 26001:
    if (set)
      o->vt->set2(o, p->b == 1);
    else
      p->b = o->vt->get2(o) != 0;
    break;
  case 27001:
    if (set)
      o->flag = p->b != 1;
    else
      p->b = o->flag == 0;
    break;
  }
}
// matching idiom: case 19001 and case 19002 each carry their own copy of the same body (merged labels give a range test instead of retail's separate compares).
