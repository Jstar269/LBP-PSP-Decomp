/* LBPMATCH module=EBOOT.BIN start=0x089a4e60 end=0x089a4f4c symbol=FUN_089a4e60 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

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
void FUN_08a9d390(void *self, unsigned v);
void FUN_08a9bff8(void *self);
void FUN_08a5032c(int id, unsigned *v);
void FUN_08a50450(int id);
}

struct Ref {
  Rc *p;
  Ref(const Ref &o) : p(o.p) {
    Rc *q = o.p;
    if (q) {
      FUN_08ad3088(q->lock);
      q->count = q->count + 1;
      FUN_08ad2810(q->lock);
    }
  }
  ~Ref() { if (p) FUN_088050b8(p); }
  Ref &operator=(const Ref &o) {
    Rc *q = o.p;
    if (q) {
      FUN_08ad3088(q->lock);
      q->count = q->count + 1;
      FUN_08ad2810(q->lock);
    }
    Rc *old = p;
    p = q;
    if (old) FUN_088050b8(old);
    return *this;
  }
};

struct Tmp {
  char d[24];
  Tmp(unsigned v) { FUN_08a9d390(this, v); }
  ~Tmp() { FUN_08a9bff8(this); }
};

extern "C" Ref FUN_08a4e2a8(int id);

struct Self {
  int id;
  unsigned char flag;
};

struct Target {
  char pad0[28];
  Ref r;
  char pad1[4];
  unsigned v;
};

extern "C" void FUN_089a4e60(Self *self, Target *t, int *out)
{
  int id = self->id;
  unsigned v;
  if (self->flag != 0) {
    v = t->v;
    {
      Tmp tmp(v);
    }
    FUN_08a5032c(id, &v);
    FUN_08a50450(self->id);
  } else {
    *out = 0;
    Ref r = FUN_08a4e2a8(id);
    t->r = r;
  }
}
// NEAR 5 words: only the two stack slots differ (retail Ref r at sp+0 and unsigned v at sp+4, ours v at 0 and r at 4; the temp Tmp at 8 matches).
// The slot order follows the order in which the arms appear in the source; retail's order needs the flag==0 arm (Ref r) first in the source
// but its block layout has the flag!=0 arm inline: with `if (flag == 0) {r...} else {v...}` the layout flips (40 words). Tried a function-level `unsigned v;`.
// near miss from lane api-lbp-c-20261121 (unreviewed)
