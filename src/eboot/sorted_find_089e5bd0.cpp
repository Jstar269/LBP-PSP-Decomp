/* LBPMATCH module=EBOOT.BIN start=0x089e5bd0 end=0x089e5cbc symbol=FUN_089e5bd0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. std::lower_bound over a sorted array with a comparison function pointer, then an equality test. */
typedef int (*Cmp)(unsigned, unsigned);
struct Set {
  int pad0;
  unsigned *begin;
  int count;
  int pad12;
  Cmp comp;
};
inline unsigned *lower_bound(unsigned *first, unsigned *last, unsigned val, Cmp comp)
{
  int len = last - first;
  int half;
  unsigned *middle;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    middle += half;
    if (comp(*middle, val)) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else
      len = half;
  }
  return first;
}
extern "C" unsigned FUN_089e5bd0(Set *s, unsigned val)
{
  unsigned *p = lower_bound(s->begin, s->begin + s->count, val, s->comp);
  if (p == s->begin + s->count)
    return 0;
  if (s->comp(val, *p) || s->comp(*p, val))
    return 0;
  return *p;
}
