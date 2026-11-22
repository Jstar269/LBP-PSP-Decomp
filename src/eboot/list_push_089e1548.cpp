/* LBPMATCH module=EBOOT.BIN start=0x089e1548 end=0x089e15dc symbol=FUN_089e1548 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
extern "C" void *DAT_08be84a0;
extern "C" void *FUN_08adbb84(void *, int);
extern "C" void FUN_08b88fac(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
struct Shared { int pad0; int pad4; int rc; int pad12[2]; int lock; };
struct Ref {
  Shared *p;
  Ref(const Ref &o)
  {
    Shared *s = o.p;
    p = s;
    if (s) {
      FUN_08ad3088(&s->lock);
      s->rc++;
      FUN_08ad2810(&s->lock);
    }
  }
};
struct Node { int a; int b; Ref item; };
extern "C" void FUN_089e1548(void *list, Ref *r)
{
  Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, 12);
  new (&n->item) Ref(*r);
  FUN_08b88fac(n, list);
}
