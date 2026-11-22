/* LBPMATCH module=EBOOT.BIN start=0x089e6a00 end=0x089e6aec symbol=FUN_089e6a00 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct SrcEnt { int a, b, c, d, e; };
struct Src { int pad0; int n; SrcEnt *ents; };
struct Elem {
  int f0, f4, f8, f12, f16;
  short f20, f22;
  int f24, f28, f32;
  Elem(const SrcEnt *e)
  {
    f32 = e->e;
    f0 = e->a;
    f4 = e->b;
    f16 = e->d;
    f20 = e->c;
    f8 = 0;
    f12 = 0;
    f22 = 4;
    f24 = 0;
    f28 = 0;
  }
};
struct Vec {
  Elem *begin;
  Elem **slots;
  int count;
  int cap;
  int pad16;
  char dirty;
};
struct Mgr {
  char pad0[36];
  Vec a;
  Vec b;
};
inline Vec *sel(Mgr *m, int flag)
{
  Vec *p = &m->a;
  if (flag != 0)
    p = &m->b;
  return p;
}
extern "C" void FUN_089e6a00(Mgr *m, int flag, Vec *v)
{
  Src *src = *(Src **)((char *)v->begin + 168);
  int n = src->n;
  for (int i = 0; i < n; i++, v = sel(m, flag)) {
    SrcEnt e = src->ents[i];
    if (v->cap != v->count) {
      int c = v->count++;
      Elem *el = v->begin + c;
      v->dirty = 0;
      v->slots[c] = el;
      new (el) Elem(&e);
    }
  }
}
// NEAR 55 words (61/59): structure right (src entries copied to a local before the capacity test, pooled slot vector with count/cap, placement-new Elem, selector m->a/m->b evaluated at the loop bottom); ours hoists `m+36` out of the loop and needs a fifth saved register (s4), retail computes both m+36 and m+60 at the bottom and uses 4 (s0..s3).
// Tried: ternary vs if form of the selector, SrcEnt by pointer vs by value.
// near miss from lane api-lbp-d-20261121 (unreviewed)
