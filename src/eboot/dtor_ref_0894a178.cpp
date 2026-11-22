/* LBPMATCH module=EBOOT.BIN start=0x0894a178 end=0x0894a210 symbol=FUN_0894a178 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void destroy();
  int refs;
};
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" char DAT_08bb7740, DAT_08bb7750;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0894a260(void *);
struct Self {
  void *vt;
  int pad;
  Obj *obj;
};
extern "C" void FUN_0894a178(Self *s)
{
  Obj *o = s->obj;
  s->vt = &DAT_08bb7740;
  if (o) {
    FUN_08ad3088(&DAT_08bef8f4);
    int r = o->refs - 1;
    o->refs = r;
    FUN_08ad2810(&DAT_08bef8f4);
    if (r == 0) o->destroy();
  }
  s->vt = &DAT_08bb7750;
  FUN_0894a260(s);
}
