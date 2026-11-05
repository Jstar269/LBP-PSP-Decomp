/* LBPMATCH module=EBOOT.BIN start=0x089228cc end=0x08922940 symbol=FUN_089228cc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08922798(void *s);
extern "C" void FUN_089222cc(void *s);
struct S { int pad[1]; int st; int pad2[0]; char lock[1]; };
extern S *DAT_08bedb20;
extern "C" void FUN_089228cc()
{
  S *s = DAT_08bedb20;
  FUN_08ad3088(s->lock);
  switch (s->st) {
  case 0: FUN_08922798(s); break;
  case 5: FUN_089222cc(s); break;
  }
  FUN_08ad2810(s->lock);
}
