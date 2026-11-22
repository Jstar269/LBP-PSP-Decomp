/* LBPMATCH module=EBOOT.BIN start=0x0896fa34 end=0x0896fb20 symbol=FUN_0896fa34 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
/* matching idiom: `struct Obj ***pe = &v->end` read through a local pointer (retail keeps the address of the end field in s6 for the second loop bound). */
struct Obj { char pad[6]; unsigned short rc; };
struct Vec { struct Obj **begin; struct Obj **end; };
extern void FUN_08970b6c(struct Obj *o);

struct Obj **FUN_0896fa34(struct Vec *v, struct Obj **first, struct Obj **last)
{
  int n = v->end - last;
  struct Obj **d = first;
  struct Obj **s = last;
  struct Obj **e;
  struct Obj ***pe = &v->end;
  while (0 < n) {
    struct Obj *o = *s;
    struct Obj *old;
    n = n - 1;
    s = s + 1;
    if (o != 0)
      o->rc = o->rc + 1;
    old = *d;
    *d = o;
    d = d + 1;
    if (old != 0)
      FUN_08970b6c(old);
  }
  e = *pe;
  while (d != e) {
    struct Obj *p = *d;
    d = d + 1;
    if (p != 0)
      FUN_08970b6c(p);
  }
  v->end = (struct Obj **)((int)v->end - (((int)last - (int)first) & ~3));
  return first;
}
