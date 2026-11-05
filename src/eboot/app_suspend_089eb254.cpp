/* LBPMATCH module=EBOOT.BIN start=0x089eb254 end=0x089eb2d4 symbol=FUN_089eb254 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: volatile App and Net flags, as the App running flag in the main loop 0x088048a8 (the only exact form found: with plain flags gcc404xs-O2/Os, gcc402x-O2/Os, gcc402t-O2 and gcc410-Os fill the call delay slots that retail leaves empty and do not hoist the session argument above the flag test). */
struct Lock {
  int handle;
};
struct Session {
  int handle;
};
struct Net {
  int pad[0x5b0 / 4];
  volatile unsigned char busy; /* +0x5b0 */
};
struct App {
  volatile bool running;
  volatile bool flag1;
  volatile unsigned char suspended2; /* +0x2 */
  unsigned char pad0;
  Lock lock;                /* +0x4 */
  int pad[(0x9c - 8) / 4];
  Session session;          /* +0x9c */
};
extern "C" Net *DAT_08bedb18;
extern "C" void FUN_08ad3088(Lock *lock);
extern "C" void FUN_08ad2810(Lock *lock);
extern "C" void FUN_088b2e24(Session *session);
extern "C" void FUN_088829c0(int mode);
extern "C" void FUN_0889a888(void);

extern "C" void FUN_089eb254(App *app)
{
  FUN_08ad3088(&app->lock);
  if (!app->suspended2) {
    app->suspended2 = 1;
    FUN_088b2e24(&app->session);
    DAT_08bedb18->busy = 1;
    FUN_088829c0(1);
    FUN_0889a888();
  }
  FUN_08ad2810(&app->lock);
}
