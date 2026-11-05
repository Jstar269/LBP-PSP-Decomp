/* LBPMATCH module=EBOOT.BIN start=0x08922fe0 end=0x08923050 symbol=FUN_08922fe0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08baa4fc(void *a, void *b);
struct G { int pad[110]; int f440; };
extern "C" G *FUN_089ecaa8();
struct Obj { int pad[3]; int f12; int f16, f20, f24, f28, f32, f36; };
extern "C" void FUN_08922fe0(Obj *o)
{
  o->f12 = 708;
  o->f36 = 31;
  o->f24 = 32;
  o->f32 = 33;
  o->f28 = 34;
  FUN_08baa4fc(&o->f16, &o->f20);
  o->f20 = ((unsigned)(FUN_089ecaa8()->f440 - 2) >= 2);
}
