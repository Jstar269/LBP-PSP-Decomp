/* LBPMATCH module=EBOOT.BIN start=0x08845fbc end=0x0884600c symbol=FUN_08845fbc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08843f48(void *p);

extern void *DAT_08bae6e0[];
extern char DAT_08bedb94[];
struct Obj { void **vt; int pad[12]; void *buf; };
extern "C" void FUN_08845fbc(Obj *o)
{
  o->vt = DAT_08bae6e0;
  if (o->buf != DAT_08bedb94 && o->buf != 0) FUN_08a9e0c4(o->buf);
  FUN_08843f48(o);
}
