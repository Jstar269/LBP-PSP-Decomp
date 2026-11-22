/* LBPMATCH module=EBOOT.BIN start=0x0895cce4 end=0x0895cdc4 symbol=FUN_0895cce4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Arr { int pad; char *base; };
extern void *DAT_08bedb54;
extern int FUN_0884224c(void *o, int i);
extern unsigned char *FUN_0895a96c(void);
extern void FUN_0895b95c(void *p, int a, int b);
extern void FUN_08a9c164(void *dst, void *src);
extern void FUN_089597a8(void *a, void *b, int c, int d, int e, int f, int g);
extern void FUN_08a9bff8(void *s);

void FUN_0895cce4(struct Arr *self, int i)
{
  int buf[8];
  int *e;
  if (FUN_0884224c(DAT_08bedb54, i) == 0) {
    e = (int *)(self->base + i * 0x60);
    int v = e[22] != 0;
    unsigned char *o = FUN_0895a96c();
    if (o[80] != 0)
      FUN_0895b95c(FUN_0895a96c(), 0, 0);
    if (FUN_0884224c(DAT_08bedb54, i) != 0)
      v = 0;
    o = FUN_0895a96c();
    FUN_08a9c164(buf, e + 1);
    FUN_089597a8(o, buf, *e != 0, 0, 0, v, 0);
    FUN_08a9bff8(buf);
  }
}
