/* LBPMATCH module=EBOOT.BIN start=0x089da7d4 end=0x089da834 symbol=FUN_089da7d4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_089d96fc(void *o);
extern "C" void FUN_0896b048(void *p, int x);
struct Obj { int pad[1]; unsigned flags; int pad2[16]; void *f72; };
extern "C" void FUN_089da7d4(Obj *o)
{
  if (o->f72) {
    int a = 0;
    if (FUN_089d96fc(o)) {
      a = 1;
      if (o->flags & 0x400) a = 0;
    }
    FUN_0896b048(o->f72, a);
  }
}
