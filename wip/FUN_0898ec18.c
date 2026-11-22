/* LBPMATCH module=EBOOT.BIN start=0x0898ec18 end=0x0898ed18 symbol=FUN_0898ec18 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Q;
struct QVT { int pad[2]; void (*f8)(struct Q *q, int b); };
struct Q { const struct QVT *vt; };
struct Ctx { char pad0[40]; int f40; char pad1[8]; int f52; };
struct Cp { struct Ctx *c; };
struct Obj {
  struct Q *q;
  int f4;
  int f8[1];
  int f12;
  int f16;
  char pad0[12];
  int f32[1];
  char pad1[24];
  int f60[1];
  char pad2[24];
  int f88;
};
extern int DAT_08be8600;
extern void FUN_08921858(int *p);
extern void FUN_08a9c6b0(int *p, int a);
extern void FUN_08ba10e8(char *dst, int s, int n);
extern void FUN_089e7130(int a, int b, int *p);

void FUN_0898ec18(struct Cp *cp, struct Obj *o)
{
  int p[3];
  char buf[68];
  struct Q *q;
  int flag;
  int *p60 = o->f60;
  int *p32 = o->f32;
  int *p8 = o->f8;
  if (cp->c->f52 != 0) {
    o->f88 = 0;
    FUN_08921858(p32);
    FUN_08921858(p60);
    FUN_08a9c6b0(p8, 1);
    q = o->q;
    flag = 1;
  } else {
    p[1] = 640;
    p[0] = 0;
    p[2] = cp->c->f40 - 25;
    FUN_08ba10e8(buf, p8[2], o->f12 + 1);
    FUN_089e7130(DAT_08be8600, o->f88, p);
    o->f88 = 0;
    FUN_08921858(p32);
    FUN_08921858(p60);
    FUN_08a9c6b0(p8, 1);
    q = o->q;
    flag = 0;
  }
  o->f4 = 0;
  if (q != 0)
    q->vt->f8(q, flag);
}
// NEAR: 2 words differ (64/64): the final `sw zero,4(s0)` (o->f4 = 0) is issued after `move a0,v0` in ours, before it (and the beqz) in retail. The three derived pointers o+60, o+32, o+8 as locals declared in that order give retail's register roles;
// the `flag` assigned in each branch gives the constant 0/1 second argument of the final virtual call. Layout: the nonzero-f52 cleanup arm first.
// near miss from lane api-lbp-b-20261121 (unreviewed)
