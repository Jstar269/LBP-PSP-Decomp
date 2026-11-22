/* LBPMATCH module=EBOOT.BIN start=0x08999a64 end=0x08999b58 symbol=FUN_08999a64 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Ref;
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
void *FUN_08a9e838(int size);
void FUN_08a20ba8(void *o, const Ref &r);
void FUN_08a61fa4(void *a, void *b);
void FUN_08a20de8(void *p);
void FUN_08a9e0cc(void *p);
extern void *DAT_08bf5df8;
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
  ~Ref() { if (p) FUN_088050b8(p); }
};

struct Self {
  void *obj;
  char pad0[12];
  int val;
  void *res;
  char pad1[76];
  Rc *rc;
};

extern "C" void FUN_08999a64(Self *self, const int *pv)
{
  int v = *pv;
  if (self->val != v) {
    self->val = v;
    if (v == 0) {
      if (self->obj == 0) {
        Ref t(self->rc);
        void *o = FUN_08a9e838(240);
        FUN_08a20ba8(o, t);
        self->obj = o;
      }
    } else {
      if (self->res != 0) {
        FUN_08a61fa4(self->res, self->obj);
        self->res = 0;
      }
      void *p = DAT_08bf5df8;
      if (p != 0) {
        FUN_08a20de8(p);
        FUN_08a9e0cc(p);
      }
      DAT_08bf5df8 = self->obj;
      self->obj = 0;
    }
  }
}
