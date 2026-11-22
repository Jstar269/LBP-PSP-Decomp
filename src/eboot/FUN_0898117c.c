/* LBPMATCH module=EBOOT.BIN start=0x0898117c end=0x0898124c symbol=FUN_0898117c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { int pad[13]; void (*f34)(struct Obj *, void *); };
struct Obj { const struct VT *vt; };
struct Info { char pad0[16]; short s16; char pad1[2]; int f20; int f24; short s28; char pad2[2]; short s32; };
struct In1 { int pad[3]; struct Info *info; };
struct Sub { char pad[4]; struct In1 *in; };
struct Ctx { char pad[688]; struct Sub *sub; };
extern struct Ctx *DAT_08be85fc;
extern void *FUN_08a9e838(int n);
extern void FUN_089d5884(struct Obj *o, short a, short b, short c, short d, unsigned e);
extern void FUN_089d5bfc(struct Obj *o, int a, int b, int c);
extern void *FUN_08965c98(void);
extern void FUN_089667c0(struct Obj *o);

struct Obj *FUN_0898117c(unsigned arg)
{
  struct Sub *sub = DAT_08be85fc->sub;
  struct Info *i = sub->in->info;
  short a = i->s16;
  short c = i->s28;
  short d = i->s32;
  struct Obj *o = (struct Obj *)FUN_08a9e838(276);
  FUN_089d5884(o, a, a, c, d, arg);
  i = sub->in->info;
  FUN_089d5bfc(o, i->f20, i->f24, 0);
  o->vt->f34(o, FUN_08965c98());
  FUN_089667c0(o);
  return o;
}
