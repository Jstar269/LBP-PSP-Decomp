/* LBPMATCH module=EBOOT.BIN start=0x0886f414 end=0x0886f4c0 symbol=FUN_0886f414 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct Source {
  virtual void method0();
  virtual void release();
  virtual void method2();
  virtual void method3();
  virtual void method4();
  virtual void method5();
  virtual void method6();
  virtual void method7();
  virtual void *data();
  virtual unsigned int size();
};

struct Self {
  char pad[0x8c];
  Source *source;
};

extern "C" void *FUN_08a9e824(unsigned int);
extern "C" void FUN_08b9e7a8(void *, void *, unsigned int);
extern "C" void FUN_0886cb2c(Self *, void *, unsigned int, int);

extern "C" void FUN_0886f414(Self *self)
{
  unsigned int length = self->source->size();
  void *buffer = FUN_08a9e824(length);
  void *data = self->source->data();
  FUN_08b9e7a8(buffer, data, length);
  FUN_0886cb2c(self, buffer, length, 1);
  if (self->source != 0) {
    self->source->release();
  }
  self->source = 0;
}
