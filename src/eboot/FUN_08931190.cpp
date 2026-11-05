/* LBPMATCH module=EBOOT.BIN start=0x08931190 end=0x089311f0 symbol=FUN_08931190 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *l);
extern "C" void FUN_08ad2810(void *l);
extern "C" void FUN_08adbc24(void *o, unsigned a, int b);
struct Obj { int pad[8]; char lock[1]; };
extern "C" void FUN_08931190(Obj *o, unsigned a, int b)
{
  FUN_08ad3088(o->lock);
  FUN_08adbc24(o, a, b);
  FUN_08ad2810(o->lock);
}
