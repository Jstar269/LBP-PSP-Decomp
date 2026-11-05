/* LBPMATCH module=EBOOT.BIN start=0x089ea43c end=0x089ea4a4 symbol=FUN_089ea43c lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Object;
extern "C" void FUN_08a9c014(Object *object);
extern "C" void FUN_08a9cf68(Object *object, const char *name, void *b, void *a, int flags);
extern "C" void FUN_08aaa9a0(Object *object);
extern "C" const char DAT_08bc2584[];
extern "C" int DAT_08bdbe2c;

extern "C" Object *FUN_089ea43c(Object *object, void *a, void *b)
{
  FUN_08a9c014(object);
  FUN_08a9cf68(object, DAT_08bc2584, b, a, DAT_08bdbe2c);
  FUN_08aaa9a0(object);
  return object;
}
