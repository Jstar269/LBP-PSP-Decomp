/* LBPMATCH module=EBOOT.BIN start=0x08870980 end=0x08870a38 symbol=FUN_08870980 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
struct Obj {
  virtual void v0();
  virtual void destroy();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  int refs; /* +4 */
};
struct Ctx {
  char pad[0x30];
  char map[1]; /* +0x30 */
};
extern "C" void *FUN_08a9e838(unsigned n);
extern "C" void FUN_0887367c(void *mem, int a);
extern "C" void FUN_0886daf8(void *map, Obj **p);
extern "C" void FUN_08870980(void *unused, int a, Ctx *c)
{
  Obj *o = (Obj *)FUN_08a9e838(68);
  FUN_0887367c(o, a);
  Obj *local = o;
  FUN_0886daf8(c->map, &local);
  Obj *p = local;
  if (p) {
    FUN_08ad3088(&DAT_08bef8f4);
    int r = p->refs - 1;
    p->refs = r;
    FUN_08ad2810(&DAT_08bef8f4);
    if (r == 0)
      p->destroy();
  }
  o->v7();
}
