/* LBPMATCH module=EBOOT.BIN start=0x088918b8 end=0x0889190c symbol=FUN_088918b8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char *FUN_08a9e824(int n);
extern "C" void FUN_08b9e7a8(void *dst, const void *src, int n);
struct Obj { char *buf; int size; };
extern "C" void FUN_088918b8(Obj *o, const void *src, int n)
{
  o->size = n;
  char *b = FUN_08a9e824(n);
  o->buf = b;
  FUN_08b9e7a8(b, src, n);
}
