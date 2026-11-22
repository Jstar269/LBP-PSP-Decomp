/* LBPMATCH module=EBOOT.BIN start=0x0898dc98 end=0x0898dd68 symbol=FUN_0898dc98 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Lock { int x; };
struct Obj;
struct VT { int pad[2]; void (*del)(struct Obj *); };
struct Obj { const struct VT *vt; int count; int pad[20]; int *f80[1]; };
struct P2 { int a; int b; };
extern struct Lock DAT_08bef8f4;
extern void FUN_0898d940(void);
extern void FUN_0898d954(void);
extern void FUN_0883b1bc(struct Obj **out);
extern void FUN_089211a8(struct Obj *o, struct P2 *p);
extern void FUN_089377f0(struct Obj **o);
extern void FUN_08ad3088(struct Lock *l);
extern void FUN_08ad2810(struct Lock *l);

int FUN_0898dc98(struct P2 *src, int a, int b)
{
  struct Obj *o;
  struct P2 p;
  p = *src;
  FUN_0883b1bc(&o);
  FUN_089211a8(o, &p);
  ((int *)o)[20] = (int)FUN_0898d940;
  ((int *)o)[17] = b;
  ((int *)o)[16] = a;
  ((int *)o)[23] = (int)FUN_0898d954;
  ((int *)o)[26] = 0;
  FUN_089377f0(&o);
  {
    struct Obj *q = o;
    if (q != 0) {
      int n;
      FUN_08ad3088(&DAT_08bef8f4);
      n = q->count - 1;
      q->count = n;
      FUN_08ad2810(&DAT_08bef8f4);
      if (n == 0)
        q->vt->del(q);
    }
  }
  return 1;
}
