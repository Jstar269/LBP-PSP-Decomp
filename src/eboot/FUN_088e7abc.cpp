/* LBPMATCH module=EBOOT.BIN start=0x088e7abc end=0x088e7b30 symbol=FUN_088e7abc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_088e68a0(void *o, int a, int b);
extern unsigned DAT_08beedd8;
struct Obj { int pad[43]; int f172; };
extern "C" int FUN_088e7abc(Obj *o, int x)
{
  if (DAT_08beedd8 >= 21) {
    FUN_088e68a0(o, 1, 23);
    if (x != o->f172) o->f172 = x;
    return 1;
  }
  FUN_088e68a0(o, 0, 23);
  return 0;
}
