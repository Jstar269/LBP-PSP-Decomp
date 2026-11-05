/* LBPMATCH module=EBOOT.BIN start=0x089eabd8 end=0x089eac94 symbol=FUN_089eabd8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Game;
struct App;
extern "C" Game *DAT_08bf03b8;
extern "C" App *DAT_08be85fc;
extern "C" void FUN_0896f2b0(Game *game);
extern "C" void FUN_08973468(App *app);
extern "C" void FUN_089f1a64(int id);
extern "C" int FUN_089f19c0(int id);
extern "C" void FUN_0895ce8c(void);
extern "C" void *FUN_08adb810(unsigned int size, unsigned int align, int a, int b);
extern "C" void FUN_08841428(void *mem);
extern "C" void FUN_0896ef04(Game *game, int stage);
extern "C" void *FUN_08914a7c(void);
extern "C" void FUN_089154cc(void *p);
extern "C" void FUN_089151c0(void *p);

extern "C" void FUN_089eabd8(void)
{
  FUN_0896f2b0(DAT_08bf03b8);
  FUN_08973468(DAT_08be85fc);
  FUN_089f1a64(7);
  FUN_0895ce8c();
  FUN_089f19c0(7);
  FUN_089f1a64(2);
  FUN_08841428(FUN_08adb810(48, 16, 0, 0));
  FUN_089f19c0(2);
  FUN_089f1a64(23);
  FUN_0896ef04(DAT_08bf03b8, 0);
  FUN_0896ef04(DAT_08bf03b8, 2);
  FUN_089f19c0(23);
  FUN_089154cc(FUN_08914a7c());
  FUN_089151c0(FUN_08914a7c());
}
