/* LBPMATCH module=EBOOT.BIN start=0x08a11c10 end=0x08a11cc8 symbol=FUN_08a11c10 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Contact_08a11c10;
struct Vec_08a11c10 { Contact_08a11c10 **begin; Contact_08a11c10 **end; Contact_08a11c10 **cap; };
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void *FUN_08b9e8cc(void *dst, const void *src, unsigned n);
extern "C" void FUN_08b8829c(void) __attribute__((noreturn));
static inline Contact_08a11c10 **Copy_08a11c10(Contact_08a11c10 *const *first, Contact_08a11c10 *const *last, Contact_08a11c10 **out)
{
  FUN_08b9e8cc(out, first, (last - first) * sizeof(Contact_08a11c10 *));
  return out + (last - first);
}
extern "C" void FUN_08a11c10(Vec_08a11c10 *v, const Vec_08a11c10 *o)
{
  unsigned n = o->end - o->begin;
  v->begin = 0;
  v->end = 0;
  v->cap = 0;
  if (n > 0x3fffffffu)
    FUN_08b8829c();
  v->begin = (Contact_08a11c10 **)FUN_08a9e838(n * sizeof(Contact_08a11c10 *));
  v->end = v->begin;
  v->cap = v->begin + n;
  v->end = Copy_08a11c10(o->begin, o->end, v->begin);
}
