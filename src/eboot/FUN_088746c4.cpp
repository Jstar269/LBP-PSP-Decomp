/* LBPMATCH module=EBOOT.BIN start=0x088746c4 end=0x08874754 symbol=FUN_088746c4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); matching idiom: volatile flag23 store preserves the retail lock-call order; exact match verified; names and types await human review. */
struct Holder {
  virtual void method0();
  virtual void release();
  int refs;
  int count;
  unsigned char flag12;
  char pad[7];
  unsigned char flag20;
  char pad2[2];
  volatile unsigned char flag23;
};

extern "C" char DAT_08bef8f4[];
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0888256c();

extern "C" void FUN_088746c4(int *param_1, Holder *param_2)
{
  if (*(unsigned int *)(*param_1 + 0x40) != 0) {
    param_2->flag12 = 1;
  }
  param_2->flag20 = 1;
  param_2->flag23 = 0;
  FUN_08ad3088(&DAT_08bef8f4);
  int refs = param_2->refs;
  int count = param_2->count;
  refs = refs - 1;
  count = count - 1;
  param_2->count = count;
  param_2->refs = refs;
  FUN_08ad2810(&DAT_08bef8f4);
  if (refs == 0) {
    param_2->release();
  }
  FUN_0888256c();
}
