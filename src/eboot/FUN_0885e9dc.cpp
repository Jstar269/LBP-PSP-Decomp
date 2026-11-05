/* LBPMATCH module=EBOOT.BIN start=0x0885e9dc end=0x0885ea50 symbol=FUN_0885e9dc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern int DAT_08bef8f4;
struct Obj { int pad; int rc; };
struct Handle { Obj *p; int extra; };
extern "C" void FUN_0885e9dc(Handle *dst, Handle *src)
{
  Obj *p = src->p;
  dst->p = p;
  if (p) {
    FUN_08ad3088(&DAT_08bef8f4);
    p->rc = p->rc + 1;
    FUN_08ad2810(&DAT_08bef8f4);
  }
  dst->extra = src->extra;
}
