/* LBPMATCH module=EBOOT.BIN start=0x08874cb8 end=0x08874d50 symbol=FUN_08874cb8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct Ref {
  virtual void method0();
  virtual void method1();
  virtual void release();
  int count;
};

struct Self {
  void *vtable;
  char pad[20];
  Ref *reference;
};

extern "C" char DAT_08bb0618[];
extern "C" char DAT_08bb0638[];
extern "C" char DAT_08bef8f4[];
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0894a260(void *);

extern "C" void FUN_08874cb8(Self *self)
{
  Ref *reference = self->reference;
  self->vtable = DAT_08bb0618;
  if (reference != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    int count = reference->count;
    count = count - 1;
    reference->count = count;
    FUN_08ad2810(&DAT_08bef8f4);
    if (count == 0) {
      reference->release();
    }
  }
  self->vtable = DAT_08bb0638;
  FUN_0894a260(self);
}
