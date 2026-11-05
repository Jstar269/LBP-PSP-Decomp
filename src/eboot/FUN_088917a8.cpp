/* LBPMATCH module=EBOOT.BIN start=0x088917a8 end=0x08891808 symbol=FUN_088917a8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad0d68(void *p);
extern "C" void FUN_08ad0ee4(void *p, void *q);
extern "C" void FUN_08ad089c(void *p);
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_088874d8(void *p);
struct Obj { void *buf; int pad[1]; void *h; /* +8 */ char sub0[1]; char pad1[47]; char sub2[1]; /* +60 */ };
extern "C" void FUN_088917a8(Obj *o)
{
  if (o->h) {
    FUN_08ad0d68(o->h);
    FUN_08ad0ee4(o->h, 0);
  }
  FUN_08ad089c(&o->sub0);
  if (o->buf) FUN_08a9e0c4(o->buf);
  FUN_088874d8(o->sub2);
}
