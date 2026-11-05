/* LBPMATCH module=EBOOT.BIN start=0x088050b8 end=0x08805150 symbol=FUN_088050b8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08aab090(void *g, void *p);
extern void *DAT_08be84a8;
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); int f4, f8; int pad[2]; int lock[1]; };
extern "C" void FUN_088050b8(Obj *o)
{
  FUN_08ad3088(o->lock);
  int c = o->f8 - 1;
  o->f8 = c;
  FUN_08ad2810(o->lock);
  if (c == 0) {
    FUN_08aab090(DAT_08be84a8, &o->f4);
    o->v2();
    o->v1();
  }
}
