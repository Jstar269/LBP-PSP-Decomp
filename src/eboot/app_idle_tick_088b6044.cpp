/* LBPMATCH module=EBOOT.BIN start=0x088b6044 end=0x088b6084 symbol=FUN_088b6044 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct App;
extern "C" App *FUN_089ecaa8(void);
extern "C" bool FUN_089ea994(App *app);
extern "C" void *FUN_08a50fbc(void);
extern "C" void FUN_08a51f94(void *p);

extern "C" void FUN_088b6044(void)
{
  if (!FUN_089ea994(FUN_089ecaa8()))
    FUN_08a51f94(FUN_08a50fbc());
}
