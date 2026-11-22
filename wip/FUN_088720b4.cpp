/* LBPMATCH module=EBOOT.BIN start=0x088720b4 end=0x0887216c symbol=FUN_088720b4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_08873454(int a);
extern "C" void FUN_08a9df6c(void *p);
extern "C" void FUN_0888256c();
struct Self {
  virtual void v0();
  virtual void destroy();
  virtual void v2();
  virtual void v3();
  virtual void a();
  int refs;  /* +4 */
  int cnt;   /* +8 */
  char pad[0x24];
  int f30;   /* +0x30 */
  char pad2[0x5c];
  char flag; /* +0x90 */
  char pad3[3];
  void *p94; /* +0x94 */
};
struct Info { char pad[0x34]; int flag; };
struct Owner { Info *info; };
extern "C" void FUN_088720b4(Owner *o, Self *self)
{
  void *p;
  if (*(int *)((char *)*(void **)o + 0x34) == 0) {
    FUN_08873454(self->f30);
    self->flag = 1;
    p = self->p94;
  } else {
    self->a();
    p = self->p94;
  }
  FUN_08a9df6c(p);
  self->p94 = 0;
  FUN_08ad3088(&DAT_08bef8f4);
  int r = self->refs - 1;
  self->cnt = self->cnt - 1;
  self->refs = r;
  FUN_08ad2810(&DAT_08bef8f4);
  if (r == 0)
    self->destroy();
  FUN_0888256c();
}
// NEAR: 3 words differ. Only the then-block schedule: ours lw a0,148(s2); li v0,1; j; sb (delay), retail li; sb; j; lw a0 (delay slot).
// Ruled out: loading p94 before/after the flag store in source (h2), loading it in the join (34 differ, dbr fills with sb instead of the lw).
// near miss from lane api-sonnet-20261121 (unreviewed)
