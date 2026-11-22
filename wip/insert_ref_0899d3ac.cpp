/* LBPMATCH module=EBOOT.BIN start=0x0899d3ac end=0x0899d484 symbol=FUN_0899d3ac lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Lock { int x; };
struct Rc;
struct RcVT { int pad[2]; void (*destroy)(Rc *); };
struct Rc { RcVT *vt; int count; };

extern "C" {
void FUN_08ad3088(Lock *l);
void FUN_08ad2810(Lock *l);
extern Lock DAT_08bef8f4;
}

struct Ref {
  Rc *p;
  Ref(const Ref &o) : p(o.p) {
    Rc *q = o.p;
    if (q) {
      FUN_08ad3088(&DAT_08bef8f4);
      q->count = q->count + 1;
      FUN_08ad2810(&DAT_08bef8f4);
    }
  }
  ~Ref() {
    Rc *q = p;
    if (q) {
      FUN_08ad3088(&DAT_08bef8f4);
      int n = q->count - 1;
      q->count = n;
      FUN_08ad2810(&DAT_08bef8f4);
      if (n == 0)
        q->vt->destroy(q);
    }
  }
};

struct Key {
  unsigned a;
  Ref r;
  Key(unsigned a_, const Ref &r_) : a(a_), r(r_) {}
};

struct Res {
  int first;
  unsigned char second;
};

extern "C" Res FUN_088b7874(void *sub, const Key &k);

struct Self {
  char pad[16];
  char sub[1];
};

static inline Res insert(char *sub, const Key &k) { return FUN_088b7874(sub, k); }

extern "C" unsigned FUN_0899d3ac(Self *self, unsigned a, const Ref *r)
{
  Res res = insert(self->sub, Key(a, *r));
  return res.second;
}
// NEAR 30 words (53/54; the real difference is ~5 instructions at the result copy): Key{a, Ref} copy + inline insert wrapper
// returning an 8-byte pair {int, unsigned char} in v0/v1 reproduces the frame (res 0, key 8, tmp 16) and the early `addiu s1,a0,16`.
// Retail: `sw v1,20(sp); lbu v1,20(sp)` (second read back from the temp as a byte), `move s2,v1`, `sb v1,4(sp)`; ours stores v1 as a word
// to 4(sp) and re-reads it with `lbu s2,4(sp)` after the call (one word shorter).
// near miss from lane api-lbp-c-20261121 (unreviewed)
