/* LBPMATCH module=EBOOT.BIN start=0x089264b4 end=0x08926514 symbol=FUN_089264b4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08924744(void *s, int a, int b);
struct Sys { int pad[51]; char lock[1]; };
extern Sys *DAT_08bedb18;
extern "C" int FUN_089264b4(void *unused, int x)
{
  Sys *s = DAT_08bedb18;
  FUN_08ad3088(s->lock);
  FUN_08924744(s, 0, x);
  FUN_08ad2810(s->lock);
  return 0;
}
