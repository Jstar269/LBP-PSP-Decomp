/* LBPMATCH module=EBOOT.BIN start=0x08804794 end=0x088048a8 symbol=FUN_08804794 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Lock {
  int handle;
};
struct App {
  unsigned char running;
  char pad1[3];
  Lock lock; /* +0x4 */
  char pad2[0x196 - 0x8];
  unsigned char flag196;
  unsigned char flag197;
  unsigned char flag198;
  unsigned char flag199;
};
struct Status {
  char pad[0xc];
  unsigned char busy; /* +0xc */
};
extern "C" void FUN_089eaa68(void);
extern "C" void FUN_08a9ac50(void);
extern "C" void FUN_089ea9c8(App *app);
extern "C" void FUN_089eaf88(App *app);
extern "C" void FUN_08ad3088(Lock *lock);
extern "C" void FUN_08ad2810(Lock *lock);
extern "C" Status *FUN_089ea97c(void);
extern "C" void FUN_089eb478(App *app);
extern "C" void FUN_089eb068(App *app);
extern "C" void FUN_089ea9bc(App *app);
extern "C" void FUN_089ea9a0(App *app);
extern "C" void FUN_089eaabc(App *app);
extern "C" void FUN_089ec60c(App *app);
extern "C" void FUN_089ea9d0(App *app);
extern "C" void FUN_08a2d9b8(void);
extern "C" void FUN_08a2d8e8(void);
extern "C" unsigned char DAT_08be83f8;

extern "C" void FUN_08804794(App *app)
{
  FUN_089eaa68();
  FUN_08a9ac50();
  FUN_089ea9c8(app);
  FUN_089eaf88(app);
  FUN_08ad3088(&app->lock);
  if (!FUN_089ea97c()->busy) {
    FUN_089eb478(app);
    FUN_089eb068(app);
  } else {
    DAT_08be83f8 = 1;
  }
  FUN_089ea9bc(app);
  if (!FUN_089ea97c()->busy)
    FUN_089ea9a0(app);
  if (!app->flag197)
    FUN_089eaabc(app);
  FUN_08ad2810(&app->lock);
  FUN_089ec60c(app);
  FUN_089ea9d0(app);
  if (app->flag196) {
    if (app->flag199)
      FUN_08a2d9b8();
    else
      app->flag199 = 1;
  }
  if (app->flag198)
    FUN_08a2d8e8();
}
