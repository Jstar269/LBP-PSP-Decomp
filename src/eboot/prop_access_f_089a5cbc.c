/* LBPMATCH module=EBOOT.BIN start=0x089a5cbc end=0x089a5d78 symbol=FUN_089a5cbc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Obj;
struct VT {
  char pad[292];
  float (*getf)(struct Obj *);
};
struct Obj {
  struct VT *vt;
  char pad0[145];
  unsigned char flag;
};
struct Prop { unsigned id; unsigned b; float f; };

extern void FUN_08a60b90(float f, struct Obj *o);

void FUN_089a5cbc(struct Obj *o, struct Prop *p, int mode)
{
  _Bool set;

  if (mode == 2)
    return;
  set = mode == 0;
  switch (p->id) {
  case 10005:
    if (set)
      FUN_08a60b90(p->f, o);
    else
      p->f = o->vt->getf(o);
    break;
  case 10006:
    if (set)
      FUN_08a60b90(p->f, o);
    else
      p->f = o->vt->getf(o);
    break;
  case 27001:
    if (set)
      o->flag = p->b != 1;
    else
      p->b = o->flag == 0;
    break;
  case 0x84d1:
    if (set)
      FUN_08a60b90(p->f, o);
    else
      p->f = o->vt->getf(o);
    break;
  }
}
// matching idiom: the three float-property cases each carry their own copy of the same body (retail dispatches 10006, <10007 then 10005, then 27001, then 0x84d1).
