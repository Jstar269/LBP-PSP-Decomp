/* LBPMATCH module=EBOOT.BIN start=0x088b46b0 end=0x088b4714 symbol=FUN_088b46b0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a49c(void *p);
extern "C" void FUN_08ad3088(void *p);
extern "C" void FUN_08ad2810(void *p);
extern "C" void FUN_088b2fb8(void *p, int a, int b);
extern int DAT_08bef8f4, DAT_08be8430;
struct Obj { int pad[5]; char sub[1]; };
extern "C" void FUN_088b46b0(Obj *o)
{
  FUN_0894a49c(o->sub);
  FUN_08ad3088(o->sub);
  FUN_08ad3088(&DAT_08bef8f4);
  FUN_088b2fb8(&DAT_08be8430, 5, 0);
  FUN_08ad2810(&DAT_08bef8f4);
  FUN_08ad2810(o->sub);
}
