/* LBPMATCH module=EBOOT.BIN start=0x08999718 end=0x08999804 symbol=FUN_08999718 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Name;
extern "C" {
unsigned FUN_08a9b8a4(Name *self, const char *s);
extern char DAT_08babf20[];
}

struct Name {
  const char *s;
  unsigned h;
  void init(const char *p) {
    h = FUN_08a9b8a4(this, p);
    s = p;
  }
};

struct Key {
  unsigned id;
  Name n;
};

extern "C" {
unsigned FUN_08a9d790(Name *n);
void *FUN_08a50fbc(void);
void *FUN_08a52a88(void *mgr, int a, Key *k);
void *FUN_08a9e838(int size);
void FUN_08a87440(void *o, unsigned h);
}

extern "C" void *FUN_08999718(void)
{
  Key k;
  Name n3;
  Name n2;
  Name n1;
  unsigned h1;
  void *mgr;
  void *o;

  n1.init(DAT_08babf20);
  h1 = FUN_08a9d790(&n1);
  n2.init(DAT_08babf20);
  mgr = FUN_08a50fbc();
  n3.init(DAT_08babf20);
  k.id = FUN_08a9d790(&n3);
  k.n.init(DAT_08babf20);
  o = FUN_08a52a88(mgr, 8, &k);
  if (o == 0) {
    o = FUN_08a9e838(592);
    FUN_08a87440(o, h1);
  }
  return o;
}
// NEAR 53 words (an earlier form with Key/Name constructors and temporaries had 36 differ, 58/59 words): the control flow and calls match.
// Retail frame: 80 bytes, locals Key at sp+0 (id, Name), temps at 12/20/28 (n3, unused n2, n1), mgr in s2, h1 in s3, s1 = &n1 then &n3.
// Ours keeps the Key above the temps (frame 64) and drops the dead stores of the unused Name (n2). GCC orders the slots otherwise than retail.
// near miss from lane api-lbp-c-20261121 (unreviewed)
