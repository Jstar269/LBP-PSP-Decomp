/* LBPMATCH module=EBOOT.BIN start=0x08944b38 end=0x08944bf0 symbol=FUN_08944b38 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void destroy();
  int refs;
};
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" char DAT_08bb7454, DAT_08bb7440, DAT_08bb74f8, DAT_08bb7508;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0894a260(void *);
struct Self {
  void *vt0;
  int pad;
  void *vt8;
  int pad2;
  Obj *obj;
};
extern "C" void FUN_08944b38(Self *s)
{
  Obj *o = s->obj;
  s->vt8 = &DAT_08bb7454;
  s->vt0 = &DAT_08bb7440;
  if (o) {
    FUN_08ad3088(&DAT_08bef8f4);
    int r = o->refs - 1;
    o->refs = r;
    FUN_08ad2810(&DAT_08bef8f4);
    if (r == 0) o->destroy();
  }
  s->vt8 = &DAT_08bb74f8;
  FUN_0894a260(&s->vt8);
  s->vt0 = &DAT_08bb7508;
  FUN_0894a260(s);
}
