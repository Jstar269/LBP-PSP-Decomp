/* LBPMATCH module=EBOOT.BIN start=0x089c6810 end=0x089c6874 symbol=FUN_089c6810 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_089c649c(void *a, int b, int c);
struct Sys { int pad[389]; char lock[224]; unsigned char flag; /* +1780 */ };
extern Sys *DAT_08bedb1c;
extern "C" void FUN_089c6810(void *a)
{
  Sys *s = DAT_08bedb1c;
  FUN_08ad3088(s->lock);
  unsigned char c = s->flag;
  FUN_08ad2810(s->lock);
  FUN_089c649c(a, c == 0 ? 602 : 588, 2);
}
