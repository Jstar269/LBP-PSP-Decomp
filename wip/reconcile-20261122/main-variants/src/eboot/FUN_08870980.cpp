/* LBPMATCH module=EBOOT.BIN start=0x08870980 end=0x08870a38 symbol=FUN_08870980 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
struct Ref {
  virtual void method0();
  virtual void release();
  virtual void method2();
  virtual void method3();
  virtual void method4();
  virtual void method5();
  virtual void method6();
  virtual void method7();
  int count;
};

extern "C" char DAT_08bef8f4[];
extern "C" void *FUN_08a9e838(unsigned int);
extern "C" void FUN_0887367c(void *, void *);
extern "C" Ref **FUN_0886daf8(Ref **, Ref **);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

extern "C" void FUN_08870980(void *param_1, void *param_2, char *param_3)
{
  Ref *local_30[4];
  Ref *created = (Ref *)FUN_08a9e838(0x44);

  FUN_0887367c(created, param_2);
  local_30[0] = created;
  FUN_0886daf8((Ref **)(param_3 + 0x30), (Ref **)local_30);
  Ref *reference = local_30[0];
  if (reference != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    int count = reference->count - 1;
    reference->count = count;
    FUN_08ad2810(&DAT_08bef8f4);
    if (count == 0) {
      reference->release();
    }
  }
  created->method7();
}
