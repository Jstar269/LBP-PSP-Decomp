/* LBPMATCH module=EBOOT.BIN start=0x089eb478 end=0x089eb4d0 symbol=FUN_089eb478 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: volatile App and Net flags, as the App running flag in the main loop 0x088048a8 (the only exact form found: with plain flags gcc404xs-O2/Os, gcc402x-O2/Os, gcc402t-O2 and gcc410-Os fill the call delay slots that retail leaves empty and do not hoist the session argument above the flag test). */
struct Session {
  int handle;
};
struct App {
  volatile bool running;
  volatile bool online;              /* +0x1 */
  volatile unsigned char suspended2; /* +0x2 */
  volatile bool polled;              /* +0x3 */
  int pad[(0x9c - 4) / 4];
  Session session;                   /* +0x9c */
  int pad2[(0x194 - 0xa0) / 4];
  unsigned char pad3;
  bool quit;                /* +0x195 */
};
extern "C" void FUN_089eb348(App *app);
extern "C" void FUN_088b34bc(Session *session);

extern "C" void FUN_089eb478(App *app)
{
  if (app->suspended2)
    return;
  FUN_089eb348(app);
  FUN_088b34bc(&app->session);
  if (app->quit)
    app->running = 0;
}
