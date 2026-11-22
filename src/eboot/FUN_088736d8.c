/* LBPMATCH module=EBOOT.BIN start=0x088736d8 end=0x08873774 symbol=FUN_088736d8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Self {
  void *vtbl;      /* +0x00 */
  int pad4[2];
  int state;       /* +0x0c */
  int pad10[8];
  int f30;
  int f34;
  int f38;
  int f3c;
};
extern char DAT_08bb0548[];
extern void FUN_0886e018(struct Self *self, int a);
extern void *FUN_08a9e824(unsigned n);
extern void *FUN_08b9e7a8(void *d, const void *s, unsigned n);
extern void FUN_0886cb2c(struct Self *self, void *p, unsigned n, int f);
void FUN_088736d8(struct Self *self, unsigned n, const void *src)
{
  void *buf;
  FUN_0886e018(self, 0);
  self->vtbl = DAT_08bb0548;
  self->f30 = 0;
  self->f34 = 0;
  self->f38 = 0;
  self->f3c = 0;
  buf = FUN_08a9e824(n);
  FUN_08b9e7a8(buf, src, n);
  FUN_0886cb2c(self, buf, n, 1);
  self->state = 2;
}
