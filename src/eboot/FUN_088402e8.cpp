/* LBPMATCH module=EBOOT.BIN start=0x088402e8 end=0x0884034c symbol=FUN_088402e8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08ae4754(void);
struct Sink { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void notify(); };
struct Holder { int pad[42]; Sink *sink; };
struct Event {
  int pad[4];
  Holder *holder;                       /* +0x10 */
  void (*cb)(void *, Event *, int);     /* +0x14 */
  void *arg;                            /* +0x18 */
};
struct Obj { void **vt; };
extern "C" void FUN_088402e8(Obj *self, Event *ev)
{
  void *id = self->vt[16];
  bool same = false;
  if (id == FUN_08ae4754()) {
    ev->holder->sink->notify();
    same = true;
  }
  ev->cb(ev->arg, ev, same);
}
