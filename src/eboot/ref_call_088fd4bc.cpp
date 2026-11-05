/* LBPMATCH module=EBOOT.BIN start=0x088fd4bc end=0x088fd574 symbol=FUN_088fd4bc lang=c++ rodata=0x08bab5d8 */
/* Clone of src/eboot/ref_call_088fcf54.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp8c, Claude Sonnet 5.5; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
struct Res {
  virtual void v0();
  virtual void v1();
  virtual void release();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void setScale(float f);
  int rc;
};
struct World { char pad[0x24c]; int cam; };
struct Owner { char pad[0x18]; char flag; char pad2[3]; Res *res; };
extern "C" World *DAT_08bf7304;
extern "C" void FUN_0891074c(Owner *);
extern "C" int FUN_08a50fbc();
extern "C" void FUN_08a2f7f8(float, int, Res **, char);
extern "C" void FUN_088fd4bc(Owner *o)
{
  FUN_0891074c(o);
  FUN_08a50fbc();
  o->flag = 0;
  World *w = DAT_08bf7304;
  o->res->setScale(0.0f);
  Res *r = o->res;
  int cam = w->cam;
  if (r) r->rc++;
  FUN_08a2f7f8(0.1f, cam, &r, 0);
  if (r && --r->rc == 0)
    r->release();
}
