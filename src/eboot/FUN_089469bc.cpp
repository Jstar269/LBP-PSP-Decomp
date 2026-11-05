/* LBPMATCH module=EBOOT.BIN start=0x089469bc end=0x08946a34 symbol=FUN_089469bc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
struct G { int pad[389]; int lock[1]; int pad2[38]; unsigned char b[2]; };
extern G *DAT_08bedb1c;
extern "C" int FUN_089469bc()
{
  G *g = DAT_08bedb1c;
  void *l = g->lock;
  FUN_08ad3088(l);
  int r = 1;
  for (int i = 0; i < 2; i++)
    if ((g->b[i] & 5) != 5) { r = 0; break; }
  FUN_08ad2810(l);
  return r;
}
