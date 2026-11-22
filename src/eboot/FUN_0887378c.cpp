/* LBPMATCH module=EBOOT.BIN start=0x0887378c end=0x08873864 symbol=FUN_0887378c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" char DAT_08bb0578[];
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_08a9e0cc(void *self);
struct Obj {
  virtual void v0();
  virtual void destroy();
  int refs; /* +4 */
};
struct Handle {
  Obj *p;
  ~Handle() {
    Obj *o = p;
    if (o) {
      FUN_08ad3088(&DAT_08bef8f4);
      int r = o->refs - 1;
      o->refs = r;
      FUN_08ad2810(&DAT_08bef8f4);
      if (r == 0)
        o->destroy();
    }
  }
};
struct Self {
  void *vtbl;
  char pad[0xd4];
  Handle a; /* +0xd8 */
  Handle b; /* +0xdc */
};
extern "C" void FUN_0887378c(Self *self)
{
  self->vtbl = DAT_08bb0578;
  self->b.~Handle();
  self->a.~Handle();
  FUN_08a9e0cc(self);
}
