/* LBPMATCH module=EBOOT.BIN start=0x089310b8 end=0x08931120 symbol=FUN_089310b8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" int FUN_0892fcb8(void *g, void *a, int b);
struct G { int pad[25]; char lock[1]; };
extern G *DAT_08bedb14;
extern "C" int FUN_089310b8(void *a, int b)
{
  G *g = DAT_08bedb14;
  void *args[1];
  args[0] = a;
  FUN_08ad3088(g->lock);
  int r = FUN_0892fcb8(g, args, b);
  FUN_08ad2810(g->lock);
  return r;
}
