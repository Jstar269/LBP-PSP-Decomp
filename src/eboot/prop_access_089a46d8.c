/* LBPMATCH module=EBOOT.BIN start=0x089a46d8 end=0x089a478c symbol=FUN_089a46d8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Prop { unsigned id; unsigned b; float f; };
struct Obj {
  char pad0[149];
  unsigned char flag;
  char pad1[442];
  float val;
};

void FUN_089a46d8(struct Obj *o, struct Prop *p, int mode)
{
  _Bool set;

  if (mode == 2)
    return;
  set = mode == 0;
  switch (p->id) {
  case 4004:
  case 6001:
  case 10008:
    if (set)
      o->val = p->f;
    else
      p->f = o->val;
    break;
  case 27001:
    if (set)
      o->flag = p->b != 1;
    else
      p->b = o->flag == 0;
    break;
  case 29001:
    if (set)
      o->val = p->f;
    else
      p->f = o->val;
    break;
  }
}
