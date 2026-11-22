/* LBPMATCH module=EBOOT.BIN start=0x089abb34 end=0x089abc04 symbol=FUN_089abb34 lang=c++ */
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
};

struct Elem12 { int a; int b; int c; };
struct Item {
  char pad[44];
  Ref ref;
};
struct Src {
  char pad[44];
  Item items[1];
};
struct Self {
  char pad[112];
  Elem12 *start;
  Elem12 *finish;
};

extern "C" void FUN_089ab578(Self *self, const Ref &r, unsigned i);

extern "C" void FUN_089abb34(Self *self, Item *src)
{
  for (unsigned i = 0; i < (unsigned)(self->finish - self->start); i++)
    FUN_089ab578(self, Ref(src[i].ref), i);
}
// NEAR 28 words (57/52): structure matches (Ref temporary per element passed by const&, element count as exact pointer difference / 12).
// Blocking: the exact division `(diff >> 2) * 0xaaaaaaab` is `lui/ori; mult; mflo` in retail, a 12-insn shift/add chain in ours (same
// multiply-by-constant cost difference as 0x0899a6e8); also retail walks a pointer to the first Ref (`addiu s3,a1,44; += 48`) instead of indexing.
// near miss from lane api-lbp-c-20261121 (unreviewed)
