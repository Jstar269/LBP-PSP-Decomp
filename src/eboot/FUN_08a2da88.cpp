/* LBPMATCH module=EBOOT.BIN start=0x08a2da88 end=0x08a2daf0 symbol=FUN_08a2da88 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a4ec(void *o);
struct Obj { int pad[147]; int f588, f592, f596, f600, f604, f608, f612, f616; };
extern Obj *DAT_08bee648;
extern char DAT_08bf6a70[5];
extern "C" void FUN_08a2da88(Obj *o)
{
  DAT_08bee648 = o;
  FUN_0894a4ec(o);
  o->f592 = 0;
  o->f596 = 0;
  o->f600 = 0;
  o->f604 = 0;
  o->f608 = 0;
  o->f612 = 0;
  o->f616 = 0;
  DAT_08bf6a70[1] = 0;
  DAT_08bf6a70[2] = 0;
  DAT_08bf6a70[3] = 0;
  DAT_08bf6a70[4] = 0;
  DAT_08bf6a70[0] = 0;
  o->f588 = 0;
}
