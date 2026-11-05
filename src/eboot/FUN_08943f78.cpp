/* LBPMATCH module=EBOOT.BIN start=0x08943f78 end=0x08943fd8 symbol=FUN_08943f78 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08b1b230(int h, int *out);
struct Sys { int pad[98]; char lock[1]; };
extern Sys *DAT_08bedb18;
extern "C" bool FUN_08943f78(void *unused, int *h)
{
  int r = 0;
  Sys *s = DAT_08bedb18;
  FUN_08ad3088(s->lock);
  FUN_08b1b230(*h, &r);
  FUN_08ad2810(s->lock);
  return r == 0x78;
}
