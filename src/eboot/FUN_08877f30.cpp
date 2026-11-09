/* LBPMATCH module=EBOOT.BIN start=0x08877f30 end=0x08877fe4 symbol=FUN_08877f30 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct Self {
  virtual void method0();
  virtual void release();
  virtual void method2();
  virtual void method3();
  virtual void method4();
  virtual void method5();
  int refs;
  int count;
  char pad[48];
  void *payload;
  void *container;
  void *resource;
};

extern "C" char DAT_08bef8f4[];
extern "C" void FUN_0886cb2c(void *, void *, void *, int);
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0888256c();

extern "C" void FUN_08877f30(void *param_1, Self *self)
{
  FUN_0886cb2c(self, self->payload, self->container, 1);
  self->payload = 0;
  self->method5();
  if (self->resource != 0) {
    FUN_08a9e0c4(self->resource);
  }
  self->resource = 0;
  FUN_08ad3088(&DAT_08bef8f4);
  int refs = self->refs;
  int count = self->count;
  refs = refs - 1;
  count = count - 1;
  self->count = count;
  self->refs = refs;
  FUN_08ad2810(&DAT_08bef8f4);
  if (refs == 0) {
    self->release();
  }
  FUN_0888256c();
}
