/* LBPMATCH module=EBOOT.BIN start=0x08913cf8 end=0x08913d58 symbol=FUN_08913cf8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" bool FUN_08918920(int h);
extern "C" void FUN_08918928(int h);
struct Obj { int pad[12]; int handle; int state; };
extern "C" void FUN_08913cf8(Obj *o, bool b)
{
  if (FUN_08918920(o->handle)) {
    if (o->state != 0) {
      if (b) {
        o->state = 6;
      } else {
        FUN_08918928(o->handle);
        o->state = 7;
      }
    }
  }
}
