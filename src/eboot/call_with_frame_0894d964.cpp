/* LBPMATCH module=EBOOT.BIN start=0x0894d964 end=0x0894da14 symbol=FUN_0894d964 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void destroy();
  int refs;
};
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" int FUN_0894e158(void *);
struct Self;
struct Frame { Obj *out; Self *self; int a2; int a4; int a1; int a3; int a5; int a6; };
struct Self {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6();
  virtual void run(int (*cb)(void *), Frame *f, int x, int y);
};
extern "C" void FUN_0894d964(Self *s, int a1, int a2, int a3, int a4, int a5, int a6)
{
  Frame f;
  f.a1 = a1;
  f.a4 = a4;
  f.a3 = a3;
  f.a2 = a2;
  f.self = s;
  f.a6 = a6;
  f.a5 = a5;
  f.out = 0;
  s->run(FUN_0894e158, &f, 0, 1);
  Obj *o = f.out;
  if (o) {
    FUN_08ad3088(&DAT_08bef8f4);
    int r = o->refs - 1;
    o->refs = r;
    FUN_08ad2810(&DAT_08bef8f4);
    if (r == 0) o->destroy();
  }
}
