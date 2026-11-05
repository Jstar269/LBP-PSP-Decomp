/* LBPMATCH module=EBOOT.BIN start=0x08949590 end=0x08949604 symbol=FUN_08949590 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08948ff0(void *g);
extern "C" void FUN_08946fdc(void *g);
struct G { int pad[385]; int state; int pad2[3]; int lock[1]; };
extern G *DAT_08bedb1c;
extern "C" void FUN_08949590()
{
  G *g = DAT_08bedb1c;
  void *l = g->lock;
  FUN_08ad3088(l);
  switch (g->state) { case 0: FUN_08948ff0(g); break; case 5: FUN_08946fdc(g); break; }
  FUN_08ad2810(l);
}
