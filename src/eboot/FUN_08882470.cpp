/* LBPMATCH module=EBOOT.BIN start=0x08882470 end=0x088824d0 symbol=FUN_08882470 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08879588(void *p);
extern "C" void FUN_08885278(void *p);
extern "C" void FUN_08896044(void *p);
extern "C" void FUN_08893334(void *p);
extern unsigned char DAT_08bee058;
extern void *DAT_08bee020, *DAT_08bee010, *DAT_08bee014, *DAT_08bee00c;
extern "C" void FUN_08882470(void)
{
  if (DAT_08bee058 == 0) {
    FUN_08879588(DAT_08bee020);
    FUN_08885278(DAT_08bee010);
    FUN_08896044(DAT_08bee014);
    FUN_08893334(DAT_08bee00c);
  }
  DAT_08bee058 = 1;
}
