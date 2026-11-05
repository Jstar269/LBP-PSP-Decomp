/* LBPMATCH module=EBOOT.BIN start=0x088048a8 end=0x08804948 symbol=FUN_088048a8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: volatile running flag (the only exact form found: with a plain flag GCC copies the loop test load into the loop-entry jump's delay slot, retail leaves it empty). */
extern "C" void FUN_089f1a0c(void);
extern "C" void FUN_089f1a64(int mode);
extern "C" int FUN_089f19c0(int mode);
extern "C" int FUN_08ba0868(char *buf, unsigned int size, const char *fmt, ...);
extern "C" void FUN_088a7e7c(void);
extern "C" unsigned char DAT_08bf9385;
extern "C" int DAT_08bf92e4;
extern "C" const char DAT_08bcf89c[];

struct App {
  volatile bool running;
};
extern "C" App *FUN_089ecaa8(void);
extern "C" void FUN_089ec234(App *app);
extern "C" void FUN_08804794(App *app);
extern "C" void FUN_089ecc80(App *app);

extern "C" int FUN_088048a8(void)
{
  char buf[128];

  DAT_08bf9385 = 1;
  FUN_089f1a0c();
  FUN_089f1a64(2);
  DAT_08bf92e4 = 1;
  FUN_08ba0868(buf, sizeof buf, DAT_08bcf89c, FUN_088a7e7c);
  App *app = FUN_089ecaa8();
  FUN_089ec234(app);
  while (app->running)
    FUN_08804794(app);
  FUN_089ecc80(app);
  FUN_089f19c0(2);
  return 0;
}
