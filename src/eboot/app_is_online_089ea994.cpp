/* LBPMATCH module=EBOOT.BIN start=0x089ea994 end=0x089ea9a0 symbol=FUN_089ea994 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: volatile App and Net flags, as the App running flag in the main loop 0x088048a8 (the only exact form found: with plain flags gcc404xs-O2/Os, gcc402x-O2/Os, gcc402t-O2 and gcc410-Os fill the call delay slots that retail leaves empty and do not hoist the session argument above the flag test). */
struct App {
  volatile bool running;
  volatile bool online; /* +0x1 */
};

extern "C" bool FUN_089ea994(App *app)
{
  return app->online;
}
