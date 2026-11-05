/* LBPMATCH module=EBOOT.BIN start=0x08a115e0 end=0x08a116a0 symbol=FUN_08a115e0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
#include <stddef.h>
inline void *operator new(size_t, void *p) throw() { return p; }
struct Rec_08a115e0 {
  int a; int b; int e; int d; int c;
  Rec_08a115e0() {}
  Rec_08a115e0(int a_, int b_, int c_, int d_, int e_) : a(a_), b(b_), e(e_), d(d_), c(c_) {}
};
struct Log_08a115e0 { char pad[8204]; Rec_08a115e0 *recs; int count; };
extern "C" void FUN_08a115e0(Log_08a115e0 *l, int a, int b, int c, int d, int e)
{
  if (l->recs == 0) {
    l->recs = new Rec_08a115e0[512];
    l->count = 0;
  }
  new (&l->recs[l->count++]) Rec_08a115e0(a, b, c, d, e);
}
