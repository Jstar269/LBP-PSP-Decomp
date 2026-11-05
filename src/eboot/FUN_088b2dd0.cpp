/* LBPMATCH module=EBOOT.BIN start=0x088b2dd0 end=0x088b2df4 symbol=FUN_088b2dd0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bb3200[];
struct P { void *vt; int f4; unsigned char b8; char pad[3]; int f12; unsigned char b16, b17; };
extern "C" void FUN_088b2dd0(struct P *o, int a, int b)
{
  o->vt = DAT_08bb3200;
  o->f4 = a;
  o->f12 = b;
  o->b8 = 0;
  o->b16 = 0;
  o->b17 = 0;
}
