/* LBPMATCH module=EBOOT.BIN start=0x089e13a4 end=0x089e1438 symbol=FUN_089e13a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Parent { int pad0; unsigned flags; struct Obj *child; };
struct Obj {
  const void *vt;
  unsigned flags;
  int pad8[4];
  struct Parent *parent;
  int f28;
  int f32;
  int f36;
  int f40;
  int f44;
};
extern char DAT_08bc1c40[];
extern void FUN_0896b21c(struct Obj *o);
extern void FUN_089e1168(struct Obj *o);

void FUN_089e13a4(struct Obj *o, struct Parent *p, int a, int b)
{
  FUN_0896b21c(o);
  o->f36 = a;
  o->f40 = -1;
  o->flags |= 0x3001;
  o->f44 = -1;
  o->vt = DAT_08bc1c40;
  {
    unsigned f = o->flags | (p->flags & 2);
    o->f28 = b;
    o->flags = f;
  }
  o->parent = p;
  p->child = o;
  o->f32 = 0;
  FUN_089e1168(o);
}
// matching idiom: the flags value is computed into a local before the store to +28 (statement order steers the scheduler); the +36 store is written first (register roles of the two arguments).
