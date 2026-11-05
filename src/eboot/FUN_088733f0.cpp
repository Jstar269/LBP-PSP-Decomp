/* LBPMATCH module=EBOOT.BIN start=0x088733f0 end=0x08873454 symbol=FUN_088733f0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08a9e0cc(void *p);
extern "C" void FUN_0886cd04(void *p);
extern void *DAT_08bb0548[];
struct Obj { void **vt; int pad[12]; void (*release)(void *); /* +0x34 */ void *buf; /* +0x38 */ void *res; /* +0x3c */ };
extern "C" void FUN_088733f0(Obj *o)
{
  o->vt = DAT_08bb0548;
  if (o->buf) FUN_08a9e0c4(o->buf);
  if (o->res) o->release(o->res);
  FUN_0886cd04(o);
  FUN_08a9e0cc(o);
}
