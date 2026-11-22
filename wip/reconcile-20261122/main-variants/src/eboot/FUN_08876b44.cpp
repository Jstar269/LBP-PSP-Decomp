/* LBPMATCH module=EBOOT.BIN start=0x08876b44 end=0x08876bd8 symbol=FUN_08876b44 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct Ref {
  virtual void method0();
  virtual void release();
  int count;
};

struct Self {
  void *vtable;
  char pad[12];
  Ref *reference;
};

extern "C" char DAT_08bb06f0[];
extern "C" char DAT_08bef8f4[];
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08875da8(void *);
extern "C" void FUN_08a9e0cc(void *);

extern "C" void FUN_08876b44(Self *self)
{
  Ref *reference = self->reference;
  self->vtable = DAT_08bb06f0;
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
  FUN_08875da8(self);
  FUN_08a9e0cc(self);
}
