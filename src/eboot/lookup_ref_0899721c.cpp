/* LBPMATCH module=EBOOT.BIN start=0x0899721c end=0x08997300 symbol=FUN_0899721c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Ref;
struct Key { int a; int b; Key(const Key &o) { b = o.b; a = o.a; } };
struct Rc {
  int a;
  int b;
  int count;
  int c;
  int d;
  char lock[4];
};

extern "C" {
void FUN_08ad3088(void *lock);
void FUN_08ad2810(void *lock);
void FUN_088050b8(void *p);
int FUN_089e5b3c(int a, const Ref &ref);
extern int DAT_08be8600;
extern int DAT_08be84a8;
}

struct Ref {
  Rc *p;
  explicit Ref(Rc *q) : p(q) {
    if (q) {
      FUN_08ad3088(q->lock);
      q->count = q->count + 1;
      FUN_08ad2810(q->lock);
    }
  }
  Ref(const Ref &o) : p(o.p) {
    Rc *q = o.p;
    if (q) {
      FUN_08ad3088(q->lock);
      q->count = q->count + 1;
      FUN_08ad2810(q->lock);
    }
  }
  ~Ref() { if (p) FUN_088050b8(p); }
};

extern "C" Rc *FUN_08aaaa80(int a, Key k);

extern "C" int FUN_0899721c(void *self, const Key *key)
{
  Ref r(FUN_08aaaa80(DAT_08be84a8, *key));
  return FUN_089e5b3c(DAT_08be8600, Ref(r));
}
// matching idiom: Key copy constructor assigns b before a (retail copies word 0 first; the scheduler reverses source order).
