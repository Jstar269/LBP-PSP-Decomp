/* LBPMATCH module=EBOOT.BIN start=0x08909aa8 end=0x08909b1c symbol=FUN_08909aa8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9e0c4(void *p);
extern "C" void FUN_08adb8a4(void *p);
struct Rc { virtual void v0(); int cnt; };
struct Obj { Rc *rc; void *buf; };
extern "C" void FUN_08909aa8(Obj *o)
{
  if (o->buf) FUN_08a9e0c4(o->buf);
  Rc *r = o->rc;
  if (r && --r->cnt == 0) {
    r->v0();
    FUN_08adb8a4(r);
  }
}
