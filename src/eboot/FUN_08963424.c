/* LBPMATCH module=EBOOT.BIN start=0x08963424 end=0x089634f4 symbol=FUN_08963424 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct VT { int pad[73]; int (*f124)(void *); };
struct Game { const struct VT *vt; char pad[848]; unsigned char f852; };
struct Arg { int pad; unsigned flags; };
struct Res { void *p; int pad[3]; };
extern void *DAT_08bf03b8;
extern struct Game *DAT_08bf7304;
extern void FUN_08a50fbc(void);
extern void FUN_0896fea8(struct Res *r, void *o, int n);
extern void FUN_08970b6c(void *p);
extern void FUN_0896ee90(void *o, int a, int b);
extern int FUN_0896f3c0(void *o);
extern void FUN_08970494(void *o, int a, int b);

void FUN_08963424(int unused, struct Arg *arg)
{
  struct Res r;
  struct Game *g;
  void *o;
  FUN_08a50fbc();
  g = DAT_08bf7304;
  o = DAT_08bf03b8;
  FUN_0896fea8(&r, o, 4000);
  if (r.p != 0) {
    FUN_08970b6c(r.p);
    FUN_0896ee90(o, 0, 0);
  } else if ((arg->flags & 0x2000) != 0 && FUN_0896f3c0(o) == 0 && g->vt->f124(g) == 0 && g->f852 == 0) {
    FUN_08970494(o, 0, 0);
  }
}
