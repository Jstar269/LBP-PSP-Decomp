/* LBPMATCH module=EBOOT.BIN start=0x088cf548 end=0x088cf5a8 symbol=FUN_088cf548 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Ent { int pad[52]; unsigned flags; };
extern "C" int FUN_088f65dc(Ent *e, void (*fn)(void));
extern "C" void FUN_088e68a0(Ent *e, int a, int b);
extern Ent *DAT_08beee0c;
extern "C" void FUN_088cf548(void)
{
  Ent *e = DAT_08beee0c;
  e->flags &= ~0x40u;
  if (FUN_088f65dc(e, FUN_088cf548) == 0) FUN_088e68a0(DAT_08beee0c, 1, 31);
}
