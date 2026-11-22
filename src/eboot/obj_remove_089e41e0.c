/* LBPMATCH module=EBOOT.BIN start=0x089e41e0 end=0x089e4284 symbol=FUN_089e41e0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Sub { int pad[8]; struct Obj *owner; };
struct Obj {
  char pad0[640];
  struct Sub sub;
  char pad676[172];
  int f848;
  int pad852;
  int f856;
  int f860;
  int f864;
  int pad868;
  unsigned flags;
};
struct Mgr {
  char pad0[112];
  void (*cb)(int, int, struct Obj *, struct Obj *);
};
struct Key { int a; int b; int c; int d; };
extern int FUN_089d7dc8(int a, struct Sub *s, int b, int c, int d);
extern void FUN_089e51e0(void *buf);
extern void FUN_089e3cb0(struct Mgr *m, struct Key *k);

void FUN_089e41e0(struct Mgr *m, struct Obj *o)
{
  struct Key k;
  char buf[80];
  struct Sub *sp = &o->sub;
  m->cb(9000, o->f848, o, o);
  sp->owner = o;
  o->flags |= 8;
  o->f860 = FUN_089d7dc8(o->f848, sp, o->f856, o->f864, 0);
  {
    int x = o->f864;
    int y = o->f848;
    int z = o->f856;
    k.a = x;
    k.b = z;
    k.c = y;
  }
  k.d = 0;
  FUN_089e51e0(buf);
  FUN_089e3cb0(m, &k);
}
// matching idiom: the three key fields are loaded into locals in the order 864, 848, 856 before being stored (register roles of the loads); `struct Sub *sp = &o->sub` is a pointer local (retail keeps the sub-object address in v1).
