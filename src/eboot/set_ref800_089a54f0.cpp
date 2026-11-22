/* LBPMATCH module=EBOOT.BIN start=0x089a54f0 end=0x089a55dc symbol=FUN_089a54f0 lang=c++ */
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
void FUN_08a9c164(void *self, const unsigned *p);
void FUN_08a9bff8(void *self);
void FUN_08a8cb64(int id, void *tmp);
extern unsigned DAT_08bd6488[];
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
  char d[32];
  Tmp(const unsigned *p) { FUN_08a9c164(this, p); }
  ~Tmp() { FUN_08a9bff8(this); }
};

struct Inner { char pad[64]; const unsigned *v; };
struct Mid { char pad[168]; Inner *inner; };
struct Obj { char pad[800]; Ref r; };
struct Self {
  Obj *obj;
  unsigned char flag;
};

struct Target {
  char pad0[24];
  Mid *m;
  Ref r;
};

extern "C" void FUN_089a54f0(Self *self, Target *t, int *out)
{
  if (self->flag != 0) {
    const unsigned *p;
    if (t->m == 0)
      p = DAT_08bd6488;
    else
      p = t->m->inner->v;
    int id = *(int *)self;
    Tmp tmp(p);
    FUN_08a8cb64(id, &tmp);
  } else {
    *out = 0;
    Ref r = self->obj->r;
    t->r = r;
  }
}
