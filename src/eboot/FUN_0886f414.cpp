/* LBPMATCH module=EBOOT.BIN start=0x0886f414 end=0x0886f4c0 symbol=FUN_0886f414 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e824(unsigned n);
extern "C" void FUN_08b9e7a8(void *d, const void *s, unsigned n);
struct Self;
extern "C" void FUN_0886cb2c(Self *self, void *p, unsigned n, int f);
struct Src {
  virtual void v0();
  virtual void destroy();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual const void *data();
  virtual unsigned size();
};
struct Self {
  char pad[0x8c];
  Src *src; /* +0x8c */
};
extern "C" void FUN_0886f414(Self *self)
{
  unsigned n = self->src->size();
  void *buf = FUN_08a9e824(n);
  FUN_08b9e7a8(buf, self->src->data(), n);
  FUN_0886cb2c(self, buf, n, 1);
  Src *s = self->src;
  if (s) s->destroy();
  self->src = 0;
}
