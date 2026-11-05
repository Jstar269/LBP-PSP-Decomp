/* LBPMATCH module=EBOOT.BIN start=0x088cf434 end=0x088cf498 symbol=FUN_088cf434 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_088826e4(void);
extern "C" int FUN_08882aec(void);
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f73dc(void *p);
extern "C" void FUN_088e3414(void *a, void *b, int c);
struct Sys { int pad[113]; void *f452; };
extern Sys *DAT_08beed14;
extern void *DAT_08beed8c;
extern "C" void FUN_088cf434(void)
{
  if (FUN_088826e4() && FUN_08882aec()) {
    FUN_088f73dc(FUN_088d7f9c());
    FUN_088e3414(DAT_08beed8c, DAT_08beed14->f452, 1);
  }
}
