/* LBPMATCH module=EBOOT.BIN start=0x08923d10 end=0x08923d84 symbol=FUN_08923d10 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08923a80(void *s);
extern "C" void FUN_08923c70(void *s);
struct S { int pad[1]; int st; int pad2[0]; char lock[1]; };
extern S *DAT_08bef76c;
extern "C" void FUN_08923d10()
{
  S *s = DAT_08bef76c;
  FUN_08ad3088(s->lock);
  switch (s->st) {
  case 0: FUN_08923a80(s); break;
  case 5: FUN_08923c70(s); break;
  }
  FUN_08ad2810(s->lock);
}
