/* LBPMATCH module=EBOOT.BIN start=0x08996fec end=0x08997094 symbol=FUN_08996fec lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

extern "C" {
void FUN_08982744(void *self, const char *name, int line);
void FUN_08982298(void *self);
unsigned FUN_08adbd60(const char *s);
void FUN_08983c38(void *a, unsigned key, int n, void *src);
void FUN_088050b8(void *p);
extern char DAT_08bc00c8[];
extern char DAT_08bbef98[];
}

struct Ref {
  void *p;
  ~Ref() { if (p) FUN_088050b8(p); }
};

struct Obj {
  Ref a;
  Ref b;
  int pad[6];
};

struct Pair {
  Obj o[2];
  Pair(const char *n, int l) { FUN_08982744(&o[0], n, l); FUN_08982298(&o[1]); }
};

extern "C" void FUN_08996fec(void *unused, void *dst)
{
  Pair pr(DAT_08bc00c8, 4018);
  unsigned k = FUN_08adbd60(DAT_08bbef98);
  FUN_08983c38(dst, ~k, 210, &pr.o[0]);
}
// NEAR 14 words: structure matches (member array Pair{Obj o[2]} with Ref members, implicit dtor loop), but retail's loop
// carries q (s0, tested by beq s0,sp) and copies p=q at the top (s1); ours keeps p in s0 and recomputes p-32 for the test.
// Tried: local array with initialiser (35 words), C do-while (36 words), explicit inline ~Obj (40 words).
// near miss from lane api-lbp-c-20261121 (unreviewed)
