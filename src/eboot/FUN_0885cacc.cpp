/* LBPMATCH module=EBOOT.BIN start=0x0885cacc end=0x0885cb44 symbol=FUN_0885cacc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern int DAT_08bef8f4;
struct Obj { int pad; int rc; };
struct Inner { int pad[82]; Obj *p; };
struct Src { int pad[3]; Inner *inner; };
struct Handle { Obj *p; };
extern "C" Handle *FUN_0885cacc(Handle *dst, Src *s)
{
  Inner *x = s->inner;
  if (x) {
    Obj *p = x->p;
    dst->p = p;
    if (p) {
      FUN_08ad3088(&DAT_08bef8f4);
      p->rc = p->rc + 1;
      FUN_08ad2810(&DAT_08bef8f4);
    }
    return dst;
  }
  dst->p = 0;
  return dst;
}
