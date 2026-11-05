/* LBPMATCH module=EBOOT.BIN start=0x089eafb8 end=0x089eb068 symbol=FUN_089eafb8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Service;
extern "C" void *FUN_08adb810(unsigned int size, unsigned int align, int a, int b);
extern "C" void FUN_08825418(void *mem);
extern "C" void FUN_089f9db4(void *mem);
extern "C" void FUN_0895a96c(void);
extern "C" void FUN_0895a500(void);
extern "C" void FUN_08841c9c(void);
extern "C" void FUN_0895a56c(void);
extern "C" void *FUN_08a9e838(unsigned int size);
extern "C" void FUN_08842da8(Service *service);
extern "C" Service *DAT_08bedb74;

extern "C" void FUN_089eafb8(void)
{
  FUN_08825418(FUN_08adb810(80, 16, 0, 0));
  FUN_089f9db4(FUN_08adb810(22304, 16, 0, 0));
  FUN_0895a96c();
  FUN_0895a500();
  FUN_08841c9c();
  if (!DAT_08bedb74) {
    Service *service = (Service *)FUN_08a9e838(16);
    FUN_08842da8(service);
    DAT_08bedb74 = service;
  }
  FUN_0895a56c();
}
