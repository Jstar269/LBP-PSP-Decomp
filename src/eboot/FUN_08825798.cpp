/* LBPMATCH module=EBOOT.BIN start=0x08825798 end=0x0882580c symbol=FUN_08825798 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e824(int n);
extern "C" void FUN_08ba0984(const char *s, const char *fmt, void *a, void *b, void *c, void *d, void *e, void *f, void *g);
extern char DAT_08bad834[];
struct Obj { int f0, f4, f8, f12, f16, f20, f24; };
extern "C" void FUN_08825798(void *unused, const char *s, Obj **out)
{
  Obj *m = (Obj *)FUN_08a9e824(28);
  FUN_08ba0984(s, DAT_08bad834, &m->f0, &m->f4, &m->f8, &m->f12, &m->f16, &m->f20, &m->f24);
  *out = m;
}
