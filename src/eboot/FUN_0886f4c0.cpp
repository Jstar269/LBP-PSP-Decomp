/* LBPMATCH module=EBOOT.BIN start=0x0886f4c0 end=0x0886f568 symbol=FUN_0886f4c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(unsigned n);
extern "C" void FUN_08954bb4(void *p);
extern "C" int FUN_08882440(int k);
extern "C" int FUN_08882458(int k);
struct Obj {
  char pad[8];
  int f12; /* +0x0c */
  int f16; /* +0x10 */
  virtual void v0();
  virtual void v1();
  virtual void v2(int a);
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
};
struct Self {
  char pad[0x88];
  Obj *obj;  /* +0x88 */
  char pad2[0x0c];
  int kind;  /* +0x98 */
};
extern "C" void FUN_0886f4c0(void *unused, int arg, Self *self)
{
  Obj *o = (Obj *)FUN_08a9e838(20);
  FUN_08954bb4(o);
  int k = self->kind;
  self->obj = o;
  if (k != 3) {
    int a = FUN_08882440(k);
    Obj *p = self->obj;
    o->f12 = a;
    p->f16 = FUN_08882458(k);
  }
  self->obj->v2(arg);
  self->obj->v8();
}
