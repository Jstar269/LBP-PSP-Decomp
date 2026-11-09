/* LBPMATCH module=EBOOT.BIN start=0x0886f4c0 end=0x0886f568 symbol=FUN_0886f4c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct NewObject {
  virtual void method0();
  virtual void method1();
  virtual void method2(void *);
  virtual void method3();
  virtual void method4();
  virtual void method5();
  virtual void method6();
  virtual void method7();
  virtual void method8();
  int reserved[2];
  unsigned int field_c;
  unsigned int field_10;
};

struct Self {
  char pad[0x88];
  NewObject *object;
  char gap[12];
  int mode;
};

extern "C" void *FUN_08a9e838(unsigned int);
extern "C" void FUN_08954bb4(NewObject *);
extern "C" unsigned int FUN_08882440(int);
extern "C" unsigned int FUN_08882458(int);

extern "C" void FUN_0886f4c0(void *param_1, void *param_2, Self *self)
{
  NewObject *object = (NewObject *)FUN_08a9e838(0x14);
  FUN_08954bb4(object);
  self->object = object;
  int mode = self->mode;
  if (mode != 3) {
    object->field_c = FUN_08882440(mode);
    self->object->field_10 = FUN_08882458(mode);
  }
  self->object->method2(param_2);
  self->object->method8();
}
