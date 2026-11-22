/* LBPMATCH module=EBOOT.BIN start=0x0895cacc end=0x0895cba8 symbol=FUN_0895cacc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Arr { int pad; char *base; };
extern void *DAT_08bf03b8;
extern int FUN_0896f3c0(void *o);
extern int FUN_0896f800(void *o);
extern void *FUN_0895a96c(void);
extern void FUN_08a9c164(void *dst, void *src);
extern void FUN_089597a8(void *a, void *b, int c, int d, int e, int f, int g);
extern void FUN_08a9bff8(void *s);

void FUN_0895cacc(struct Arr *self, int i)
{
  int buf[8];
  void *c = DAT_08bf03b8;
  int has = *(int *)(self->base + i * 0x60) != 0;
  unsigned char flag = 0;
  if (FUN_0896f3c0(c) != 0) {
    if (FUN_0896f800(c) == 2 && !has)
      flag = 1;
  }
  void *o = FUN_0895a96c();
  FUN_08a9c164(buf, self->base + i * 0x60 + 4);
  FUN_089597a8(o, buf, has, 0, flag, 0, 0);
  FUN_08a9bff8(buf);
}
