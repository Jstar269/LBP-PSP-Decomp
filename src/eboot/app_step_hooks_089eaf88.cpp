/* LBPMATCH module=EBOOT.BIN start=0x089eaf88 end=0x089eafb8 symbol=FUN_089eaf88 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct App {
  char pad[0x197];
  unsigned char flag197; /* +0x197 */
};
extern "C" void FUN_08a2478c(void);
extern "C" void FUN_08a25aa8(void);

extern "C" void FUN_089eaf88(App *app)
{
  if (!app->flag197)
    FUN_08a2478c();
  FUN_08a25aa8();
}
