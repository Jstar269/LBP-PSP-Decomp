/* LBPMATCH module=EBOOT.BIN start=0x08997094 end=0x08997164 symbol=FUN_08997094 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Ref;
extern "C" {
void FUN_08ad3088(void *lock);
void FUN_08ad2810(void *lock);
void FUN_088050b8(void *p);
int FUN_089e5b3c(int a, const struct Ref &ref);
extern int DAT_08be8600;
}

struct Rc {
  int a;
  int b;
  int count;
  int c;
  int d;
  char lock[4];
};

struct Ref {
  Rc *p;
  Ref() : p(0) {}
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

struct Obj;
struct VT {
  int pad[15];
  void (*fn)(Obj *, Ref *, Ref *);
};
struct Obj {
  VT *vt;
};

extern "C" int FUN_08997094(Obj *o)
{
  Ref x;
  Ref y;
  o->vt->fn(o, &x, &y);
  return FUN_089e5b3c(DAT_08be8600, Ref(x));
}
