/* LBPMATCH module=EBOOT.BIN start=0x088906d4 end=0x08890734 symbol=FUN_088906d4 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08b898a0(void *node);
extern "C" void FUN_08890624(void *ctx, void *item);
struct Tree { int cmp; char header[8]; void *leftmost; };
struct Obj { int pad[4]; Tree tree; };
extern "C" void FUN_088906d4(Obj *o, void *ctx)
{
  Tree *t = &o->tree;
  for (void *n = t->leftmost; n != (void *)t->header; n = FUN_08b898a0(n))
    FUN_08890624(ctx, (char *)n + 36);
}
