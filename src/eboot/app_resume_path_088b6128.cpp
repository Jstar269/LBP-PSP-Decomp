/* LBPMATCH module=EBOOT.BIN start=0x088b6128 end=0x088b6190 symbol=FUN_088b6128 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Temp {
  int words[8];
};
struct Name;
struct Hud {
  int pad[0x14 / 4];
  short pad16;
  short a;  /* +0x16 */
  short b;  /* +0x18 */
};
struct App {
  int pad[0x18 / 4];
  Name *name[1];           /* +0x18 */
  int pad2[(0xe4 - 0x1c) / 4];
  unsigned char pad3[3];
  unsigned char shown;     /* +0xe7 */
  int pad4[(0x2e0 - 0xe8) / 4];
  unsigned char resumed;   /* +0x2e0 */
};
extern "C" Hud *DAT_08beecb4;
extern "C" App *FUN_089ecaa8(void);
extern "C" void FUN_089eb8ec(App *app);
extern "C" void FUN_08a9c0b4(Temp *temp, Name **name);
extern "C" void FUN_088b5468(Temp *temp);
extern "C" void FUN_08a9bff8(Temp *temp);

extern "C" void FUN_088b6128(App *app)
{
  app->shown = 0;
  if (!app->resumed) {
    FUN_089eb8ec(FUN_089ecaa8());
  } else {
    DAT_08beecb4->a = 0;
    DAT_08beecb4->b = 0;
    Temp temp;
    FUN_08a9c0b4(&temp, app->name);
    FUN_088b5468(&temp);
    FUN_08a9bff8(&temp);
  }
}
