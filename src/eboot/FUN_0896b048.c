/* LBPMATCH module=EBOOT.BIN start=0x0896b048 end=0x0896b0f0 symbol=FUN_0896b048 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { int pad[4]; void (*f10)(struct Obj *, unsigned); int pad2[3]; void (*f20)(struct Obj *); };
struct Obj { const struct VT *vt; unsigned flags; struct Obj *parent; };

void FUN_0896b048(struct Obj *o, unsigned char on)
{
  if (on != 0 && (o->flags & 4) == 0)
    o->vt->f20(o);
  if (on != ((o->flags >> 2) & 1)) {
    if (on != 0)
      o->flags = o->flags | 4;
    else
      o->flags = o->flags & ~4u;
    unsigned f = o->flags | 0x40;
    struct Obj *p = o->parent;
    if (p != 0)
      p->vt->f10(p, f);
  }
}
