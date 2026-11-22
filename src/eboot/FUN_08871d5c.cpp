/* LBPMATCH module=EBOOT.BIN start=0x08871d5c end=0x08871e28 symbol=FUN_08871d5c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_088706f4(void *);
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_08871a38(void *self);
extern "C" void FUN_0888256c();
struct Self;
struct Src {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7(void (*f)(void *), Self *p, int x, int y);
};
struct Self {
  virtual void v0();
  virtual void destroy();
  virtual void v2();
  virtual void v3();
  virtual void a();
  virtual void b();
  int refs;  /* +4 */
  int cnt;   /* +8 */
  char pad[0x7c];
  Src *src;  /* +0x88 */
};
struct Info { char pad[0x34]; int flag; };
struct Owner { Info *info; };
extern "C" void FUN_08871d5c(Owner *o, Self *self)
{
  if (*(int *)((char *)*(void **)o + 0x34) == 0) {
    self->src->v7(FUN_088706f4, self, 0, 1);
    FUN_08871a38(self);
  } else {
    self->a();
  }
  FUN_08ad3088(&DAT_08bef8f4);
  int r = self->refs - 1;
  self->cnt = self->cnt - 1;
  self->refs = r;
  FUN_08ad2810(&DAT_08bef8f4);
  if (r == 0)
    self->destroy();
  FUN_0888256c();
}
