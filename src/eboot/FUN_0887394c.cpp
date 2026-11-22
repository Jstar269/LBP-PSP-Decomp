/* LBPMATCH module=EBOOT.BIN start=0x0887394c end=0x08873a2c symbol=FUN_0887394c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned int, void *p) { return p; }
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" char DAT_08bb0578[];
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_08af767c(void *p);
struct Obj {
  virtual void v0();
  virtual void destroy();
  int refs; /* +4 */
};
struct Handle {
  Obj *p;
  Handle(Obj *const *src) {
    Obj *o = *src;
    p = o;
    if (o) {
      FUN_08ad3088(&DAT_08bef8f4);
      o->refs++;
      FUN_08ad2810(&DAT_08bef8f4);
    }
  }
};
struct Self {
  void *vtbl;       /* +0x00 */
  char f4;
  char pad5[3];
  int f8;
  int fc;
  char f10;
  char pad11[3];
  int f14;
  char sub18[0x60]; /* +0x18 */
  char sub78[0x60]; /* +0x78 */
  Handle a;         /* +0xd8 */
  Handle b;         /* +0xdc */
  int fe0;
  int fe4;
};
extern "C" void FUN_0887394c(Self *self, Obj **pa, Obj **pb)
{
  self->vtbl = DAT_08bb0578;
  self->f14 = 0;
  FUN_08af767c(self->sub18);
  FUN_08af767c(self->sub78);
  new (&self->a) Handle(pa);
  new (&self->b) Handle(pb);
  self->fe0 = 0;
  self->fe4 = 0;
  self->f4 = 0;
  self->f8 = 0;
  self->fc = 0;
  self->f10 = 0;
}
